/* kttyp.c
 *
 * Copyright (C) 2010 GOLDWAY INC.
 *
 * Serial driver emulate from socket
 *
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#include <linux/version.h>
#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/sysrq.h>
#include <linux/console.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/serial_core.h>
#include <linux/serial.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/irq.h>
#include <linux/timer.h>
#include <linux/socket.h>
#include <linux/tcp.h>
#include <linux/net.h>
#include <linux/in.h>
#include <net/sock.h>
#ifdef CONFIG_SERIO
#include <linux/serio.h>
#endif

#include "ksocket.h"

#ifndef INPUT_SUPPORT
#undef CONFIG_SERIO
#endif

enum PROXY_STAGE {
	PROXY_STOP,
	PROXY_CONNECT,
	PROXY_READY,
};

struct ttyp_port {
	spinlock_t		lock;

	ksocket_t 		socket;
	struct 			sockaddr_in address;
	char 			*serverip;
	unsigned short 		ipport;

	struct work_struct 	timer_work;
	struct timer_list 	timer;
	int 			stage;

	int			rx_claimed;
	int			tx_claimed;

	struct uart_port	port;

#define BUF_SIZE 		1400
	unsigned char 		buf[BUF_SIZE];

#ifdef CONFIG_SERIO
#define SERIO_SKBD 		0x43
#define SERIO_GTOUCH 		0x44
#define SERIO_KB_LINE 		4
#define SERIO_TS_LINE 		5
	struct serio 		*serio;
#endif
};

/* UART name and device definitions */

#define TTY_PROXY_NAME		"ttyProxy"
#define TTY_PROXY_MAJOR		206
#define TTY_PROXY_MINOR		8
#define PROXY_PORT_BASE 	20000

/* macros to change one thing to another */
#define tx_enabled(port) ((port)->unused[0])
#define rx_enabled(port) ((port)->unused[1])

static char *serverip = "127.0.0.1";
module_param(serverip, charp, 0);

static struct workqueue_struct *proxy_work;
static int workqueue_flush_flag;

static void tty_proxy_stop_tx(struct uart_port *port);
static void tty_proxy_start_tx(struct uart_port *port);
static void tty_proxy_stop_rx(struct uart_port *port);

static inline struct ttyp_port *to_ourport(struct uart_port *port)
{
	return container_of(port, struct ttyp_port, port);
}

static void tty_proxy_stop_tx(struct uart_port *port)
{
}

static void tty_proxy_start_tx(struct uart_port *port)
{
#if 0
	unsigned long flags;

	proxyn_lock_irqsave(&port->lock, flags);
	proxyn_unlock_irqrestore(&port->lock, flags);
#endif
}

static void tty_proxy_stop_rx(struct uart_port *port)
{
}

static void tty_proxy_enable_ms(struct uart_port *port)
{
}

static unsigned int tty_proxy_tx_empty(struct uart_port *port)
{
	return 1;
}

/* no modem control lines */
static unsigned int tty_proxy_get_mctrl(struct uart_port *port)
{
	return TIOCM_CAR | TIOCM_DSR;
}

static void tty_proxy_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
}

static void tty_proxy_break_ctl(struct uart_port *port, int break_state)
{
}

static int socket_transmit(struct ttyp_port *proxy)
{
	struct uart_port *port = &proxy->port;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
	struct tty_struct *tty = port->state->port.tty;
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
	struct tty_struct *tty = port->info->port.tty;
#else
	struct tty_struct *tty = port->info->tty;
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
	struct circ_buf *xmit = &port->state->xmit;
#else
	struct circ_buf *xmit = &port->info->xmit;
#endif
	int c, l;
	int ret;

	c = uart_circ_chars_pending(xmit);
	c = c < BUF_SIZE ? c : BUF_SIZE;
	if (c) {
		l = min(c, (int)(UART_XMIT_SIZE - xmit->tail));
		memcpy(proxy->buf, &xmit->buf[xmit->tail], l);
		memcpy(proxy->buf + l, xmit->buf, c - l);
		port->icount.tx += c;
		xmit->tail = (xmit->tail + c) & (UART_XMIT_SIZE-1);

		if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS)
			uart_write_wakeup(port);

		if (uart_circ_empty(xmit))
			tty_proxy_stop_tx(port);

		ret = ksend(proxy->socket, proxy->buf, c, MSG_DONTWAIT);
		if (ret < 0) {
			return -ECONNRESET;
		}
	}

	ret = krecv(proxy->socket, proxy->buf, BUF_SIZE, MSG_DONTWAIT);
	if (ret > 0) {
#ifdef CONFIG_SERIO
		if (proxy->serio) {
			for (l = 0; l < ret; l++) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,21)
				serio_interrupt(proxy->serio, proxy->buf[l], 0);
#else
				serio_interrupt(proxy->serio, proxy->buf[l], 0, NULL);
#endif
			}
		}
