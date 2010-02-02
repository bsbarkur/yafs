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
	{ 0x217e8984, "struct_module" },
	{ 0xce9756f1, "kmem_cache_destroy" },
	{ 0x21037cc4, "per_cpu__current_task" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0x405c1144, "get_seconds" },
	{ 0x8142a388, "__mark_inode_dirty" },
	{ 0xd0d8621b, "strlen" },
	{ 0x67535f44, "kill_anon_super" },
	{ 0x13988844, "malloc_sizes" },
	{ 0x64759a2d, "generic_read_dir" },
	{ 0x2e74a89c, "__insert_inode_hash" },
	{ 0x73092a90, "get_sb_single" },
	{ 0xb72397d5, "printk" },
	{ 0xb95193e2, "d_rehash" },
	{ 0x9f98dfca, "d_alloc_root" },
	{ 0x2f287f0d, "copy_to_user" },
	{ 0x11467de4, "kmem_cache_free" },
	{ 0xf9b61bc2, "inode_init_once" },
	{ 0x4bb373ee, "kmem_cache_alloc" },
	{ 0x9098f8be, "kmem_cache_create" },
	{ 0x9302ad51, "register_filesystem" },
	{ 0xf90c612f, "iput" },
	{ 0x37a0cba, "kfree" },
	{ 0x488ec76c, "simple_statfs" },
	{ 0x28ea6a00, "unregister_filesystem" },
	{ 0x8988bd34, "init_special_inode" },
	{ 0x32e632f7, "new_inode" },
	{ 0xd6c963c, "copy_from_user" },
	{ 0x9b4c954f, "d_instantiate" },
	{ 0x83090731, "iget_locked" },
	{ 0x51360a42, "generic_fillattr" },
	{ 0xe914e41e, "strcpy" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "9B1B96067D72A277E970C9D");
