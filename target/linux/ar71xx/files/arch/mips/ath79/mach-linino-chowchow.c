/*
 *  TP-LINK TL-WR1041 v2 board support
 *
 *  Copyright (C) 2010-2012 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2011-2012 Anan Huang <axishero@foxmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/pci.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/ath9k_platform.h>
#include <linux/ar8216_platform.h>

#include "machtypes.h"
#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/mach-linino.h>

#include "common.h"
#include "dev-ap9x-pci.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-spi.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"
#include "gpio.h"
#include "linux/gpio.h"
#include <linux/spi/spi_gpio.h>

#define CHOWCHOW_GPIO_MCU_RESET	0
#define CHOWCHOW_GPIO_LED0		12
#define CHOWCHOW_GPIO_LED1		11

#define CHOWCHOW_GPIO_UART0_RX	9
#define CHOWCHOW_GPIO_UART0_TX	10
#define CHOWCHOW_GPIO_UART1_RX	13
#define CHOWCHOW_GPIO_UART1_TX	14
#define CHOWCHOW_GPIO_OE2		15
#define CHOWCHOW_GPIO_CONF_BTN	17
#define CHOWCHOW_GPIO_UART_POL	GPIOF_OUT_INIT_LOW

#define	CHOWCHOW_GPIO_SPI_SCK	4
#define	CHOWCHOW_GPIO_SPI_MISO	3
#define	CHOWCHOW_GPIO_SPI_MOSI	2
#define CHOWCHOW_GPIO_SPI_CS0	1

#define CHOWCHOW_GPIO_SPI_INTERRUPT	16

static struct gpio_led chowchow_leds_gpio[] __initdata = {
	{
		.name		= "usb",
		.gpio		= CHOWCHOW_GPIO_LED0,
		.active_low	= 0,
	}, {
		.name		= "wlan",
		.gpio		= CHOWCHOW_GPIO_LED1,
		.active_low	= 0,
	}
};

static struct gpio_keys_button chowchow_gpio_keys[] __initdata = {
	{
		.desc		= "configuration button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = DS_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= CHOWCHOW_GPIO_CONF_BTN,
		.active_low	= 1,
	}
};

static struct ar8327_pad_cfg db120_ar8327_pad0_cfg = {
	.mode = AR8327_PAD_MAC_RGMII,
	.txclk_delay_en = true,
	.rxclk_delay_en = true,
	.txclk_delay_sel = AR8327_CLK_DELAY_SEL1,
	.rxclk_delay_sel = AR8327_CLK_DELAY_SEL2,
};

static struct ar8327_platform_data db120_ar8327_data = {
	.pad0_cfg = &db120_ar8327_pad0_cfg,
	.cpuport_cfg = {
		.force_link = 1,
		.speed = AR8327_PORT_SPEED_1000,
		.duplex = 1,
		.txpause = 1,
		.rxpause = 1,
	}
};

static struct mdio_board_info db120_mdio0_info[] = {
	{
		.bus_id = "ag71xx-mdio.0",
		.phy_addr = 0,
		.platform_data = &db120_ar8327_data,
	},
};

/* * * * * * * * * * * * * * * * * * * SPI * * * * * * * * * * * * * * * * * */

/*
 * The SPI bus between the main processor and the MCU is available only in the
 * following board: YUN, FREEDOG
 */

static struct spi_gpio_platform_data spi_bus1 = {
	.sck = CHOWCHOW_GPIO_SPI_SCK,
	.mosi = CHOWCHOW_GPIO_SPI_MOSI,
	.miso = CHOWCHOW_GPIO_SPI_MISO,
	.num_chipselect = LININO_N_SPI_CHIP_SELECT,
};

static struct platform_device linino_spi1_device = {
	.name	= "spi_gpio",
	.id	= 1,
	.dev.platform_data = &spi_bus1,
};

/* SPI devices on Linino */
static struct spi_board_info linino_spi_info[] = {
	/*{
		.bus_num		= 1,
		.chip_select		= 0,
		.max_speed_hz		= 10000000,
		.mode			= 0,
		.modalias		= "spidev",
		.controller_data	= (void *) SPI_CHIP_SELECT,
	},*/
	{
		.bus_num		= 1,
		.chip_select		= 0,
		.max_speed_hz		= 10000000, /* unused but required */
		.mode			= 0,
		.modalias		= "atmega32u4",
		.controller_data	= (void *) CHOWCHOW_GPIO_SPI_CS0,
		.platform_data		= (void *) CHOWCHOW_GPIO_SPI_INTERRUPT,
	},
};