#endif
		port->icount.rx += ret;
		tty_insert_flip_string(tty, proxy->buf, ret);
		tty_flip_buffer_push(tty);
	} else {
		if (ret != -EAGAIN)
			return -ECONNRESET;
	}

	return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,21)
static void socket_work(struct work_struct *work)
{
	struct ttyp_port *proxy = container_of(work, struct ttyp_port, timer_work);
#else
static void socket_work(void *data)
{
	struct ttyp_port *proxy = data;
#endif
	struct uart_port *port = &proxy->port;
	struct device *dev = proxy->port.dev;
	int val = 1;
	int ret;

	if (!tx_enabled(port) || !rx_enabled(port))
		return;

	switch (proxy->stage) {
	case PROXY_CONNECT:
		if (proxy->socket) {
			kclose(proxy->socket);
			proxy->socket = NULL;
		}

		proxy->socket = ksocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (!proxy->socket) {
			dev_err(dev, "create socket %s:%d failed\n", \
					proxy->serverip, proxy->ipport);
			proxy->stage = PROXY_STOP;
			return;
		}
		proxy->address.sin_family = AF_INET;
		proxy->address.sin_addr.s_addr = inet_addr(proxy->serverip);
		proxy->address.sin_port = htons(proxy->ipport);

		ksetsockopt(proxy->socket, SOL_TCP, TCP_NODELAY, 
				(char __user *)&val, sizeof(val));

		ret = kconnect(proxy->socket, (struct sockaddr *)&proxy->address, sizeof(proxy->address));
		if (ret < 0) {
			if (proxy->socket) {
				kclose(proxy->socket);
				proxy->socket = NULL;
			}
			/* dev_info(dev, "socket reconnect to %s:%d %d\n", \ */
					/* proxy->serverip, proxy->ipport, ret); */
			mod_timer(&proxy->timer, jiffies + HZ);
			return;
		} else {
			dev_info(dev, "socket connect to %s:%d succeed %d\n", \
					proxy->serverip, proxy->ipport, ret);
		}
		proxy->stage = PROXY_READY;
		mod_timer(&proxy->timer, jiffies + HZ/100);
		break;

	case PROXY_READY:
		ret = socket_transmit(proxy);
		if (ret != 0) {
			dev_info(dev, "socket %s:%d transmit failed %d\n", \
					proxy->serverip, proxy->ipport, ret);
			proxy->stage = PROXY_CONNECT;
		}
		mod_timer(&proxy->timer, jiffies + 5);
		break;

	default:
		if (proxy->socket) {
			kclose(proxy->socket);
			proxy->socket = NULL;
		}
	}
}

void tty_proxy(unsigned long data)
{
	struct ttyp_port *proxy = (struct ttyp_port *)data;

	/* struct device *dev = proxy->port.dev; */
	/* dev_info(dev, "%s:%d stage %d\n", __func__, __LINE__, proxy->stage); */

	if (proxy_work == NULL)
		return;

	switch (proxy->stage) {
	case PROXY_CONNECT:
	case PROXY_READY:
		queue_work(proxy_work, &proxy->timer_work);
		break;

	default:
		break;
	}
}

static int tty_proxy_startup(struct uart_port *port)
{
	struct ttyp_port *proxy = to_ourport(port);

	tx_enabled(port) = 1;
	rx_enabled(port) = 1;

	proxy->tx_claimed = 1;
	proxy->rx_claimed = 1;

	proxy->stage = PROXY_CONNECT;
	INIT_LIST_HEAD(&proxy->timer_work.entry);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,21)
	PREPARE_WORK(&proxy->timer_work, socket_work);
#else
	PREPARE_WORK(&proxy->timer_work, socket_work, (void *)proxy);
#endif
	setup_timer(&proxy->timer,
		    tty_proxy, (unsigned long) proxy);
	mod_timer(&proxy->timer, jiffies + HZ / 10);

	return 0;
}

static void tty_proxy_shutdown(struct uart_port *port)
{
	struct ttyp_port *proxy = to_ourport(port);

	proxy->stage = PROXY_STOP;
	del_timer_sync(&proxy->timer);
	flush_workqueue(proxy_work);
	if (proxy->socket) {
		kclose(proxy->socket);
		proxy->socket = NULL;
	}

	tx_enabled(port) = 0;
	proxy->tx_claimed = 0;

	proxy->rx_claimed = 0;
	rx_enabled(port) = 0;
}

