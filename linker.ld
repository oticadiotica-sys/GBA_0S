ENTRY(_start)

MEMORY
{
    rom  (rx)  : ORIGIN = 0x08000000, LENGTH = 32M
    ram  (rwx): ORIGIN = 0x03000000, LENGTH = 256K
}

SECTIONS
{
    . = ORIGIN(rom);

    .text :
    {
        KEEP(*(.vectors))
        *(.text*)
        *(.text.*)
        *(.rodata*)
        *(.rodata.*)
    } > rom

    .data :
    {
        __data_start__ = .;
        *(.data*)
        *(.data.*)
        __data_end__ = .;
    } > ram AT > rom

    .bss (NOLOAD) :
    {
        __bss_start__ = .;
        *(.bss*)
        *(.bss.*)
        *(COMMON)
        __bss_end__ = .;
    } > ram

    . = ALIGN(4);
    __end__ = .;

    /DISCARD/ :
    {
        *(.comment)
        *(.note*)
        *(.eh_frame*)
        *(.init)
        *(.fini)
    }
}