/*
 * Copyright (C) 2009 Jean-Christophe PLAGNIOL-VILLARD <plagnio@jcrosoft.com>
 *
 * Copyright (C) 2007 Sascha Hauer, Pengutronix
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *
 */

#include <common.h>
#include <net.h>
#include <init.h>
#include <environment.h>
#include <asm/armlinux.h>
#include <generated/mach-types.h>
#include <partition.h>
#include <fs.h>
#include <fcntl.h>
#include <io.h>
#include <asm/hardware.h>
#include <nand.h>
#include <linux/mtd/nand.h>
#include <mach/at91_pmc.h>
#include <mach/board.h>
#include <mach/gpio.h>
#include <mach/io.h>
#include <mach/at91sam9_smc.h>
#include <mach/sam9_smc.h>

static struct atmel_nand_data nand_pdata = {
	.ale		= 21,
	.cle		= 22,
/*	.det_pin	= ... not connected */
	.rdy_pin	= AT91_PIN_PD3,
	.enable_pin	= AT91_PIN_PC14,
	.bus_width_16	= 0,
	.on_flash_bbt	= 1,
};

static struct sam9_smc_config pm_nand_smc_config = {
	.ncs_read_setup		= 0,
	.nrd_setup		= 1,
	.ncs_write_setup	= 0,
	.nwe_setup		= 1,

	.ncs_read_pulse		= 2,
	.nrd_pulse		= 3,
	.ncs_write_pulse	= 3,
	.nwe_pulse		= 4,

	.read_cycle		= 4,
	.write_cycle		= 7,

	.mode			= AT91_SMC_READMODE | AT91_SMC_WRITEMODE | AT91_SMC_EXNWMODE_DISABLE,
	.tdf_cycles		= 3,
};

static void pm_add_device_nand(void)
{
	pm_nand_smc_config.mode |= AT91_SMC_DBW_8;

	/* configure chip-select 3 (NAND) */
	sam9_smc_configure(3, &pm_nand_smc_config);

	at91_add_device_nand(&nand_pdata);
}

#if defined(CONFIG_MCI_ATMEL)
static struct atmel_mci_platform_data __initdata mci_data = {
	.bus_width	= 4,
	.wp_pin		= 0,
	.detect_pin	= AT91_PIN_PD6,
};

static void pm9g45_add_device_mci(void)
{
	at91_add_device_mci(0, &mci_data);
}
#else
static void pm9g45_add_device_mci(void) {}
#endif

/*
 * USB OHCI Host port
 */
#ifdef CONFIG_USB_OHCI_AT91
static struct at91_usbh_data  __initdata usbh_data = {
	.ports		= 2,
	.vbus_pin	= { AT91_PIN_PD0,  0x0 },
};

static void __init pm9g45_add_device_usbh(void)
{
	at91_add_device_usbh_ohci(&usbh_data);
}
#else
static void __init pm9g45_add_device_usbh(void) {}
#endif

static struct at91_ether_platform_data macb_pdata = {
	.flags = AT91SAM_ETHER_RMII,
	.phy_addr = 0,
};

static void pm9g45_phy_init(void)
{
	/*
	 * PD2 enables the 50MHz oscillator for Ethernet PHY
	 * 1 - enable
	 * 0 - disable
	 */
	at91_set_gpio_output(AT91_PIN_PD2, 1);
	at91_set_gpio_value(AT91_PIN_PD2, 1);
}

static int pm9g45_mem_init(void)
{
	at91_add_device_sdram(128 * 1024 * 1024);

	return 0;
}
mem_initcall(pm9g45_mem_init);

static int pm9g45_devices_init(void)
{
	pm_add_device_nand();
	pm9g45_add_device_mci();
	pm9g45_phy_init();
	at91_add_device_eth(0, &macb_pdata);
	pm9g45_add_device_usbh();

	devfs_add_partition("nand0", 0x00000, SZ_128K, DEVFS_PARTITION_FIXED, "at91bootstrap_raw");
	dev_add_bb_dev("at91bootstrap_raw", "at91bootstrap");
	devfs_add_partition("nand0", SZ_128K, SZ_256K, DEVFS_PARTITION_FIXED, "self_raw");
	dev_add_bb_dev("self_raw", "self0");
	devfs_add_partition("nand0", SZ_256K + SZ_128K, SZ_128K, DEVFS_PARTITION_FIXED, "env_raw");
	dev_add_bb_dev("env_raw", "env0");

	armlinux_set_bootparams((void *)(AT91_CHIPSELECT_6 + 0x100));
	armlinux_set_architecture(MACH_TYPE_PM9G45);

	return 0;
}
device_initcall(pm9g45_devices_init);

static int pm9g45_console_init(void)
{
	at91_register_uart(0, 0);
	return 0;
}
console_initcall(pm9g45_console_init);