static void tty_proxy_pm(struct uart_port *port, unsigned int level,
			      unsigned int old)
{
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,21)
static void tty_proxy_set_termios(struct uart_port *port,
				       struct ktermios *termios,
				       struct ktermios *old)
#else
static void tty_proxy_set_termios(struct uart_port *port,
				       struct termios *termios,
				       struct termios *old)
#endif
{
	unsigned int baud;
	unsigned long flags;

	/*
	 * We don't support modem control lines.
	 */
	termios->c_cflag &= ~(HUPCL | CMSPAR);
	termios->c_cflag |= CLOCAL;

	baud = uart_get_baud_rate(port, termios, old, 0, 115200*8);

	spin_lock_irqsave(&port->lock, flags);

	/*
	 * Update the per-port timeout.
	 */
	uart_update_timeout(port, termios->c_cflag, baud);

	/*
	 * Which character status flags should we ignore?
	 */
	port->ignore_status_mask = 0;

	spin_unlock_irqrestore(&port->lock, flags);
}

static const char *tty_proxy_type(struct uart_port *port)
{
	return "tty-proxy";
}

static void tty_proxy_release_port(struct uart_port *port)
{
}

static int tty_proxy_request_port(struct uart_port *port)
{
	return 0;
}

static void tty_proxy_config_port(struct uart_port *port, int flags)
{
	if (flags & UART_CONFIG_TYPE &&
	    tty_proxy_request_port(port) == 0)
		port->type = PORT_S3C2440;
}

/*
 * verify the new serial_struct (for TIOCSSERIAL).
 */
static int
tty_proxy_verify_port(struct uart_port *port, struct serial_struct *ser)
{
	if (ser->type != PORT_UNKNOWN && ser->type != PORT_S3C2440)
		return -EINVAL;

	return 0;
}

static struct uart_ops tty_proxy_ops = {
	.pm		= tty_proxy_pm,
	.tx_empty	= tty_proxy_tx_empty,
	.get_mctrl	= tty_proxy_get_mctrl,
	.set_mctrl	= tty_proxy_set_mctrl,
	.stop_tx	= tty_proxy_stop_tx,
	.start_tx	= tty_proxy_start_tx,
	.stop_rx	= tty_proxy_stop_rx,
	.enable_ms	= tty_proxy_enable_ms,
	.break_ctl	= tty_proxy_break_ctl,
	.startup	= tty_proxy_startup,
	.shutdown	= tty_proxy_shutdown,
	.set_termios	= tty_proxy_set_termios,
	.type		= tty_proxy_type,
	.release_port	= tty_proxy_release_port,
	.request_port	= tty_proxy_request_port,
	.config_port	= tty_proxy_config_port,
	.verify_port	= tty_proxy_verify_port,
};

static struct uart_driver ttyp_uart_driver = {
	.owner		= THIS_MODULE,
	.dev_name	= "tty_proxy",
	.nr		= TTY_PROXY_MINOR,
	.driver_name	= TTY_PROXY_NAME,
	.major		= TTY_PROXY_MAJOR,
	.minor		= 0,
};

static struct platform_device *ttyp_uart_device[TTY_PROXY_MINOR];

static int tty_proxy_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct ttyp_port *proxy;
	int ret;

	proxy = kzalloc(sizeof(struct ttyp_port), GFP_KERNEL);
	if (proxy == NULL) {
		dev_err(dev, "Cannot allocated tty proxy port\n");
		return -ENOMEM;
	}

	proxy->serverip = serverip;
	proxy->ipport = PROXY_PORT_BASE + pdev->id;
	proxy->port.dev = &pdev->dev;
	proxy->port.mapbase = (unsigned long)proxy;
	proxy->port.membase = (void *)proxy;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0)
	proxy->port.lock = __SPIN_LOCK_UNLOCKED(proxy->port.lock);
#else
	proxy->port.lock = SPIN_LOCK_UNLOCKED;
#endif
	proxy->port.iotype = UPIO_MEM;
	proxy->port.irq = 44;
	proxy->port.uartclk = 1;
	proxy->port.fifosize = 16;
	proxy->port.ops = &tty_proxy_ops;
	proxy->port.flags = UPF_BOOT_AUTOCONF;
	proxy->port.line = pdev->id;

	ret = uart_add_one_port(&ttyp_uart_driver, &proxy->port);
	if (ret) {
		dev_err(dev, "Add uart port failed %d\n", ret);
		kfree(proxy);
		return ret;
	}

