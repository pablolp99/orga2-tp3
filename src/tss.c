/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definicion de estructuras para administrar tareas
*/

#include "tss.h"
#include "defines.h"
#include "kassert.h"
#include "mmu.h"
#include "i386.h"

#define BASE_LOW(tss) \
    (uint32_t) &tss & 0x0000FFFF;

#define BASE_MID(tss) \
	((uint32_t) &tss & 0x00FF0000) >> 16;

#define BASE_HIGH(tss) \
	(uint32_t) &tss >> 24;


tss_t tss_initial = {0};
tss_t tss_idle = {0};
tss_t tss_new = {0};

void init_initial_tss(void) {
	gdt[GDT_IDX_INITIAL_TSS].base_15_0  = BASE_LOW(tss_initial);  
	gdt[GDT_IDX_INITIAL_TSS].base_23_16 = BASE_MID(tss_initial); 
	gdt[GDT_IDX_INITIAL_TSS].base_31_24 = BASE_HIGH(tss_initial); 
}

void init_idle_tss(void) {
	// CR3
	tss_idle.cr3 = rcr3();

	// Task address
	tss_idle.eip = TASK_IDLE_START;

	// EFLAGS
	tss_idle.eflags = EFLAGS_DEFAULT_VALUE;

	tss_idle.esp = KERNEL_STACK;
	tss_idle.ebp = KERNEL_STACK;

	// Selectors
	tss_idle.cs = CODE_SELECTOR_0;
	tss_idle.es = DATA_SELECTOR_0;
	tss_idle.ss = DATA_SELECTOR_0;
	tss_idle.ds = DATA_SELECTOR_0;
	tss_idle.fs = DATA_SELECTOR_0;
	tss_idle.gs = DATA_SELECTOR_0;

	tss_idle.iomap = TSS_IOMAP_ALL_PORTS_DISABLED;

	gdt[GDT_IDX_IDLE_TSS].base_15_0  = BASE_LOW(tss_idle);  
	gdt[GDT_IDX_IDLE_TSS].base_23_16 = BASE_MID(tss_idle); 
	gdt[GDT_IDX_IDLE_TSS].base_31_24 = BASE_HIGH(tss_idle); 
}

tss_t* create_user_task_tss(uint8_t gdt_idx, paddr_t phy_start) {
	// Creamos nuevo task
	tss_t* tss_new = (tss_t*) mmu_next_free_kernel_page();

	// CR3
	tss_new->cr3 = mmu_init_task_dir(phy_start);

	// Task address
	tss_new->eip = TASK_CODE_VIRTUAL;

	// EFLAGS
	tss_new->eflags = EFLAGS_DEFAULT_VALUE;

	// Stacks
	tss_new->esp = TASK_STACK_BASE;
	tss_new->ebp = TASK_STACK_BASE;

	tss_new->ss0 = DATA_SELECTOR_0;
	tss_new->esp0 = mmu_next_free_kernel_page() + PAGE_SIZE;

	// Selectors
	tss_new->cs = CODE_SELECTOR_3;
	tss_new->es = DATA_SELECTOR_3;
	tss_new->ss = DATA_SELECTOR_3;
	tss_new->ds = DATA_SELECTOR_3;
	tss_new->fs = DATA_SELECTOR_3;
	tss_new->gs = DATA_SELECTOR_3;

	tss_new->iomap = TSS_IOMAP_ALL_PORTS_DISABLED;

	gdt[gdt_idx].base_15_0  = BASE_LOW(*tss_new);  
	gdt[gdt_idx].base_23_16 = BASE_MID(*tss_new); 
	gdt[gdt_idx].base_31_24 = BASE_HIGH(*tss_new); 

	return tss_new;
}


void tss_init(void) {
	init_idle_tss();
	init_initial_tss();
}
