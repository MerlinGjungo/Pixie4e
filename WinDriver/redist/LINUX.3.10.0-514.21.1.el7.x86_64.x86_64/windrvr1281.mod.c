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
	{ 0x28950ef1, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x6bc3fbc0, __VMLINUX_SYMBOL_STR(__unregister_chrdev) },
	{ 0x2d3385d3, __VMLINUX_SYMBOL_STR(system_wq) },
	{ 0xc2f9c045, __VMLINUX_SYMBOL_STR(timespec_to_jiffies) },
	{ 0x98ab5c8d, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x7f59e59, __VMLINUX_SYMBOL_STR(pci_write_config_dword) },
	{ 0xd2b09ce5, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0xc897c382, __VMLINUX_SYMBOL_STR(sg_init_table) },
	{ 0xf5893abf, __VMLINUX_SYMBOL_STR(up_read) },
	{ 0x4c4fef19, __VMLINUX_SYMBOL_STR(kernel_stack) },
	{ 0x8bd590db, __VMLINUX_SYMBOL_STR(pci_write_config_word) },
	{ 0xd6ee688f, __VMLINUX_SYMBOL_STR(vmalloc) },
	{ 0x784213a6, __VMLINUX_SYMBOL_STR(pv_lock_ops) },
	{ 0xc996d097, __VMLINUX_SYMBOL_STR(del_timer) },
	{ 0x754d539c, __VMLINUX_SYMBOL_STR(strlen) },
	{ 0xc2f7c1b1, __VMLINUX_SYMBOL_STR(pci_read_config_byte) },
	{ 0x59d5a7f7, __VMLINUX_SYMBOL_STR(dma_set_mask) },
	{ 0x45449b56, __VMLINUX_SYMBOL_STR(boot_cpu_data) },
	{ 0x1c3e657e, __VMLINUX_SYMBOL_STR(pci_disable_device) },
	{ 0x58ecf574, __VMLINUX_SYMBOL_STR(pci_disable_msix) },
	{ 0x7fac3dae, __VMLINUX_SYMBOL_STR(windrvr1281_OS_stream_transfer_start) },
	{ 0x2c8118a8, __VMLINUX_SYMBOL_STR(windrvr1281_OS_uninit) },
	{ 0x9f6b4c62, __VMLINUX_SYMBOL_STR(down_interruptible) },
	{ 0x77528365, __VMLINUX_SYMBOL_STR(windrvr1281_OS_wakeup) },
	{ 0xc816cb3, __VMLINUX_SYMBOL_STR(__register_chrdev) },
	{ 0x5b8adbca, __VMLINUX_SYMBOL_STR(x86_dma_fallback_dev) },
	{ 0xc9426d6d, __VMLINUX_SYMBOL_STR(pci_write_config_byte) },
	{ 0x3710ec32, __VMLINUX_SYMBOL_STR(windrvr1281_OS_get_device_property) },
	{ 0xcf73ce21, __VMLINUX_SYMBOL_STR(pci_release_regions) },
	{ 0x593a99b, __VMLINUX_SYMBOL_STR(init_timer_key) },
	{ 0x999e8297, __VMLINUX_SYMBOL_STR(vfree) },
	{ 0x7d11c268, __VMLINUX_SYMBOL_STR(jiffies) },
	{ 0x183627f8, __VMLINUX_SYMBOL_STR(windrvr1281_OS_set_stream_context) },
	{ 0xe2d5255a, __VMLINUX_SYMBOL_STR(strcmp) },
	{ 0x57a6ccd0, __VMLINUX_SYMBOL_STR(down_read) },
	{ 0xb115ad9c, __VMLINUX_SYMBOL_STR(windrvr1281_OS_close_device) },
	{ 0x71de9b3f, __VMLINUX_SYMBOL_STR(_copy_to_user) },
	{ 0xbe4a1520, __VMLINUX_SYMBOL_STR(pci_set_master) },
	{ 0xd683ee53, __VMLINUX_SYMBOL_STR(pci_get_bus_and_slot) },
	{ 0xfb578fc5, __VMLINUX_SYMBOL_STR(memset) },
	{ 0xcfafacd8, __VMLINUX_SYMBOL_STR(windrvr1281_OS_is_stream_requests_queue_empty) },
	{ 0xac1adf42, __VMLINUX_SYMBOL_STR(pci_enable_msix) },
	{ 0xb92fff82, __VMLINUX_SYMBOL_STR(windrvr1281_OS_lock_user_buff) },
	{ 0x8f64aa4, __VMLINUX_SYMBOL_STR(_raw_spin_unlock_irqrestore) },
	{ 0xb8c7ff88, __VMLINUX_SYMBOL_STR(current_task) },
	{ 0x2e2b40d2, __VMLINUX_SYMBOL_STR(strncat) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xbe1d568c, __VMLINUX_SYMBOL_STR(windrvr1281_OS_transfer) },
	{ 0x449ad0a7, __VMLINUX_SYMBOL_STR(memcmp) },
	{ 0x4c9d28b0, __VMLINUX_SYMBOL_STR(phys_base) },
	{ 0x9166fada, __VMLINUX_SYMBOL_STR(strncpy) },
	{ 0x5a921311, __VMLINUX_SYMBOL_STR(strncmp) },
	{ 0xc2560ac2, __VMLINUX_SYMBOL_STR(pci_read_config_word) },
	{ 0xbf8ba54a, __VMLINUX_SYMBOL_STR(vprintk) },
	{ 0x16305289, __VMLINUX_SYMBOL_STR(warn_slowpath_null) },
	{ 0x19132411, __VMLINUX_SYMBOL_STR(windrvr1281_OS_get_stream_context) },
	{ 0x68aca4ad, __VMLINUX_SYMBOL_STR(down) },
	{ 0x6d849ca0, __VMLINUX_SYMBOL_STR(get_zone_device_page) },
	{ 0xb13cd896, __VMLINUX_SYMBOL_STR(windrvr1281_OS_open_pipe) },
	{ 0xbe2c0274, __VMLINUX_SYMBOL_STR(add_timer) },
	{ 0xd6b8e852, __VMLINUX_SYMBOL_STR(request_threaded_irq) },
	{ 0x8e323e18, __VMLINUX_SYMBOL_STR(windrvr1281_OS_selective_suspend) },
	{ 0x8a72d913, __VMLINUX_SYMBOL_STR(windrvr1281_OS_num_pending_urbs) },
	{ 0xadd2aceb, __VMLINUX_SYMBOL_STR(__get_page_tail) },
	{ 0x61651be, __VMLINUX_SYMBOL_STR(strcat) },
	{ 0x99b0aabc, __VMLINUX_SYMBOL_STR(pci_find_capability) },
	{ 0xb77399db, __VMLINUX_SYMBOL_STR(windrvr1281_OS_stream_issue_new_transfers) },
	{ 0x9768e9bb, __VMLINUX_SYMBOL_STR(windrvr1281_OS_set_interface) },
	{ 0x78764f4e, __VMLINUX_SYMBOL_STR(pv_irq_ops) },
	{ 0x42c8de35, __VMLINUX_SYMBOL_STR(ioremap_nocache) },
	{ 0xa916b694, __VMLINUX_SYMBOL_STR(strnlen) },
	{ 0xf0fdf6cb, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0x32f730e3, __VMLINUX_SYMBOL_STR(get_user_pages) },
	{ 0x310de205, __VMLINUX_SYMBOL_STR(windrvr1281_wdusb_register_callbacks) },
	{ 0xa324f098, __VMLINUX_SYMBOL_STR(windrvr1281_OS_get_max_urb_transfer_size) },
	{ 0x61bb9c3f, __VMLINUX_SYMBOL_STR(windrvr1281_OS_stream_transfer_create) },
	{ 0xd62c833f, __VMLINUX_SYMBOL_STR(schedule_timeout) },
	{ 0x1000e51, __VMLINUX_SYMBOL_STR(schedule) },
	{ 0x8a7d1c31, __VMLINUX_SYMBOL_STR(high_memory) },
	{ 0x43261dca, __VMLINUX_SYMBOL_STR(_raw_spin_lock_irq) },
	{ 0xebfdcb96, __VMLINUX_SYMBOL_STR(pci_read_config_dword) },
	{ 0x672c4569, __VMLINUX_SYMBOL_STR(windrvr1281_OS_unlock_user_buff) },
	{ 0xb73f01ae, __VMLINUX_SYMBOL_STR(windrvr1281_OS_reset_pipe) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0x2cb61da5, __VMLINUX_SYMBOL_STR(pci_unregister_driver) },
	{ 0x41ec4c1a, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x9327f5ce, __VMLINUX_SYMBOL_STR(_raw_spin_lock_irqsave) },
	{ 0x99195078, __VMLINUX_SYMBOL_STR(vsnprintf) },
	{ 0xada88a2e, __VMLINUX_SYMBOL_STR(windrvr1281_OS_halt_transfer) },
	{ 0x4f68e5c9, __VMLINUX_SYMBOL_STR(do_gettimeofday) },
	{ 0x4a350370, __VMLINUX_SYMBOL_STR(vm_mmap) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0xe84cb310, __VMLINUX_SYMBOL_STR(remap_pfn_range) },
	{ 0x69acdf38, __VMLINUX_SYMBOL_STR(memcpy) },
	{ 0x801678, __VMLINUX_SYMBOL_STR(flush_scheduled_work) },
	{ 0xc3fc2f, __VMLINUX_SYMBOL_STR(pci_request_regions) },
	{ 0x79142775, __VMLINUX_SYMBOL_STR(pci_disable_msi) },
	{ 0x7a7f7d68, __VMLINUX_SYMBOL_STR(dma_supported) },
	{ 0xedc03953, __VMLINUX_SYMBOL_STR(iounmap) },
	{ 0xd3719d59, __VMLINUX_SYMBOL_STR(paravirt_ticketlocks_enabled) },
	{ 0x71e3cecb, __VMLINUX_SYMBOL_STR(up) },
	{ 0x99487493, __VMLINUX_SYMBOL_STR(__pci_register_driver) },
	{ 0x334c1f75, __VMLINUX_SYMBOL_STR(put_page) },
	{ 0x58af4a0f, __VMLINUX_SYMBOL_STR(pci_get_device) },
	{ 0x5b56860c, __VMLINUX_SYMBOL_STR(vm_munmap) },
	{ 0x983e9a0a, __VMLINUX_SYMBOL_STR(windrvr1281_OS_register_devices) },
	{ 0xdaf7b334, __VMLINUX_SYMBOL_STR(pci_dev_put) },
	{ 0x2e0d2f7f, __VMLINUX_SYMBOL_STR(queue_work_on) },
	{ 0x117cb312, __VMLINUX_SYMBOL_STR(pci_enable_msi_block) },
	{ 0x18e6b5cd, __VMLINUX_SYMBOL_STR(vmalloc_to_page) },
	{ 0xc6225679, __VMLINUX_SYMBOL_STR(windrvr1281_OS_get_device_info) },
	{ 0xbfc55e1b, __VMLINUX_SYMBOL_STR(windrvr1281_OS_init) },
	{ 0x4601cbd4, __VMLINUX_SYMBOL_STR(windrvr1281_OS_unregister_devices) },
	{ 0x46734db7, __VMLINUX_SYMBOL_STR(pci_enable_device) },
	{ 0x77e2f33, __VMLINUX_SYMBOL_STR(_copy_from_user) },
	{ 0x1add957f, __VMLINUX_SYMBOL_STR(windrvr1281_OS_reset_device) },
	{ 0x2482e688, __VMLINUX_SYMBOL_STR(vsprintf) },
	{ 0x9e7d6bd0, __VMLINUX_SYMBOL_STR(__udelay) },
	{ 0x584c5b17, __VMLINUX_SYMBOL_STR(dma_ops) },
	{ 0xfd1d10c8, __VMLINUX_SYMBOL_STR(windrvr1281_OS_stream_request_insert) },
	{ 0xf20dabd8, __VMLINUX_SYMBOL_STR(free_irq) },
	{ 0xe914e41e, __VMLINUX_SYMBOL_STR(strcpy) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=windrvr1281_usb";

MODULE_ALIAS("pci:v*d*sv*sd*bc*sc*i*");

MODULE_INFO(srcversion, "604960E37C5F4EB3EEA95E6");
MODULE_INFO(rhelversion, "7.3");
