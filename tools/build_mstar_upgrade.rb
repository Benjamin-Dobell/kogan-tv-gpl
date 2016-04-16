#!/usr/bin/env ruby

require 'ostruct'

MINIMUM_RUBY_COMPONENTS = [1, 9, 10] # We're taking advantage of ordered hashes
version_components = RUBY_VERSION.split('.').map &:to_i

version_components.each_index do |index|
  break if version_components[index] > MINIMUM_RUBY_COMPONENTS[index]

  if version_components[index] < MINIMUM_RUBY_COMPONENTS[index]
    STDERR.puts "Ruby 1.9.0 or higher is required"
    exit 1
  end
end

if ARGV.length < 2
  STDERR.puts "Usage: #{$PROGRAM_NAME} <firmware info path> <output MstarUpgrade path>"
  exit 1
end

SCRIPT_SIZE = 16384
FILE_PART_LOAD_ADDRESS = "50000000"
MAX_FILE_PART_SIZE = 209715200
BLOCK_SIZE = 512

firmware_info_path = ARGV[0]
output_path = ARGV[1]

file_root_path = File.dirname firmware_info_path

partitions = {}
env_vars = {}

lines = File.open(firmware_info_path).readlines
line_type = :unknown

offset = SCRIPT_SIZE

lines.each_index do |line_no|
  line = lines[line_no]

  if line == "##### PARTITIONS #####\n"
    line_type = :partition
    puts "Reading partition list..."
  elsif line === "##### ENV #####\n"
    line_type = :env
    puts "Reading env vars..."
  elsif line_type == :partition
    if m = /(\w+) (\h+)(?: (\S+)(?: ([a-zA-Z]+))?)?\s*\n/.match(line)
      name = m[1]
      size = m[2].to_i(16)
      path = File.join(file_root_path, m[3]) if m[3]
      compression = m[4] ? m[4].downcase.to_sym : :none

      if partitions[name]
        STDERR.puts "ERROR: Line #{line_no}:\nDuplicate entries for partition #{name}"
        exit 1
      end

      if path
        unless compression == :none || compression == :lzo
          STDERR.puts "ERROR: Line #{line_no}:\nUnknown compression algorithm #{compression}"
          exit 1
        end

        unless File.exist? path
          STDERR.puts "ERROR: Line #{line_no}:\n#{path} could not be found"
          exit 1
        end

        file_size = File.size? path

        unless file_size <= size
          STDERR.puts "ERROR: Line #{line_no}:\n#{name} partition is not large enough to accomodate #{path}"
          exit 1
        end

        unless file_size > 0
          STDERR.puts "ERROR: Line #{line_no}:\n#{path} is an empty file"
          exit 1
        end

        if compression != :none && file_size > MAX_FILE_PART_SIZE
          STDERR.puts "ERROR: Line #{line_no}:\nCompressed files must be no larger than #{MAX_FILE_PART_SIZE} bytes"
          exit 1
        end

        partitions[name] = OpenStruct.new({
          size: size,
          path: path,
          compression: compression,
          file_size: file_size,
          offset: offset
        })

        offset += file_size
        offset += BLOCK_SIZE - offset % BLOCK_SIZE if offset % BLOCK_SIZE != 0
      else
        partitions[name] = OpenStruct.new({
          size: size,
          path: nil
        })
      end
    elsif line != "\n"
      STDERR.puts "ERROR: Line #{line_no}:\nInvalid partition specification:\n#{line}"
      exit 1
    end
  elsif line_type == :env
    if m = /([a-zA-Z_]+[a-zA-Z0-9_]*)\s*=\s*"?(.+?)"?\s*\n/.match(line)
      name = m[1]
      value = m[2]

      if env_vars[env_vars]
        STDERR.puts "ERROR: Line #{line_no}:\nDuplicate definitions of env var #{name}"
        exit 1
      end

      env_vars[name] = value
    elsif line != "\n"
      STDERR.puts "ERROR: Line #{line_no}:\nInvalid env var definition:\n#{line}"
      exit 1
    end
  end
end

env_vars["MstarUpgrade_complete"] = "1"

File.open(output_path, 'w+b') do |file|
  # Instruction to set SLC (single level cell) size to zero i.e. disable SLC
  file.write "mmc slc 0 1\n"

  # Instruction to clear the existing partition table
  file.write "mmc rmgpt\n"

  # Instruction to erase all blocks
  file.write "mmc erase\n"

  # Instructions to create partitions
  partitions.each do |name, info|
    file.write "mmc create #{name} 0x#{info.size.to_s 16}\n"
  end

  # Instructions to write partitions
  file_partitions = partitions.keys.select { |k| partitions[k].path }

  file_partitions.each do |name|
    info = partitions[name]

    if info.file_size > MAX_FILE_PART_SIZE
      file.write "mmc erase.p #{name}\n"

      read_offset = info.offset
      write_block_offset = 0
      remaining_size = info.file_size

      loop do
        load_size = [remaining_size, MAX_FILE_PART_SIZE].min
        file.write "filepartload #{FILE_PART_LOAD_ADDRESS} MstarUpgrade.bin #{read_offset.to_s 16} #{load_size.to_s 16}\n"
        file.write "mmc write.p.continue #{FILE_PART_LOAD_ADDRESS} #{name} #{write_block_offset.to_s 16} #{load_size.to_s 16} 1\n"

        remaining_size -= load_size
        break if remaining_size == 0

        read_offset += load_size
        write_block_offset += load_size / BLOCK_SIZE
      end
    else
      file.write "filepartload #{FILE_PART_LOAD_ADDRESS} MstarUpgrade.bin #{info.offset.to_s 16} #{info.file_size.to_s 16}\n"
      file.write "mmc erase.p #{name}\n"

      if info.compression == :lzo
        file.write "mmc unlzo #{FILE_PART_LOAD_ADDRESS} #{info.file_size.to_s 16} #{name} 1\n"
      else
        file.write "mmc write.p #{FILE_PART_LOAD_ADDRESS} #{name} #{info.file_size.to_s 16} 1\n"
      end
    end
  end

  env_vars.each do |k, v|
    file.write "setenv #{k} #{v}\n"
  end

  file.write "saveenv\n"
  file.write "printenv\n"
  file.write "reset\n"
  file.write "% <-this is end of file symbol\n"

  script_length = file.pos

  if file.pos > SCRIPT_SIZE
      STDERR.puts "ERROR: Generated script exceeded the maximum script length"
      exit 1
  end

  file.write("\xff" * (SCRIPT_SIZE - file.pos))

  file_partitions.each_index do |index|
    name = file_partitions[index]
    info = partitions[name]
    file_end = info.offset + info.file_size

    file.write(IO.binread(info.path))
    raise "Internal error - file offset inconsistency with #{name}. Expected #{file_end} but is #{file.pos}" unless file.pos == file_end

    if index + 1 < file_partitions.length
      padding_length = partitions[file_partitions[index + 1]].offset - file_end
      file.write("\xff" * padding_length)
    end
  end

  end_pos = file.pos

  file.seek 0
  poor_mans_consistency_check = file.read 16

  file.seek end_pos
  file.write poor_mans_consistency_check
end
