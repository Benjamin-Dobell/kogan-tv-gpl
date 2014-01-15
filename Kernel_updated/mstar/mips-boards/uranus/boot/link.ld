OUTPUT(elf32-littlemips)
OUTPUT_ARCH(mips)

ENTRY(BOOT_Entry)

MEMORY
{
    boot :  ORIGIN = 0xBFC00000, LENGTH = 3K    /* 0x81C00000 */
    rom :   ORIGIN = 0x94000000+0xC00, LENGTH = 2K
    ram :   ORIGIN = 0x80900000, LENGTH = 2K
}

SECTIONS
{
    .text1 :
    {
        obj/boot.o (.text)
    } > boot

    .text2 : AT ( LOADADDR(.text1) + SIZEOF(.text1) )
    {
        obj/bootrom.o (.text)
        . = ALIGN(16);
        _ld_bootram_ROM_start = .;
    } > rom

    .text3 : AT ( LOADADDR(.text2) + SIZEOF(.text2) )
    {
        _ld_bootram_RAM_start = .;
        obj/bootram.o (.text)
        . = ALIGN(16);
        _ld_bootram_RAM_end = .;
    } > ram

    /* dummy */
    .data :
    {
        *(.data)
    } > ram

    .bss :
    {
        *(.bss)
    } > ram
    /DISCARD/ : {


	/* ABI crap starts here */

	*(.reginfo)
    }
}
