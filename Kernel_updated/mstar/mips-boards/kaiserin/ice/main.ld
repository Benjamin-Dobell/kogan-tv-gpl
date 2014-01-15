
OUTPUT_FORMAT("elf32-tradlittlemips")
OUTPUT_ARCH(mips)


STARTUP(vector.o)
/*ENTRY(reset_vector)*/
/*ENTRY(main)*/

/* GROUP(libgcc.a)*/
/* GROUP (strcpy.os)*/

/* GROUP (libc.a)*/

MEMORY
{
    /*ram : ORIGIN = 0x80000200, LENGTH = 0x01fffe00*/          /* 32MB */
    ram : ORIGIN = 0xA0000000, LENGTH = 0x00400000          /* 4MB */
    /*ram : ORIGIN = 0x80000200, LENGTH = 0x0100000*/           /* 1MB Cache*/
    /*ram : ORIGIN = 0xA0000200, LENGTH = 0x01fffe00*/      /* 32MB Non-Cache*/ /*32 M DRAM test*/
    /*ram : ORIGIN = 0xBFC00000, LENGTH = 0x30000*/         /*for very_boot_from_flash_and _run_in_flash.cmm */
                                                            /*Caution : need to specify Stack and buffer at DRAM  */

}

SECTIONS
{
  .text : { *(.text) } > ram
  .data : { *(.data) } > ram
  .bss : { *(.bss) } > ram
  _gp = ALIGN(16) + 0x7ff0;
  .reginfo        : { *(.reginfo) } > ram
/*  /DISCARD/ : { *(.reginfo) } */

   .stack :
    {
        *(.stack)
        /*_ld_stack_addr = .;*/
    } > ram
}
