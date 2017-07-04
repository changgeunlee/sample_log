#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
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
	{ 0x683cfe8d, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xb73e26fa, __VMLINUX_SYMBOL_STR(netlink_kernel_release) },
	{ 0xdb7305a1, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0xa1dfc876, __VMLINUX_SYMBOL_STR(__netlink_kernel_create) },
	{ 0xf5a1a046, __VMLINUX_SYMBOL_STR(init_net) },
	{ 0xdc362526, __VMLINUX_SYMBOL_STR(__nlmsg_put) },
	{ 0x22c579d0, __VMLINUX_SYMBOL_STR(netlink_unicast) },
	{ 0xdfca5118, __VMLINUX_SYMBOL_STR(__alloc_skb) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "868F1B1EBB490C01DC9BF9F");