/**
 * Enable the software SPI controller emulated by GPIO signals
 */
static void ds_register_spi(void) {
	pr_info("mach-linino: enabling GPIO SPI Controller");

	/* Enable level shifter on SPI signals */
	gpio_set_value(CHOWCHOW_GPIO_OE2, 1);
	/* Register SPI devices */
	spi_register_board_info(linino_spi_info, ARRAY_SIZE(linino_spi_info));
	/* Register GPIO SPI controller */
	platform_device_register(&linino_spi1_device);
}

/*
 * Enable level shifters
 */
static void __init ds_setup_level_shifter_oe(void)
{
	int err;

	/* enable OE2 of level shifter */
    pr_info("Setting GPIO OE %d\n", CHOWCHOW_GPIO_OE2);
    err= gpio_request_one(CHOWCHOW_GPIO_OE2,
			GPIOF_OUT_INIT_LOW | GPIOF_EXPORT_DIR_FIXED, "OE");
	if (err)
		pr_err("mach-linino: error setting GPIO OE\n");
}


static void __init chowchow_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);
	static u8 mac[6];
	
	/* make lan / wan leds software controllable */
	ath79_gpio_output_select(CHOWCHOW_GPIO_LED0, AR934X_GPIO_OUT_GPIO);
	ath79_gpio_output_select(CHOWCHOW_GPIO_LED1, AR934X_GPIO_OUT_GPIO);

	/* enable reset button */
	ath79_gpio_output_select(CHOWCHOW_GPIO_CONF_BTN, AR934X_GPIO_OUT_GPIO);
	ath79_gpio_function_enable(AR934X_GPIO_FUNC_JTAG_DISABLE);
	ath79_gpio_output_select(CHOWCHOW_GPIO_SPI_SCK, AR934X_GPIO_OUT_GPIO);
	ath79_gpio_output_select(CHOWCHOW_GPIO_SPI_MISO, AR934X_GPIO_OUT_GPIO);
	ath79_gpio_output_select(CHOWCHOW_GPIO_SPI_MOSI, AR934X_GPIO_OUT_GPIO);
	ath79_gpio_output_select(CHOWCHOW_GPIO_MCU_RESET, AR934X_GPIO_OUT_GPIO);

	ath79_register_m25p80(NULL);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(chowchow_leds_gpio),
			chowchow_leds_gpio);
	ath79_register_gpio_keys_polled(-1, DS_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(chowchow_gpio_keys),
					 chowchow_gpio_keys);
	pr_info("mach-linino: enabling USB Controller");
	ath79_register_usb();

	ath79_init_mac(mac, art + DS_WMAC_MAC_OFFSET, -1);
	ath79_register_wmac(art + DS_CALDATA_OFFSET, mac);

	ath79_init_mac(mac, art + DS_WMAC_MAC_OFFSET, -2);
	ap91_pci_init(art + DS_PCIE_CALDATA_OFFSET, mac);

	ath79_setup_ar934x_eth_cfg(AR934X_ETH_CFG_RGMII_GMAC0 |
				   AR934X_ETH_CFG_SW_ONLY_MODE);

	ath79_register_mdio(1, 0x0);
	ath79_register_mdio(0, 0x0);

	ath79_init_mac(ath79_eth0_data.mac_addr, art + DS_WMAC_MAC_OFFSET, -2);

	mdiobus_register_board_info(db120_mdio0_info,
				    ARRAY_SIZE(db120_mdio0_info));

	printk(KERN_DEBUG "chow: Register Eth0\n");

	/* GMAC0 is connected to an AR8327 switch */
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ath79_eth0_data.phy_mask = BIT(0);
	ath79_eth0_data.mii_bus_dev = &ath79_mdio0_device.dev;
	ath79_eth0_pll_data.pll_1000 = 0x06000000;
	ath79_register_eth(0);
	
	/* enable OE of level shifters */
	ds_setup_level_shifter_oe();
	
	/* Register Software SPI controller */
	ds_register_spi();
}

MIPS_MACHINE(ATH79_MACH_LININO_CHOWCHOW, "linino-chowchow", "Linino ChowChow", chowchow_setup);
