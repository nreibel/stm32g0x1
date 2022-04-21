/* Label for the program's entry point */
ENTRY(reset_handler)

/* End of RAM / Start of stack*/
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* Must have at least 1kB RAM free */
_Min_Free_RAM = 0x400;

/* Memories definition */
MEMORY
{
  RAM   (xrw) : ORIGIN = 0x20000000, LENGTH = 36K
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 128K
}

SECTIONS
{
    /* The vector table goes at the start of flash. */
    .vector_table :
    {
        . = ALIGN(4);
        KEEP (*(.vector_table))
        . = ALIGN(4);
    } >FLASH

    /* The 'text' section contains the main program code. */
    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        . = ALIGN(4);
    } >FLASH

    /* Used by the startup to initialize data */
    _sidata = LOADADDR(.data);

    /* Initialized data sections into "RAM" Ram type memory */
    .data :
    {
        /* data start */
        . = ALIGN(4);
        _sdata = .;

        *(.data)
        *(.data*)

        /* data end */
        . = ALIGN(4);
        _edata = .;

    } >RAM AT> FLASH

    /* The 'bss' section is initialized to all 0s at the start of the program. */
    .bss :
    {
        /* bss start */
        . = ALIGN(4);
        _sbss = .;

        *(.bss)
        *(.bss*)

        /* bss end */
        . = ALIGN(4);
        _ebss = .;

    } >RAM

    /* Space set aside for the application's heap/stack. */
    .reserved :
    {
        . = ALIGN(4);
        . += _Min_Free_RAM;
        . = ALIGN(4);
    } >RAM
}
