OUTPUT_ARCH(m68k)
OUTPUT_FORMAT(elf32-m68k)

ENTRY(_start)

MEMORY
{
  dram  (rwx) : ORIGIN = 0x40000000, LENGTH = 0x00800000
}

SECTIONS
{
  .text :
  {
    *(.text)
    *(.text*)
    *(.debug*)
    . = ALIGN (16);

    *(.eh_frame)
    . = ALIGN (16);

    *(.gnu.linkonce.t.*) 

    . = ALIGN(0x4);
    __CTOR_LIST__ = .;
    KEEP(*(.ctors))
    LONG(0)
    __DTOR_LIST__ = .;
    KEEP(*(.dtors))
     LONG(0)
    *(.rodata)
    *(.rodata*)
    *(.gcc_except_table) 

    . = ALIGN(0x2);
    KEEP(*(.init))
    KEEP(*(.fini))
    *(.lit)
    . = ALIGN(16);
  } > dram

  .data :
  {
    *(.shdata)
    *(.data)
    *(.gnu.linkonce.d.*)
    . = ALIGN (16);
  } > dram

  .bss :
  {
    . = ALIGN(0x4);
    *(.shbss)
    *(.bss)
    *(COMMON)
    . = ALIGN(0x4);
  } > dram
}

DRAM_START          = 0x40000000;
DRAM_END            = DRAM_START + 0x00800000;

TEXT_START          = ADDR(.text);
TEXT_END            = TEXT_START + SIZEOF(.text);
DATA_START          = ADDR(.data);
DATA_END            = DATA_START + SIZEOF(.data);
BSS_START           = ADDR(.bss);
BSS_END             = BSS_START + SIZEOF(.bss);

FREEMEM_START       = BSS_END;
FREEMEM_END         = DRAM_END;