#ifdef CONFIG_SERIO
	if (proxy->port.line == SERIO_KB_LINE) {
		struct serio *serio;

		serio = proxy->serio = kzalloc(sizeof(struct serio), GFP_KERNEL);
		if (serio) {
			strlcpy(serio->name, "skbd", sizeof(serio->name));
			snprintf(serio->phys, sizeof(serio->phys), "%s%d/serio0", "ttyp", proxy->port.line);
			serio->id.type = SERIO_RS232;
			serio->id.proto = SERIO_SKBD,
			serio->id.extra = SERIO_ANY;
			serio->id.id = SERIO_ANY;
			serio->port_data = proxy;
			serio->dev.parent = proxy->port.dev;

			serio_register_port(serio);
		}
	}

	if (proxy->port.line == SERIO_TS_LINE) {
		struct serio *serio;

		serio = proxy->serio = kzalloc(sizeof(struct serio), GFP_KERNEL);
		if (serio) {
			strlcpy(serio->name, "gtouch", sizeof(serio->name));
			snprintf(serio->phys, sizeof(serio->phys), "%s%d/serio0", "ttyp", proxy->port.line);
			serio->id.type = SERIO_RS232;
			serio->id.proto = SERIO_GTOUCH;
			serio->id.extra = SERIO_ANY;
			serio->id.id = SERIO_ANY;
			serio->port_data = proxy;
			serio->dev.parent = proxy->port.dev;

			serio_register_port(serio);
		}
	}
#endif

	dev_set_drvdata(dev, proxy);

	return 0;
}

static int tty_proxy_remove(struct platform_device *pdev)
{
	struct ttyp_port *proxy = dev_get_drvdata(&pdev->dev);
	struct uart_port *port = &proxy->port;

#ifdef CONFIG_SERIO
	if (proxy->serio) {
		serio_unregister_port(proxy->serio);
		/* kfree(proxy->serio); */
		proxy->serio = NULL;
	}
#endif

	if (port)
		uart_remove_one_port(&ttyp_uart_driver, port);

	dev_set_drvdata(&pdev->dev, NULL);

	return 0;
}

#ifdef CONFIG_PM

static int tty_proxy_suspend(struct platform_device *pdev, pm_message_t state)
{
	return 0;
}

static int tty_proxy_resume(struct platform_device *pdev)
{
	return 0;
}

#else
#define tty_proxy_suspend NULL
#define tty_proxy_resume  NULL
#endif

static struct platform_driver ttyp_drv = {
	.probe		= tty_proxy_probe,
	.remove		= tty_proxy_remove,
	.suspend	= tty_proxy_suspend,
	.resume		= tty_proxy_resume,
	.driver		= {
		.name	= "kttyp",
		.owner	= THIS_MODULE,
		.bus    = &platform_bus_type,
	},
};

static int tty_proxy_put_devs(void)
{
	int i;

	for (i = 0; i < TTY_PROXY_MINOR; i++) {
		if (ttyp_uart_device[i])
			platform_device_del(ttyp_uart_device[i]);
	}

	return 0;
}

static int __init tty_proxy_init(void)
{
	int ret;
	int i;

	proxy_work = create_singlethread_workqueue("kttyp");
	if (!proxy_work) {
		printk(KERN_ERR "failed to singlethread workqueue for tty proxy\n");
		return -ENOMEM;
	}
	flush_workqueue(proxy_work);
	workqueue_flush_flag = 0;

	ret = uart_register_driver(&ttyp_uart_driver);
	if (ret < 0) {
		printk(KERN_ERR "failed to register socket serial driver\n");
		destroy_workqueue(proxy_work);
		return -1;
	}

	for (i = 0; i < TTY_PROXY_MINOR; i++) {
		ttyp_uart_device[i] = platform_device_alloc("kttyp", i);
		if (!ttyp_uart_device[i]) {

			return -ENOMEM;
		}
		ret = platform_device_add(ttyp_uart_device[i]);
		if (ret) {
			tty_proxy_put_devs();
			uart_unregister_driver(&ttyp_uart_driver);
			destroy_workqueue(proxy_work);
			return ret;
		}
	}

	ret = platform_driver_register(&ttyp_drv);

	if (ret) {
		tty_proxy_put_devs();
		uart_unregister_driver(&ttyp_uart_driver);
		destroy_workqueue(proxy_work);
		return ret;
	}

	return 0;
}

static void __exit tty_proxy_exit(void)
{
	int i;

	workqueue_flush_flag = 1;
	destroy_workqueue(proxy_work);
	proxy_work = NULL;

	platform_driver_unregister(&ttyp_drv);

	for (i = 0; i < TTY_PROXY_MINOR; i++) {
		platform_device_unregister(ttyp_uart_device[i]);
	}

	uart_unregister_driver(&ttyp_uart_driver);
}

module_init(tty_proxy_init);
module_exit(tty_proxy_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("xlongfeng<xlongfeng@126.com>");
MODULE_DESCRIPTION("Serial driver emulate from socket");
