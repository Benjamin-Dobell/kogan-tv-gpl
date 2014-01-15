gawk '$3 == "kernel_entry" { print "#define KERNEL_ENTRY_ADDR 0x"$1 }' ../../../../../System.map > ice.h
