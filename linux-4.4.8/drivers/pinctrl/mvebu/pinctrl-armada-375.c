/*
 * Marvell Armada 375 pinctrl driver based on mvebu pinctrl core
 *
 * Copyright (C) 2012 Marvell
 *
 * Thomas Petazzoni <thomas.petazzoni@free-electrons.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/err.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/pinctrl/pinctrl.h>

#include "pinctrl-mvebu.h"

#define MVEBU_MPPS_PER_REG	8
#define MVEBU_MPP_BITS		4
#define MVEBU_MPP_MASK		0xf

static void __iomem *mpp_base;

static int armada_375_mpp_ctrl_get(unsigned pid, unsigned long *config)
{
	return default_mpp_ctrl_get(mpp_base, pid, config);
}

static int armada_375_mpp_ctrl_set(unsigned pid, unsigned long config)
{
	return default_mpp_ctrl_set(mpp_base, pid, config);
}

static struct mvebu_mpp_mode mv88f6720_mpp_modes[] = {
	MPP_MODE(0,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "dev", "ad2"),
		 MPP_FUNCTION(0x2, "spi0", "cs1"),
		 MPP_FUNCTION(0x3, "spi1", "cs1"),
		 MPP_FUNCTION(0x5, "nand", "io2")),
	MPP_MODE(1,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "dev", "ad3"),
		 MPP_FUNCTION(0x2, "spi0", "mosi"),
		 MPP_FUNCTION(0x3, "spi1", "mosi"),
		 MPP_FUNCTION(0x5, "nand", "io3")),
	MPP_MODE(2,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "dev", "ad4"),
		 MPP_FUNCTION(0x2, "ptp", "evreq"),
		 MPP_FUNCTION(0x3, "led", "c0"),
		 MPP_FUNCTION(0x4, "audio", "sdi"),
		 MPP_FUNCTION(0x5, "nand", "io4"),
		 MPP_FUNCTION(0x6, "spi1", "mosi")),
	MPP_MODE(3,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "dev", "ad5"),
		 MPP_FUNCTION(0x2, "ptp", "trig"),
		 MPP_FUNCTION(0x3, "led", "p3"),
		 MPP_FUNCTION(0x4, "audio", "mclk"),
		 MPP_FUNCTION(0x5, "nand", "io5"),
		 MPP_FUNCTION(0x6, "spi1", "miso")),
	MPP_MODE(4,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "dev", "ad6"),
		 MPP_FUNCTION(0x2, "spi0", "miso"),
		 MPP_FUNCTION(0x3, "spi1", "miso"),
		 MPP_FUNCTION(0x5, "nand", "io6")),
	MPP_MODE(5,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "dev", "ad7"),
		 MPP_FUNCTION(0x2, "spi0", "cs2"),
		 MPP_FUNCTION(0x3, "spi1", "cs2"),
		 MPP_FUNCTION(0x5, "nand", "io7"),
		 MPP_FUNCTION(0x6, "spi1", "miso")),
	MPP_MODE(6,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "dev", "ad0"),
		 MPP_FUNCTION(0x3, "led", "p1"),
		 MPP_FUNCTION(0x4, "audio", "lrclk"),
		 MPP_FUNCTION(0x5, "nand", "io0")),
	MPP_MODE(7,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "dev", "ad1"),
		 MPP_FUNCTION(0x2, "ptp", "clk"),
		 MPP_FUNCTION(0x3, "led", "p2"),
		 MPP_FUNCTION(0x4, "audio", "extclk"),
		 MPP_FUNCTION(0x5, "nand", "io1")),
	MPP_MODE(8,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "dev", "bootcs"),
		 MPP_FUNCTION(0x2, "spi0", "cs0"),
		 MPP_FUNCTION(0x3, "spi1", "cs0"),
		 MPP_FUNCTION(0x5, "nand", "ce")),
	MPP_MODE(9,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x2, "spi0", "sck"),
		 MPP_FUNCTION(0x3, "spi1", "sck"),
		 MPP_FUNCTION(0x5, "nand", "we")),
	MPP_MODE(10,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x2, "dram", "vttctrl"),
		 MPP_FUNCTION(0x3, "led", "c1"),
		 MPP_FUNCTION(0x5, "nand", "re"),
		 MPP_FUNCTION(0x6, "spi1", "sck")),
	MPP_MODE(11,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "dev", "a0"),
		 MPP_FUNCTION(0x3, "led", "c2"),
		 MPP_FUNCTION(0x4, "audio", "sdo"),
		 MPP_FUNCTION(0x5, "nand", "cle")),
	MPP_MODE(12,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "dev", "a1"),
		 MPP_FUNCTION(0x4, "audio", "bclk"),
		 MPP_FUNCTION(0x5, "nand", "ale")),
	MPP_MODE(13,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "dev", "ready"),
		 MPP_FUNCTION(0x2, "pcie0", "rstout"),
		 MPP_FUNCTION(0x3, "pcie1", "rstout"),
		 MPP_FUNCTION(0x5, "nand", "rb"),
		 MPP_FUNCTION(0x6, "spi1", "mosi")),
	MPP_MODE(14,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x2, "i2c0", "sda"),
		 MPP_FUNCTION(0x3, "uart1", "txd")),
	MPP_MODE(15,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x2, "i2c0", "sck"),
		 MPP_FUNCTION(0x3, "uart1", "rxd")),
	MPP_MODE(16,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x2, "uart0", "txd")),
	MPP_MODE(17,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x2, "uart0", "rxd")),
	MPP_MODE(18,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x2, "tdm", "int")),
	MPP_MODE(19,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x2, "tdm", "rst")),
	MPP_MODE(20,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x2, "tdm", "pclk")),
	MPP_MODE(21,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x2, "tdm", "fsync")),
	MPP_MODE(22,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x2, "tdm", "drx")),
	MPP_MODE(23,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x2, "tdm", "dtx")),
	MPP_MODE(24,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "led", "p0"),
		 MPP_FUNCTION(0x2, "ge1", "rxd0"),
		 MPP_FUNCTION(0x3, "sd", "cmd"),
		 MPP_FUNCTION(0x4, "uart0", "rts"),
		 MPP_FUNCTION(0x5, "spi0", "cs0"),
		 MPP_FUNCTION(0x6, "dev", "cs1")),
	MPP_MODE(25,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "led", "p2"),
		 MPP_FUNCTION(0x2, "ge1", "rxd1"),
		 MPP_FUNCTION(0x3, "sd", "d0"),
		 MPP_FUNCTION(0x4, "uart0", "cts"),
		 MPP_FUNCTION(0x5, "spi0", "mosi"),
		 MPP_FUNCTION(0x6, "dev", "cs2")),
	MPP_MODE(26,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "pcie0", "clkreq"),
		 MPP_FUNCTION(0x2, "ge1", "rxd2"),
		 MPP_FUNCTION(0x3, "sd", "d2"),
		 MPP_FUNCTION(0x4, "uart1", "rts"),
		 MPP_FUNCTION(0x5, "spi0", "cs1"),
		 MPP_FUNCTION(0x6, "led", "c1")),
	MPP_MODE(27,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "pcie1", "clkreq"),
		 MPP_FUNCTION(0x2, "ge1", "rxd3"),
		 MPP_FUNCTION(0x3, "sd", "d1"),
		 MPP_FUNCTION(0x4, "uart1", "cts"),
		 MPP_FUNCTION(0x5, "spi0", "miso"),
		 MPP_FUNCTION(0x6, "led", "c2")),
	MPP_MODE(28,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "led", "p3"),
		 MPP_FUNCTION(0x2, "ge1", "txctl"),
		 MPP_FUNCTION(0x3, "sd", "clk"),
		 MPP_FUNCTION(0x5, "dram", "vttctrl")),
	MPP_MODE(29,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "pcie1", "clkreq"),
		 MPP_FUNCTION(0x2, "ge1", "rxclk"),
		 MPP_FUNCTION(0x3, "sd", "d3"),
		 MPP_FUNCTION(0x5, "spi0", "sck"),
		 MPP_FUNCTION(0x6, "pcie0", "rstout")),
	MPP_MODE(30,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x2, "ge1", "txd0"),
		 MPP_FUNCTION(0x3, "spi1", "cs0"),
		 MPP_FUNCTION(0x5, "led", "p3"),
		 MPP_FUNCTION(0x6, "ptp", "evreq")),
	MPP_MODE(31,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x2, "ge1", "txd1"),
		 MPP_FUNCTION(0x3, "spi1", "mosi"),
		 MPP_FUNCTION(0x5, "led", "p0")),
	MPP_MODE(32,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x2, "ge1", "txd2"),
		 MPP_FUNCTION(0x3, "spi1", "sck"),
		 MPP_FUNCTION(0x4, "ptp", "trig"),
		 MPP_FUNCTION(0x5, "led", "c0")),
	MPP_MODE(33,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x2, "ge1", "txd3"),
		 MPP_FUNCTION(0x3, "spi1", "miso"),
		 MPP_FUNCTION(0x5, "led", "p2")),
	MPP_MODE(34,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x2, "ge1", "txclkout"),
		 MPP_FUNCTION(0x3, "spi1", "sck"),
		 MPP_FUNCTION(0x5, "led", "c1")),
	MPP_MODE(35,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x2, "ge1", "rxctl"),
		 MPP_FUNCTION(0x3, "spi1", "cs1"),
		 MPP_FUNCTION(0x4, "spi0", "cs2"),
		 MPP_FUNCTION(0x5, "led", "p1")),
	MPP_MODE(36,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "pcie0", "clkreq"),
		 MPP_FUNCTION(0x5, "led", "c2")),
	MPP_MODE(37,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "pcie0", "clkreq"),
		 MPP_FUNCTION(0x2, "tdm", "int"),
		 MPP_FUNCTION(0x4, "ge", "mdc")),
	MPP_MODE(38,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "pcie1", "clkreq"),
		 MPP_FUNCTION(0x4, "ge", "mdio")),
	MPP_MODE(39,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x4, "ref", "clkout"),
		 MPP_FUNCTION(0x5, "led", "p3")),
	MPP_MODE(40,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x4, "uart1", "txd"),
		 MPP_FUNCTION(0x5, "led", "p0")),
	MPP_MODE(41,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x4, "uart1", "rxd"),
		 MPP_FUNCTION(0x5, "led", "p1")),
	MPP_MODE(42,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x3, "spi1", "cs2"),
		 MPP_FUNCTION(0x4, "led", "c0"),
		 MPP_FUNCTION(0x6, "ptp", "clk")),
	MPP_MODE(43,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x2, "sata0", "prsnt"),
		 MPP_FUNCTION(0x4, "dram", "vttctrl"),
		 MPP_FUNCTION(0x5, "led", "c1")),
	MPP_MODE(44,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x4, "sata0", "prsnt")),
	MPP_MODE(45,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x2, "spi0", "cs2"),
		 MPP_FUNCTION(0x4, "pcie0", "rstout"),
		 MPP_FUNCTION(0x5, "led", "c2"),
		 MPP_FUNCTION(0x6, "spi1", "cs2")),
	MPP_MODE(46,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "led", "p0"),
		 MPP_FUNCTION(0x2, "ge0", "txd0"),
		 MPP_FUNCTION(0x3, "ge1", "txd0"),
		 MPP_FUNCTION(0x6, "dev", "we1")),
	MPP_MODE(47,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "led", "p1"),
		 MPP_FUNCTION(0x2, "ge0", "txd1"),
		 MPP_FUNCTION(0x3, "ge1", "txd1"),
		 MPP_FUNCTION(0x5, "ptp", "trig"),
		 MPP_FUNCTION(0x6, "dev", "ale0")),
	MPP_MODE(48,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "led", "p2"),
		 MPP_FUNCTION(0x2, "ge0", "txd2"),
		 MPP_FUNCTION(0x3, "ge1", "txd2"),
		 MPP_FUNCTION(0x6, "dev", "ale1")),
	MPP_MODE(49,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "led", "p3"),
		 MPP_FUNCTION(0x2, "ge0", "txd3"),
		 MPP_FUNCTION(0x3, "ge1", "txd3"),
		 MPP_FUNCTION(0x6, "dev", "a2")),
	MPP_MODE(50,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "led", "c0"),
		 MPP_FUNCTION(0x2, "ge0", "rxd0"),
		 MPP_FUNCTION(0x3, "ge1", "rxd0"),
		 MPP_FUNCTION(0x5, "ptp", "evreq"),
		 MPP_FUNCTION(0x6, "dev", "ad12")),
	MPP_MODE(51,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "led", "c1"),
		 MPP_FUNCTION(0x2, "ge0", "rxd1"),
		 MPP_FUNCTION(0x3, "ge1", "rxd1"),
		 MPP_FUNCTION(0x6, "dev", "ad8")),
	MPP_MODE(52,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "led", "c2"),
		 MPP_FUNCTION(0x2, "ge0", "rxd2"),
		 MPP_FUNCTION(0x3, "ge1", "rxd2"),
		 MPP_FUNCTION(0x5, "i2c0", "sda"),
		 MPP_FUNCTION(0x6, "dev", "ad9")),
	MPP_MODE(53,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "pcie1", "rstout"),
		 MPP_FUNCTION(0x2, "ge0", "rxd3"),
		 MPP_FUNCTION(0x3, "ge1", "rxd3"),
		 MPP_FUNCTION(0x5, "i2c0", "sck"),
		 MPP_FUNCTION(0x6, "dev", "ad10")),
	MPP_MODE(54,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "pcie0", "rstout"),
		 MPP_FUNCTION(0x2, "ge0", "rxctl"),
		 MPP_FUNCTION(0x3, "ge1", "rxctl"),
		 MPP_FUNCTION(0x6, "dev", "ad11")),
	MPP_MODE(55,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x2, "ge0", "rxclk"),
		 MPP_FUNCTION(0x3, "ge1", "rxclk"),
		 MPP_FUNCTION(0x6, "dev", "cs0")),
	MPP_MODE(56,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x2, "ge0", "txclkout"),
		 MPP_FUNCTION(0x3, "ge1", "txclkout"),
		 MPP_FUNCTION(0x6, "dev", "oe")),
	MPP_MODE(57,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x2, "ge0", "txctl"),
		 MPP_FUNCTION(0x3, "ge1", "txctl"),
		 MPP_FUNCTION(0x6, "dev", "we0")),
	MPP_MODE(58,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x4, "led", "c0")),
	MPP_MODE(59,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x4, "led", "c1")),
	MPP_MODE(60,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x2, "uart1", "txd"),
		 MPP_FUNCTION(0x4, "led", "c2"),
		 MPP_FUNCTION(0x6, "dev", "ad13")),
	MPP_MODE(61,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "i2c1", "sda"),
		 MPP_FUNCTION(0x2, "uart1", "rxd"),
		 MPP_FUNCTION(0x3, "spi1", "cs2"),
		 MPP_FUNCTION(0x4, "led", "p0"),
		 MPP_FUNCTION(0x6, "dev", "ad14")),
	MPP_MODE(62,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "i2c1", "sck"),
		 MPP_FUNCTION(0x4, "led", "p1"),
		 MPP_FUNCTION(0x6, "dev", "ad15")),
	MPP_MODE(63,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x2, "ptp", "trig"),
		 MPP_FUNCTION(0x4, "led", "p2"),
		 MPP_FUNCTION(0x6, "dev", "burst/last")),
	MPP_MODE(64,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x2, "dram", "vttctrl"),
		 MPP_FUNCTION(0x4, "led", "p3")),
	MPP_MODE(65,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x1, "sata1", "prsnt")),
	MPP_MODE(66,
		 MPP_FUNCTION(0x0, "gpio", NULL),
		 MPP_FUNCTION(0x2, "ptp", "evreq"),
		 MPP_FUNCTION(0x4, "spi1", "cs3"),
		 MPP_FUNCTION(0x5, "pcie0", "rstout"),
		 MPP_FUNCTION(0x6, "dev", "cs3")),
};

static struct mvebu_pinctrl_soc_info armada_375_pinctrl_info;

static const struct of_device_id armada_375_pinctrl_of_match[] = {
	{ .compatible = "marvell,mv88f6720-pinctrl" },
	{ },
};

static struct mvebu_mpp_ctrl mv88f6720_mpp_controls[] = {
	MPP_FUNC_CTRL(0, 69, NULL, armada_375_mpp_ctrl),
};

static struct pinctrl_gpio_range mv88f6720_mpp_gpio_ranges[] = {
	MPP_GPIO_RANGE(0,   0,  0, 32),
	MPP_GPIO_RANGE(1,  32, 32, 32),
	MPP_GPIO_RANGE(2,  64, 64,  3),
};

static int armada_375_pinctrl_probe(struct platform_device *pdev)
{
	struct mvebu_pinctrl_soc_info *soc = &armada_375_pinctrl_info;
	struct resource *res;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	mpp_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(mpp_base))
		return PTR_ERR(mpp_base);

	soc->variant = 0; /* no variants for Armada 375 */
	soc->controls = mv88f6720_mpp_controls;
	soc->ncontrols = ARRAY_SIZE(mv88f6720_mpp_controls);
	soc->modes = mv88f6720_mpp_modes;
	soc->nmodes = ARRAY_SIZE(mv88f6720_mpp_modes);
	soc->gpioranges = mv88f6720_mpp_gpio_ranges;
	soc->ngpioranges = ARRAY_SIZE(mv88f6720_mpp_gpio_ranges);

	pdev->dev.platform_data = soc;

	return mvebu_pinctrl_probe(pdev);
}

static int armada_375_pinctrl_remove(struct platform_device *pdev)
{
	return mvebu_pinctrl_remove(pdev);
}

static struct platform_driver armada_375_pinctrl_driver = {
	.driver = {
		.name = "armada-375-pinctrl",
		.of_match_table = of_match_ptr(armada_375_pinctrl_of_match),
	},
	.probe = armada_375_pinctrl_probe,
	.remove = armada_375_pinctrl_remove,
};

module_platform_driver(armada_375_pinctrl_driver);

MODULE_AUTHOR("Thomas Petazzoni <thomas.petazzoni@free-electrons.com>");
MODULE_DESCRIPTION("Marvell Armada 375 pinctrl driver");
MODULE_LICENSE("GPL v2");
