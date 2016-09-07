#!/usr/bin/env ruby
require 'set'

if ARGV.length < 1
  puts "Usage: #{$PROGRAM_NAME} <MstarUpgrade path>"
  exit 1
end

mstar_upgrade_path = ARGV[0]

output_base_path = File.basename mstar_upgrade_path, ".*"

script = File.read mstar_upgrade_path, 16384

processed_paths = Set.new

if script
  path = "#{output_base_path}.part.uboot-script"
  File.write path, script
  puts "Created #{path}"

  regex = /filepartload \h+ .*? (\h+) (\h+)\n.*?mmc (?:unlzo|write\.p)(?:.continue|.cont)? \h+ (?:\h+ )?(\w+)/m
  script.scan regex do |match|
    offset = match[0].to_i 16
    size = match[1].to_i 16
    name = match[2]

    path = "#{output_base_path}.part.#{name}"

    if processed_paths.include?(path)
      IO.binwrite path, IO.binread(mstar_upgrade_path, size, offset), File.size(path)
    else
      IO.binwrite path, IO.binread(mstar_upgrade_path, size, offset)
      puts "Created #{path}"
      processed_paths.add path
    end
  end
else
  puts "Could not read #{mstar_upgrade_path}"
  exit 1
end
