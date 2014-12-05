/*
 * G60/80 serial touchscreen driver
 * base on gtouch.c
 *
 * Copyright (c) 2004 Vojtech Pavlik
 * Copyright (c) 2010 longfeng.xiao@philips.com
 */

/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 */

/*
 * 2005/02/19 Dan Streetman <ddstreet@ieee.org>
 *   Copied elo.c and edited for MicroTouch protocol
 *
 * 2010/03/17 xlongfeng <longfeng.xiao@philips.com>
 *   Copied gtouch.c and edited for G80/60 protocol
 */

#include <linux/version.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/serio.h>
#include <linux/init.h>

#define DRIVER_DESC	"G60/80 Touch serial touchscreen driver"

MODULE_AUTHOR("xlongfeng <longfeng.xiao@philps.com>");
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");

/*
 * Definitions & global arrays.
 */

#define SERIO_GTOUCH 		0x44

#define GTOUCH_FORMAT_TABLET_STATUS_BIT 0x80
#define GTOUCH_FORMAT_TABLET_TOUCH_BIT 0x40
#define GTOUCH_FORMAT_TABLET_LENGTH 5
#define GTOUCH_RESPONSE_BEGIN_BYTE 0x01
#define GTOUCH_RESPONSE_END_BYTE 0x0d

/* todo: check specs for max length of all responses */
#define GTOUCH_MAX_LENGTH 16

#define GTOUCH_MIN_XC 0
#define GTOUCH_MAX_XC 0x0fff
#define GTOUCH_MIN_YC 0
#define GTOUCH_MAX_YC 0x0fff

#define GTOUCH_GET_XC(data) (((data[1])<<7) | data[2])
#define GTOUCH_GET_YC(data) (((data[3])<<7) | data[4])
#define GTOUCH_GET_TOUCHED(data) (GTOUCH_FORMAT_TABLET_TOUCH_BIT & data[0])

/*
 * Per-touchscreen data.
 */

struct gtouch {
	struct input_dev *dev;
	struct serio *serio;
	int idx;
	unsigned char data[GTOUCH_MAX_LENGTH];
	char phys[32];
};

static void gtouch_process_linear(struct gtouch *gtouch, int *x, int *y)
{
#if 0
	static int a[] = {15540, 57, -5704384, -21, 11822, -4381552, 65536};
	int xtemp,ytemp;

	xtemp = GTOUCH_GET_XC(gtouch->data);
	ytemp = GTOUCH_GET_YC(gtouch->data);

	*x = (a[2] + a[0]*xtemp + a[1]*ytemp) / a[6];
	*y = (a[5] + a[3]*xtemp + a[4]*ytemp) / a[6];
#else
	*x = GTOUCH_GET_XC(gtouch->data);
	*y = GTOUCH_GET_YC(gtouch->data);
#endif
}

static void gtouch_process_format_tablet(struct gtouch *gtouch)
{
	struct input_dev *dev = gtouch->dev;
	int x, y;

	if (GTOUCH_FORMAT_TABLET_LENGTH == ++gtouch->idx) {
		gtouch_process_linear(gtouch, &x, &y);
		input_report_abs(dev, ABS_X, x);
		input_report_abs(dev, ABS_Y, y);
		input_report_key(dev, BTN_TOUCH, GTOUCH_GET_TOUCHED(gtouch->data));
		input_report_abs(dev, ABS_PRESSURE, GTOUCH_GET_TOUCHED(gtouch->data));
		input_sync(dev);

		gtouch->idx = 0;
	}
}

static void gtouch_process_response(struct gtouch *gtouch)
{
	gtouch->idx = 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,21)
static irqreturn_t gtouch_interrupt(struct serio *serio,
		unsigned char data, unsigned int flags)
#else
static irqreturn_t gtouch_interrupt(struct serio *serio,
		unsigned char data, unsigned int flags, struct pt_regs *regs)
