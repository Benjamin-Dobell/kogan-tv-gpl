OUTPUT(elf32-littlemips)
OUTPUT_ARCH(mips)

ENTRY(LzmaDec)
/*
MEMORY
{
	ram :	ORIGIN = 0x80C00000, LENGTH = 1M
}
*/

SECTIONS
{
    . = 0x80C00000;
   
    .text :
    {
        *(.text) 
        . = ALIGN(8);
    }
   
    .rodata :
    {
    
        *(.rodata) 
        *(.rodata.*)
    
    }
    .data : 
    {
		. = ALIGN(8);
		_ld_data_RAM_start = .;
        *(.data)
		. = ALIGN(8);
		_ld_data_RAM_end = .;
    }
    _gp = . + 0x8000; /* need init, if used */
    _gp_disp = .;
    .lit8 : { *(.lit8) }
    .lit4 : { *(.lit4) }
    .bss : {
      *(.bss)
      *(COMMON)
    }
    /DISCARD/ : {
        *(.exit.text)
        *(.exit.data)
        *(.exitcall.exit)

	/* ABI crap starts here */
	*(.comment)
	*(.MIPS.options)
	*(.note)
	*(.options)
	*(.pdr)
	*(.reginfo)
	*(.mdebug*)
  }

}
