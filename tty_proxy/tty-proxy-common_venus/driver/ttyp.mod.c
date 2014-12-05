#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xd4733cff, "module_layout" },
	{ 0xf849195d, "sock_setsockopt" },
	{ 0x72866281, "uart_write_wakeup" },
	{ 0x89b0540e, "uart_add_one_port" },
	{ 0xb2ba53eb, "dev_set_drvdata" },
	{ 0x88a193, "__alloc_workqueue_key" },
	{ 0x62c1ade7, "malloc_sizes" },
	{ 0xe00a2210, "sock_release" },
	{ 0x6ae294b, "sock_recvmsg" },
	{ 0x40c64c6b, "platform_bus_type" },
	{ 0x33543801, "queue_work" },
	{ 0xfb0e29f, "init_timer_key" },
	{ 0x4d3af2b6, "uart_unregister_driver" },
	{ 0x25bfa383, "uart_update_timeout" },
	{ 0x91715312, "sprintf" },
	{ 0x7d11c268, "jiffies" },
	{ 0x572b98aa, "uart_remove_one_port" },
	{ 0x72aa82c6, "param_ops_charp" },
	{ 0xd5f2172f, "del_timer_sync" },
	{ 0x510f6ed7, "dev_err" },
	{ 0xf97456ea, "_raw_spin_unlock_irqrestore" },
	{ 0x50eedeb8, "printk" },
	{ 0x42224298, "sscanf" },
	{ 0x9ab18205, "platform_device_del" },
	{ 0x7694d251, "sock_sendmsg" },
	{ 0x710c7fc6, "platform_device_alloc" },
	{ 0x50377768, "platform_device_add" },
	{ 0xcf100ede, "tty_insert_flip_string_fixed_flag" },
	{ 0x8c03d20c, "destroy_workqueue" },
	{ 0x8834396c, "mod_timer" },
	{ 0xd4b7531, "platform_device_unregister" },
	{ 0xee9eb20d, "platform_driver_register" },
	{ 0x42160169, "flush_workqueue" },
	{ 0x7e5d5a9e, "_dev_info" },
	{ 0x6fe340df, "kmem_cache_alloc_trace" },
	{ 0x21fb443e, "_raw_spin_lock_irqsave" },
	{ 0x37a0cba, "kfree" },
	{ 0x2e60bace, "memcpy" },
	{ 0xe2a92466, "sock_create" },
	{ 0xfc42f058, "uart_register_driver" },
	{ 0x12dad672, "tty_flip_buffer_push" },
	{ 0x5446cde6, "platform_driver_unregister" },
	{ 0x5a2b085e, "uart_get_baud_rate" },
	{ 0x74f13e03, "dev_get_drvdata" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

