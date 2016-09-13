OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(bl31_entrypoint)
MEMORY {
    RAM (rwx): ORIGIN = ((0x4000000 + 0x00001000) + (0x40000 - 0x00001000) - 0x1D000), LENGTH = ((0x4000000 + 0x00001000) + (0x40000 - 0x00001000)) - ((0x4000000 + 0x00001000) + (0x40000 - 0x00001000) - 0x1D000)
}
SECTIONS
{
    . = ((0x4000000 + 0x00001000) + (0x40000 - 0x00001000) - 0x1D000);
    ASSERT(. == ALIGN(4096),
           "BL31_BASE address is not aligned on a page boundary.")
    ro . : {
        __RO_START__ = .;
        *bl31_entrypoint.o(.text*)
        *(.text*)
        *(.rodata*)
        . = ALIGN(8);
        __RT_SVC_DESCS_START__ = .;
        KEEP(*(rt_svc_descs))
        __RT_SVC_DESCS_END__ = .;
        . = ALIGN(8);
        __CPU_OPS_START__ = .;
        KEEP(*(cpu_ops))
        __CPU_OPS_END__ = .;
        *(.vectors)
        __RO_END_UNALIGNED__ = .;
        . = NEXT(4096);
        __RO_END__ = .;
    } >RAM
    ASSERT(__CPU_OPS_END__ > __CPU_OPS_START__,
           "cpu_ops not defined for this platform.")
    __RW_START__ = . ;
    .data . : {
        __DATA_START__ = .;
        *(.data*)
        __DATA_END__ = .;
    } >RAM
    ASSERT(. <= ((0x4000000 + 0x00001000) + (0x40000 - 0x00001000) - 0x6000), "BL31 progbits has exceeded its limit.")
    stacks (NOLOAD) : {
        __STACKS_START__ = .;
        *(tzfw_normal_stacks)
        __STACKS_END__ = .;
    } >RAM
    .bss (NOLOAD) : ALIGN(16) {
        __BSS_START__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN((1 << 6));
        __BAKERY_LOCK_START__ = .;
        *(bakery_lock)
        . = ALIGN((1 << 6));
        __PERCPU_BAKERY_LOCK_SIZE__ = ABSOLUTE(. - __BAKERY_LOCK_START__);
        . = . + (__PERCPU_BAKERY_LOCK_SIZE__ * ((2 + 0) - 1));
        __BAKERY_LOCK_END__ = .;
        __BSS_END__ = .;
    } >RAM
    xlat_table (NOLOAD) : {
        *(xlat_table)
    } >RAM
    __RW_END__ = .;
    __BL31_END__ = .;
    __BSS_SIZE__ = SIZEOF(.bss);
    ASSERT(. <= ((0x4000000 + 0x00001000) + (0x40000 - 0x00001000)), "BL31 image has exceeded its limit.")
}
