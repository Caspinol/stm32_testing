/*
	Linker script for stm32f4 based on ecample scripts 
	in the SPL library
*/

ENTRY(Reset_Handler)

/* Highest address of the user mode stack */

ram_stack_buffer = 0x40; /* 64 bytes of safety buffer between RAM and stack in case of overflow */
_Min_Stack_Size = 0x500; /* required amount of stack 1280 bytes*/

MEMORY
{
flash (rx)      : ORIGIN = 0x08000000, LENGTH = 256K
ram (xrw)      : ORIGIN = 0x20000000, LENGTH = 64K
}

topOfStack = ORIGIN(ram) + LENGTH(ram); 

SECTIONS
{
  /* Interrupt vector first */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  } >flash

  /* Program code */
  .text :
  {
    . = ALIGN(4);
    *(.text)           /* code */
    *(.text*)          /* code */
    *(.glue_7)         /* glue arm to thumb code (Not sure what this is yet */
    *(.glue_7t)        /* glue thumb to arm code (Not sure what this is yet */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;        /* end of text section */
  } >flash

  /* Constant data */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata) 
    *(.rodata*)
    . = ALIGN(4);
  } >flash

  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >flash
  .ARM : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >flash

  .preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >flash
  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >flash
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >flash

  _sidata = LOADADDR(.data);

  /* Initialized data */
  .data : 
  {
    . = ALIGN(4);
    _sdata = .;
    *(.data)   
    *(.data*)  

    . = ALIGN(4);
    _edata = .;        /* end of data section */
  } >ram AT> flash

  
  /* Uninitialized data */
  . = ALIGN(4);
  .bss :
  {
    _sbss = .;         /* startup code will need this */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;         /* End of bss sectoin */
    __bss_end__ = _ebss;
  } >ram
 
 .stack :	      	      
  {
    . = ALIGN(4);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + ram_stack_buffer; /* Just a small buffer in case of stack overflow */
    . = . + _Min_Stack_Size;
    . = ALIGN(4);
  } >ram

  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