#endif
{
	struct gtouch* gtouch = serio_get_drvdata(serio);

	gtouch->data[gtouch->idx] = data;

	if (GTOUCH_FORMAT_TABLET_STATUS_BIT & gtouch->data[0])
		gtouch_process_format_tablet(gtouch);
	else
		gtouch_process_response(gtouch);

	return IRQ_HANDLED;
}

/*
 * gtouch_disconnect() is the opposite of gtouch_connect()
 */

static void gtouch_disconnect(struct serio *serio)
{
	struct gtouch* gtouch = serio_get_drvdata(serio);

	input_get_device(gtouch->dev);
	input_unregister_device(gtouch->dev);
	serio_close(serio);
	serio_set_drvdata(serio, NULL);
	input_put_device(gtouch->dev);
	kfree(gtouch);
}

/*
 * gtouch_connect() is the routine that is called when someone adds a
 * new serio device that supports MicroTouch (Format Tablet) protocol and registers it as
 * an input device.
 */

static int gtouch_connect(struct serio *serio, struct serio_driver *drv)
{
	struct gtouch *gtouch;
	struct input_dev *input_dev;
	int err;

	gtouch = kzalloc(sizeof(struct gtouch), GFP_KERNEL);
	input_dev = input_allocate_device();
	if (!gtouch || !input_dev) {
		err = -ENOMEM;
		goto fail1;
	}

	gtouch->serio = serio;
	gtouch->dev = input_dev;
	snprintf(gtouch->phys, sizeof(gtouch->phys), "%s/input0", serio->phys);

	input_dev->name = "G60/80 Touch Serial TouchScreen";
	input_dev->phys = gtouch->phys;
	input_dev->id.bustype = BUS_RS232;
	input_dev->id.vendor = 0x8303;
	input_dev->id.product = 0;
	input_dev->id.version = 0x0100;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
	input_dev->dev.parent = &serio->dev;
	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
	input_dev->keybit[BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH);
#else
	input_dev->cdev.dev = &serio->dev;
	input_dev->private = gtouch;
	input_dev->evbit[0] = BIT(EV_KEY) | BIT(EV_ABS);
	input_dev->keybit[LONG(BTN_TOUCH)] = BIT(BTN_TOUCH);
#endif
	input_set_abs_params(gtouch->dev, ABS_X, GTOUCH_MIN_XC, GTOUCH_MAX_XC, 0, 0);
	input_set_abs_params(gtouch->dev, ABS_Y, GTOUCH_MIN_YC, GTOUCH_MAX_YC, 0, 0);
	input_set_abs_params(gtouch->dev, ABS_PRESSURE, 0, GTOUCH_FORMAT_TABLET_TOUCH_BIT, 0, 0);

	serio_set_drvdata(serio, gtouch);

	err = serio_open(serio, drv);
	if (err)
		goto fail2;

	err = input_register_device(gtouch->dev);
	if (err)
		goto fail3;

	return 0;

 fail3:	serio_close(serio);
 fail2:	serio_set_drvdata(serio, NULL);
 fail1:	input_free_device(input_dev);
	kfree(gtouch);
	return err;
}

/*
 * The serio driver structure.
 */

static struct serio_device_id gtouch_serio_ids[] = {
	{
		.type	= SERIO_RS232,
		.proto	= SERIO_GTOUCH,
		.id	= SERIO_ANY,
		.extra	= SERIO_ANY,
	},
	{ 0 }
};

MODULE_DEVICE_TABLE(serio, gtouch_serio_ids);

static struct serio_driver gtouch_drv = {
	.driver		= {
		.name	= "gtouch",
	},
	.description	= DRIVER_DESC,
	.id_table	= gtouch_serio_ids,
	.interrupt	= gtouch_interrupt,
	.connect	= gtouch_connect,
	.disconnect	= gtouch_disconnect,
};

/*
 * The functions for inserting/removing us as a module.
 */

static int __init gtouch_init(void)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,21)
	return serio_register_driver(&gtouch_drv);
#else
	serio_register_driver(&gtouch_drv);
	return 0;
#endif
}

static void __exit gtouch_exit(void)
{
	serio_unregister_driver(&gtouch_drv);
}

module_init(gtouch_init);
module_exit(gtouch_exit);
