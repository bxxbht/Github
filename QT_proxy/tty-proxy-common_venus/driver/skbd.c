/*
 * socket keyboard driver for Linux
 * Copyright (C) 2010 GOLDWAY INC.
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Should you need to contact me, the author, you can do so either by
 * e-mail - mail your message to <vojtech@ucw.cz>, or by paper mail:
 * Vojtech Pavlik, Simunkova 1594, Prague 8, 182 00 Czech Republic
 */

#include <linux/version.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/init.h>
#include <linux/serio.h>

#define DRIVER_DESC	"Socket keyboard driver"

#define SERIO_SKBD 	0x43

#ifdef CONFIG_SERIO

static unsigned char skbd_keycode[256] = {
	[','] = KEY_RIGHT,
	['.'] = KEY_LEFT,
	/* ['0'] = KEY_0, */
	/* ['1'] = KEY_1, */
	/* ['2'] = KEY_2, */
	/* ['3'] = KEY_3, */
	/* ['4'] = KEY_4, */
	/* ['5'] = KEY_5, */
	/* ['6'] = KEY_6, */
	['a'] = KEY_A,
	['b'] = KEY_F2,
	['c'] = KEY_F3,
	['d'] = KEY_F4,
	['e'] = KEY_F5,
	['f'] = KEY_F6,
	['g'] = KEY_ENTER,
	['h'] = KEY_H,
	['i'] = KEY_I,
};

struct skbd {
	unsigned char keycode[256];
	struct input_dev *dev;
	struct serio *serio;
	char phys[32];
};


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,21)
static irqreturn_t skbd_interrupt(struct serio *serio,
	unsigned char data, unsigned int flags)
#else
static irqreturn_t skbd_interrupt(struct serio *serio,
	unsigned char data, unsigned int flags, struct pt_regs *regs)
#endif
{
	struct skbd *skbd = serio_get_drvdata(serio);

	if (skbd->keycode[data & 0xff]) {
		input_report_key(skbd->dev, skbd->keycode[data & 0xff], 1);
		input_report_key(skbd->dev, skbd->keycode[data & 0xff], 0);
		input_sync(skbd->dev);
	} else {
		/* printk(KERN_WARNING "skbd.c: Unknown key (scancode %#x).\n", data); */
	}
	return IRQ_HANDLED;
}

static int skbd_connect(struct serio *serio, struct serio_driver *drv)
{
	struct skbd *skbd;
	struct input_dev *input_dev;
	int err = -ENOMEM;
	int i;

	skbd = kmalloc(sizeof(struct skbd), GFP_KERNEL);
	input_dev = input_allocate_device();
	if (!skbd || !input_dev)
		goto fail1;

	skbd->serio = serio;
	skbd->dev = input_dev;
	snprintf(skbd->phys, sizeof(skbd->phys), "%s/input0", serio->phys);
	memcpy(skbd->keycode, skbd_keycode, sizeof(skbd->keycode));

	input_dev->name = "Socket Keyboard";
	input_dev->phys = skbd->phys;
	input_dev->id.bustype = BUS_RS232;
	input_dev->id.vendor  = 0x0001;
	input_dev->id.product = 0x0001;
	input_dev->id.version = 0x0100;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
	input_dev->dev.parent = &serio->dev;
#else
	input_dev->cdev.dev = &serio->dev;
	input_dev->private = skbd;
#endif

	input_dev->evbit[0] = BIT(EV_KEY) | BIT(EV_REP);
	input_dev->keycode = skbd->keycode;
	input_dev->keycodesize = sizeof(unsigned char);
	input_dev->keycodemax = ARRAY_SIZE(skbd_keycode);

	for (i = 0; i < 255; i++)
		set_bit(skbd->keycode[i], input_dev->keybit);
	clear_bit(0, input_dev->keybit);

	serio_set_drvdata(serio, skbd);

	err = serio_open(serio, drv);
	if (err)
		goto fail2;

	err = input_register_device(skbd->dev);
	if (err)
		goto fail3;

	return 0;

 fail3:	serio_close(serio);
 fail2:	serio_set_drvdata(serio, NULL);
 fail1:	input_free_device(input_dev);
	kfree(skbd);
	return err;
}

static void skbd_disconnect(struct serio *serio)
{
	struct skbd *skbd = serio_get_drvdata(serio);

	serio_close(serio);
	serio_set_drvdata(serio, NULL);
	input_unregister_device(skbd->dev);
	kfree(skbd);
}

static struct serio_device_id skbd_serio_ids[] = {
	{
		.type	= SERIO_RS232,
		.proto	= SERIO_SKBD,
		.id	= SERIO_ANY,
		.extra	= SERIO_ANY,
	},
	{ 0 }
};

MODULE_DEVICE_TABLE(serio, skbd_serio_ids);

static struct serio_driver skbd_drv = {
	.driver		= {
		.name	= "skbd",
	},
	.description	= DRIVER_DESC,
	.id_table	= skbd_serio_ids,
	.interrupt	= skbd_interrupt,
	.connect	= skbd_connect,
	.disconnect	= skbd_disconnect,
};

static int __init skbd_init(void)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,21)
	return serio_register_driver(&skbd_drv);
#else
	serio_register_driver(&skbd_drv);
	return 0;
#endif
}

static void __exit skbd_exit(void)
{
	serio_unregister_driver(&skbd_drv);
}

module_init(skbd_init);
module_exit(skbd_exit);

MODULE_AUTHOR("xiaolongfeng <xlongfeng@126.com>");
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");

#endif
