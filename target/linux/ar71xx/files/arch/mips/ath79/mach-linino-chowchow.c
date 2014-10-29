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

#include <asm/mach-ath79/ar71xx_regs.h>
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

#define CHOWCHOW_GPIO_BTN_RESET	17
#define CHOWCHOW_GPIO_LED_WLAN	12

#define CHOWCHOW_GPIO_LED_USB	11

#define CHOWCHOW_KEYS_POLL_INTERVAL		20	/* msecs */
#define CHOWCHOW_KEYS_DEBOUNCE_INTERVAL	(3 * CHOWCHOW_KEYS_POLL_INTERVAL)

#define CHOWCHOW_PCIE_CALDATA_OFFSET	0x5000

#if 0
static const char *tl_wr1041nv2_part_probes[] = {
	"tp-link",
	NULL,
};

static struct flash_platform_data tl_wr1041nv2_flash_data = {
	.part_probes	= tl_wr1041nv2_part_probes,
};
#endif

static struct gpio_led chowchow_leds_gpio[] __initdata = {
	{
		.name		= "usb",
		.gpio		= CHOWCHOW_GPIO_LED_USB,
		.active_low	= 1,
	}, {
		.name		= "wlan",
		.gpio		= CHOWCHOW_GPIO_LED_WLAN,
		.active_low	= 1,
	}
};

static struct gpio_keys_button chowchow_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = CHOWCHOW_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= CHOWCHOW_GPIO_BTN_RESET,
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

static void __init chowchow_setup(void)
{
	//u8 *mac = (u8 *) KSEG1ADDR(0x1f01fc00);
	//u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);
	static u8 mac[6];

	ath79_register_m25p80(NULL);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(chowchow_leds_gpio),
			chowchow_leds_gpio);
	ath79_register_gpio_keys_polled(-1, CHOWCHOW_KEYS_POLL_INTERVAL,
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
}

MIPS_MACHINE(ATH79_MACH_LININO_CHOWCHOW, "linino-chowchow", "Linino ChowChow", chowchow_setup);
