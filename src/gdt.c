/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definicion de la tabla de descriptores globales
*/

#include "gdt.h"

gdt_entry_t gdt[GDT_COUNT] = {
	/* Descriptor nulo*/
	/* Offset = 0x00 */
	[GDT_IDX_NULL_DESC] = {
		.limit_15_0 = 0x0000,
		.base_15_0 = 0x0000,
		.base_23_16 = 0x00,
		.type = 0x0,
		.s = 0x00,
		.dpl = 0x00,
		.p = 0x00,
		.limit_19_16 = 0x00,
		.avl = 0x0,
		.l = 0x0,
		.db = 0x0,
		.g = 0x00,
		.base_31_24 = 0x00,
	},

	[GDT_IDX_CODE_0] = {
		.base_15_0 = GDT_DESC_BASE_15_0_FLAT,
		.base_23_16 = GDT_DESC_BASE_23_16_FLAT,
		.base_31_24 = GDT_DESC_BASE_31_24_FLAT,
		.limit_15_0 = GDT_DESC_LIMIT_15_0,
		.limit_19_16 = GDT_DESC_LIMIT_19_16,
		.type = 10,
		.s = 1,
		.dpl = 0,
		.p = 1,
		.avl = 0,
		.l = 0,
		.db = 1,
		.g = 1,
	},

	[GDT_IDX_CODE_3] = {
		.base_15_0 = GDT_DESC_BASE_15_0_FLAT,
		.base_23_16 = GDT_DESC_BASE_23_16_FLAT,
		.base_31_24 = GDT_DESC_BASE_31_24_FLAT,
		.limit_15_0 = GDT_DESC_LIMIT_15_0,
		.limit_19_16 = GDT_DESC_LIMIT_19_16,
		.type = 10,
		.s = 1,
		.dpl = 3,
		.p = 1,
		.avl = 0,
		.l = 0,
		.db = 1,
		.g = 1,
	},

	[GDT_IDX_DATA_0] = {
		.base_15_0 = GDT_DESC_BASE_15_0_FLAT,
		.base_23_16 = GDT_DESC_BASE_23_16_FLAT,
		.base_31_24 = GDT_DESC_BASE_31_24_FLAT,
		.limit_15_0 = GDT_DESC_LIMIT_15_0,
		.limit_19_16 = GDT_DESC_LIMIT_19_16,
		.type = 2,
		.s = 1,
		.dpl = 0,
		.p = 1,
		.avl = 0,
		.l = 0,
		.db = 1,
		.g = 1,
	},

	[GDT_IDX_DATA_3] = {
		.base_15_0 = GDT_DESC_BASE_15_0_FLAT,
		.base_23_16 = GDT_DESC_BASE_23_16_FLAT,
		.base_31_24 = GDT_DESC_BASE_31_24_FLAT,
		.limit_15_0 = GDT_DESC_LIMIT_15_0,
		.limit_19_16 = GDT_DESC_LIMIT_19_16,
		.type = 2,
		.s = 1,
		.dpl = 3,
		.p = 1,
		.avl = 0,
		.l = 0,
		.db = 1,
		.g = 1,
	},

	[GDT_IDX_VIDEO_0] = {
		.base_15_0 = GDT_DESC_BASE_15_0_VIDEO,
		.base_23_16 = GDT_DESC_BASE_23_16_VIDEO,
		.base_31_24 = GDT_DESC_BASE_31_24_VIDEO,
		.limit_15_0 = GDT_DESC_LIMIT_15_0_VIDEO,
		.limit_19_16 = GDT_DESC_LIMIT_19_16_VIDEO,
		.type = 2,
		.s = 1,
		.dpl = 0,
		.p = 1,
		.avl = 0,
		.l = 0,
		.db = 1,
		.g = 0,
	},

	[GDT_IDX_INITIAL_TSS] = {
		.base_15_0 = 0,  //filled later
		.base_23_16 = 0, //filled later
		.base_31_24 = 0, //filled later
		.limit_15_0 = GDT_DESC_TSS_LIMIT_15_0,
		.limit_19_16 = GDT_DESC_TSS_LIMIT_19_16,
		.type = GDT_DESC_TSS_TYPE, 
		.s = GDT_DESC_TSS_S,
		.dpl = 0, //kernel
		.p = 1,   //present
		.avl = 0,
		.l = 0,
		.db = 0,
		.g = 0
	},

	[GDT_IDX_IDLE_TSS] = {
		.base_15_0 = 0,  //filled later
		.base_23_16 = 0, //filled later
		.base_31_24 = 0, //filled later
		.limit_15_0 = GDT_DESC_TSS_LIMIT_15_0,
		.limit_19_16 = GDT_DESC_TSS_LIMIT_19_16,
		.type = GDT_DESC_TSS_TYPE, 
		.s = GDT_DESC_TSS_S,
		.dpl = 0, //kernel
		.p = 1,   //present
		.avl = 0,
		.l = 0,
		.db = 0,
		.g = 0
	}
};

gdt_descriptor_t GDT_DESC = { sizeof(gdt) - 1, (uint32_t)&gdt };
