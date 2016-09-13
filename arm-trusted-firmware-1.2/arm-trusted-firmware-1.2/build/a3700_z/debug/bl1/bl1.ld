OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(bl1_entrypoint)
MEMORY {
    ROM (rx): ORIGIN = (0x4000000 + 0x100000), LENGTH = ((0x4000000 + 0x100000) + 0x04000000) - (0x4000000 + 0x100000)
    RAM (rwx): ORIGIN = ((0x4000000 + 0x00001000) + (0x40000 - 0x00001000) - 0x6000), LENGTH = ((0x4000000 + 0x00001000) + (0x40000 - 0x00001000)) - ((0x4000000 + 0x00001000) + (0x40000 - 0x00001000) - 0x6000)
}
SECTIONS
{
    . = (0x4000000 + 0x100000);
    ASSERT(. == ALIGN(4096),
           "BL1_RO_BASE address is not aligned on a page boundary.")
    ro . : {
        __RO_START__ = .;
        *bl1_entrypoint.o(.text*)
        *(.text*)
        *(.rodata*)
        . = ALIGN(8);
        __PARSER_LIB_DESCS_START__ = .;
        KEEP(*(.img_parser_lib_descs))
        __PARSER_LIB_DESCS_END__ = .;
        . = ALIGN(8);
        __CPU_OPS_START__ = .;
        KEEP(*(cpu_ops))
        __CPU_OPS_END__ = .;
        *(.vectors)
        __RO_END__ = .;
    } >ROM
    ASSERT(__CPU_OPS_END__ > __CPU_OPS_START__,
           "cpu_ops not defined for this platform.")
    . = ((0x4000000 + 0x00001000) + (0x40000 - 0x00001000) - 0x6000);
    ASSERT(. == ALIGN(4096),
           "BL1_RW_BASE address is not aligned on a page boundary.")
    .data . : ALIGN(16) {
        __DATA_RAM_START__ = .;
        *(.data*)
        __DATA_RAM_END__ = .;
    } >RAM AT>ROM
    stacks . (NOLOAD) : {
        __STACKS_START__ = .;
        *(tzfw_normal_stacks)
        __STACKS_END__ = .;
    } >RAM
    .bss : ALIGN(16) {
        __BSS_START__ = .;
        *(.bss*)
        *(COMMON)
        __BSS_END__ = .;
    } >RAM
    xlat_table (NOLOAD) : {
        *(xlat_table)
    } >RAM
    __BL1_RAM_START__ = ADDR(.data);
    __BL1_RAM_END__ = .;
    __DATA_ROM_START__ = LOADADDR(.data);
    __DATA_SIZE__ = SIZEOF(.data);
    ASSERT(__DATA_ROM_START__ + __DATA_SIZE__ <= ((0x4000000 + 0x100000) + 0x04000000),
           "BL1's RO section has exceeded its limit.")
    __BSS_SIZE__ = SIZEOF(.bss);
    ASSERT(. <= ((0x4000000 + 0x00001000) + (0x40000 - 0x00001000)), "BL1's RW section has exceeded its limit.")
}
