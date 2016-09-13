OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(bl2_entrypoint)
MEMORY {
    RAM (rwx): ORIGIN = (((0x4000000 + 0x00001000) + (0x40000 - 0x00001000) - 0x1D000) - 0xC000), LENGTH = ((0x4000000 + 0x00001000) + (0x40000 - 0x00001000) - 0x1D000) - (((0x4000000 + 0x00001000) + (0x40000 - 0x00001000) - 0x1D000) - 0xC000)
}
SECTIONS
{
    . = (((0x4000000 + 0x00001000) + (0x40000 - 0x00001000) - 0x1D000) - 0xC000);
    ASSERT(. == ALIGN(4096),
           "BL2_BASE address is not aligned on a page boundary.")
    ro . : {
        __RO_START__ = .;
        *bl2_entrypoint.o(.text*)
        *(.text*)
        *(.rodata*)
        . = ALIGN(8);
        __PARSER_LIB_DESCS_START__ = .;
        KEEP(*(.img_parser_lib_descs))
        __PARSER_LIB_DESCS_END__ = .;
        *(.vectors)
        __RO_END_UNALIGNED__ = .;
        . = NEXT(4096);
        __RO_END__ = .;
    } >RAM
    __RW_START__ = . ;
    .data . : {
        __DATA_START__ = .;
        *(.data*)
        __DATA_END__ = .;
    } >RAM
    stacks (NOLOAD) : {
        __STACKS_START__ = .;
        *(tzfw_normal_stacks)
        __STACKS_END__ = .;
    } >RAM
    .bss : ALIGN(16) {
        __BSS_START__ = .;
        *(SORT_BY_ALIGNMENT(.bss*))
        *(COMMON)
        __BSS_END__ = .;
    } >RAM
    xlat_table (NOLOAD) : {
        *(xlat_table)
    } >RAM
    __RW_END__ = .;
    __BL2_END__ = .;
    __BSS_SIZE__ = SIZEOF(.bss);
    ASSERT(. <= ((0x4000000 + 0x00001000) + (0x40000 - 0x00001000) - 0x1D000), "BL2 image has exceeded its limit.")
}
