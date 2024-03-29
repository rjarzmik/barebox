/*
 * bus.c - barebox driver model
 *
 * Copyright (c) 2009 Sascha Hauer <s.hauer@pengutronix.de>, Pengutronix
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <common.h>
#include <driver.h>
#include <errno.h>
#include <init.h>

static int platform_match(struct device_d *dev, struct driver_d *drv)
{
	if (IS_ENABLED(CONFIG_OFDEVICE) && dev->device_node &&
			drv->of_compatible)
		return of_match(dev, drv);

	if (!strcmp(dev->name, drv->name))
		return 0;

	if (drv->id_table) {
		struct platform_device_id *id = drv->id_table;

		while (id->name) {
			if (!strcmp(id->name, dev->name)) {
				dev->id_entry = id;
				return 0;
			}
			id++;
		}
	}

	return -1;
}

static int platform_probe(struct device_d *dev)
{
	return dev->driver->probe(dev);
}

static void platform_remove(struct device_d *dev)
{
	dev->driver->remove(dev);
}

int platform_driver_register(struct driver_d *drv)
{
	drv->bus = &platform_bus;

	return register_driver(drv);
}

int platform_device_register(struct device_d *new_device)
{
	new_device->bus = &platform_bus;

	return register_device(new_device);
}

struct bus_type platform_bus = {
	.name = "platform",
	.match = platform_match,
	.probe = platform_probe,
	.remove = platform_remove,
};

static int plarform_init(void)
{
	return bus_register(&platform_bus);
}
pure_initcall(plarform_init);
