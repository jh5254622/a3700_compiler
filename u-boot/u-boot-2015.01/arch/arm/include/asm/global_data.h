/*
 * (C) Copyright 2002-2010
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef	__ASM_GBL_DATA_H
#define __ASM_GBL_DATA_H

#ifdef CONFIG_MVEBU_SYS_INFO
#include <asm/arch-mvebu/system_info.h>
#endif

#ifdef CONFIG_OMAP
#include <asm/omap_boot.h>
#endif

#ifdef CONFIG_ENV_IS_IN_BOOTDEV
#include <env_bootdev.h>
#endif

/* Architecture-specific global data */
struct arch_global_data {
#if defined(CONFIG_FSL_ESDHC)
	u32 sdhc_clk;
#endif

#if defined(CONFIG_U_QE)
	u32 qe_clk;
	u32 brg_clk;
	uint mp_alloc_base;
	uint mp_alloc_top;
#endif /* CONFIG_U_QE */

#ifdef CONFIG_AT91FAMILY
	/* "static data" needed by at91's clock.c */
	unsigned long	cpu_clk_rate_hz;
	unsigned long	main_clk_rate_hz;
	unsigned long	mck_rate_hz;
	unsigned long	plla_rate_hz;
	unsigned long	pllb_rate_hz;
	unsigned long	at91_pllb_usb_init;
#endif
	/* "static data" needed by most of timer.c on ARM platforms */
	unsigned long timer_rate_hz;
	unsigned long tbu;
	unsigned long tbl;
	unsigned long lastinc;
	unsigned long long timer_reset_value;
#if !(defined(CONFIG_SYS_ICACHE_OFF) && defined(CONFIG_SYS_DCACHE_OFF))
	unsigned long tlb_addr;
	unsigned long tlb_size;
#endif

#ifdef CONFIG_OMAP
	struct omap_boot_parameters omap_boot_params;
#endif
#ifdef CONFIG_MVEBU
	void *soc_family;
	void *board_family;
	void *reg_base;
#ifdef CONFIG_MVEBU_SYS_INFO
	struct sys_info local_sys_info[MAX_OPTION];
#endif
#ifdef CONFIG_ENV_IS_IN_BOOTDEV
	struct env_func_ptr env_func;
#endif
#endif

};

#include <asm-generic/global_data.h>

#ifdef __clang__

#define DECLARE_GLOBAL_DATA_PTR
#define gd	get_gd()

static inline gd_t *get_gd(void)
{
	gd_t *gd_ptr;

#ifdef CONFIG_ARM64
	/*
	 * Make will already error that reserving x18 is not supported at the
	 * time of writing, clang: error: unknown argument: '-ffixed-x18'
	 */
	__asm__ volatile("mov %0, x18\n" : "=r" (gd_ptr));
#else
	__asm__ volatile("mov %0, r9\n" : "=r" (gd_ptr));
#endif

	return gd_ptr;
}

#else

#ifdef CONFIG_ARM64
#define DECLARE_GLOBAL_DATA_PTR		register volatile gd_t *gd asm ("x18")
#else
#define DECLARE_GLOBAL_DATA_PTR		register volatile gd_t *gd asm ("r9")
#endif
#endif

#endif /* __ASM_GBL_DATA_H */
