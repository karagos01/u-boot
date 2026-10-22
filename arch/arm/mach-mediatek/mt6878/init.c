// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Igor Belwon <igor.belwon@mentallysanemainliners.org>
 */

#include <fdtdec.h>
#include <init.h>
#include <asm/armv8/mmu.h>
#include <asm/system.h>
#include <asm/global_data.h>
#include <linux/sizes.h>
#include <linux/string.h>

DECLARE_GLOBAL_DATA_PTR;

int print_cpuinfo(void)
{
	return 0;
//	printk("CPU:   MediaTek MT6878\n");
//	return 0;
}

void reset_cpu(void)
{
	psci_system_reset();
}

static struct mm_region tetris_mem_map[] = {
	{
		/* Peripheral region */
		.virt = 0x00000000UL,
		.phys = 0x00000000UL,
		.size = 0x40000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
		PTE_BLOCK_NON_SHARE |
		PTE_BLOCK_PXN | PTE_BLOCK_UXN,
	},
	{
		/* DRAM region 0 */
		.virt = 0x040000000UL,
		.phys = 0x040000000UL,
		.size = 0x026f00000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) | PTE_BLOCK_OUTER_SHARE
	},
	{
		/* TEE Reserved 0 */
		.virt = 0x66f00000UL,
		.phys = 0x66f00000UL,
		.size = 0x09100000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
		PTE_BLOCK_NON_SHARE |
		PTE_BLOCK_PXN | PTE_BLOCK_UXN,
	},
	{
		/* DRAM region 1 */
		.virt = 0x70000000UL,
		.phys = 0x70000000UL,
		.size = 0x08000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) | PTE_BLOCK_OUTER_SHARE
	},
	{
		/* TEE Reserved 1 */
		.virt = 0x78000000UL,
		.phys = 0x78000000UL,
		.size = 0x07fff000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
		PTE_BLOCK_NON_SHARE |
		PTE_BLOCK_PXN | PTE_BLOCK_UXN,
	},
	{
		/* DRAM region 2 */
		.virt = 0x7ffff000UL,
		.phys = 0x7ffff000UL,
		.size = 0x7e0d1000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) | PTE_BLOCK_OUTER_SHARE
	},
	{
		/* Framebuffer Reserved */
		.virt = 0xfe0d0000UL,
		.phys = 0xfe0d0000UL,
		.size = 0x00c00000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL_NC) |
				 PTE_BLOCK_INNER_SHARE |
				 PTE_BLOCK_PXN | PTE_BLOCK_UXN
	},
	{
		/* DRAM region 3 */
		.virt = 0xfecd0000UL,
		.phys = 0xfecd0000UL,
		.size = 0x4132f000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) | PTE_BLOCK_OUTER_SHARE
	},
	{
		/* DRAM Controller 0 Reserved */
		.virt = 0x13ffff000UL,
		.phys = 0x13ffff000UL,
		.size = 0x000001000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
		PTE_BLOCK_NON_SHARE |
		PTE_BLOCK_PXN | PTE_BLOCK_UXN,
	},
	{
		/* DRAM region 4 */
		.virt = 0x140000000UL,
		.phys = 0x140000000UL,
		.size = 0x0c0000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) | PTE_BLOCK_OUTER_SHARE
	},
	{
		0,
	}
};

/* Secure carveouts taken from the stock LK reserved-memory */
static struct mm_region plinius_mem_map[] = {
	{
		/* Peripheral region */
		.virt = 0x00000000UL,
		.phys = 0x00000000UL,
		.size = 0x40000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
		PTE_BLOCK_NON_SHARE |
		PTE_BLOCK_PXN | PTE_BLOCK_UXN,
	},
	{
		/* DRAM region 0 */
		.virt = 0x40000000UL,
		.phys = 0x40000000UL,
		.size = 0x08800000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) | PTE_BLOCK_OUTER_SHARE
	},
	{
		/* BL31 Reserved */
		.virt = 0x48800000UL,
		.phys = 0x48800000UL,
		.size = 0x00200000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
		PTE_BLOCK_NON_SHARE |
		PTE_BLOCK_PXN | PTE_BLOCK_UXN,
	},
	{
		/* DRAM region 1 */
		.virt = 0x48a00000UL,
		.phys = 0x48a00000UL,
		.size = 0x27600000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) | PTE_BLOCK_OUTER_SHARE
	},
	{
		/* TEE Reserved */
		.virt = 0x70000000UL,
		.phys = 0x70000000UL,
		.size = 0x06c00000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
		PTE_BLOCK_NON_SHARE |
		PTE_BLOCK_PXN | PTE_BLOCK_UXN,
	},
	{
		/* DRAM region 2 */
		.virt = 0x76c00000UL,
		.phys = 0x76c00000UL,
		.size = 0x07c00000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) | PTE_BLOCK_OUTER_SHARE
	},
	{
		/* GPU, TEE secmem, SSPM, MCUPM, CCU Reserved */
		.virt = 0x7e800000UL,
		.phys = 0x7e800000UL,
		.size = 0x01900000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
		PTE_BLOCK_NON_SHARE |
		PTE_BLOCK_PXN | PTE_BLOCK_UXN,
	},
	{
		/* Framebuffer Reserved */
		.virt = 0xfe20f000UL,
		.phys = 0xfe20f000UL,
		.size = 0x01df0000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL_NC) |
				 PTE_BLOCK_INNER_SHARE |
				 PTE_BLOCK_PXN | PTE_BLOCK_UXN
	},
	{
		0,
	}
};

_Static_assert(sizeof(plinius_mem_map) <= sizeof(tetris_mem_map),
	       "plinius_mem_map must fit into tetris_mem_map");

struct mm_region *mem_map = tetris_mem_map;

int dram_init(void)
{
	int ret;

	ret = fdtdec_setup_mem_size_base();
	if (ret)
		return ret;

	/*
	 * Plinius has secure carveouts right above 2GB, so probing them
	 * would fault. Trust the memory node instead.
	 *
	 * Copy the table rather than repointing mem_map: the pointer gets
	 * its link-time value back when U-Boot relocates itself.
	 */
	if (!fdt_node_check_compatible(gd->fdt_blob, 0, "volla,plinius")) {
		memcpy(tetris_mem_map, plinius_mem_map,
		       sizeof(plinius_mem_map));
		return 0;
	}

	// Fix ram_size to 2GB, above that things start to break
	gd->ram_size = get_ram_size((void *)gd->ram_base, SZ_2G);

	return 0;
}
