/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definicion de funciones del manejador de memoria
*/

#include "mmu.h"
#include "i386.h"

#include "kassert.h"

uint32_t next_free_kernel_page;
uint32_t next_free_user_page;


void mmu_init(void) {
	next_free_kernel_page = KERNEL_FREE_SPACE;
	next_free_user_page = USER_FREE_SPACE;
}

paddr_t mmu_next_free_kernel_page(void) {
	uint32_t page = next_free_kernel_page;
	next_free_kernel_page += PAGE_SIZE;
	return page;
}

paddr_t mmu_next_free_user_page(void) {
	uint32_t page = next_free_user_page;
	next_free_user_page += PAGE_SIZE;
	return page;
}

paddr_t mmu_init_kernel_dir(void) {
	// Page directory and Page Tables
	page_directory_entry* pd = (page_directory_entry*) KERNEL_PAGE_DIR;
	page_table_entry* pt_0 = (page_table_entry*) KERNEL_PAGE_TABLE_0;
	
	for (int i = 0; i < 1024; ++i){
		pd[i] = (page_directory_entry){0};
		pt_0[i] = (page_table_entry){0};
	}

	pd[0].user_supervisor = MMU_USER_SUPERVISOR;
	pd[0].read_write = MMU_READ_WRITE;
	pd[0].present = MMU_PRESENT;
	pd[0].base_table_address = ((uint32_t) pt_0) >> 12;
	
	for (int i = 0; i < 1024; ++i){
		pt_0[i].user_supervisor = MMU_USER_SUPERVISOR;
		pt_0[i].read_write = MMU_READ_WRITE;
		pt_0[i].present = MMU_PRESENT;
		pt_0[i].base_page_address = i;
	}
	
	return (paddr_t) pd;
}

// uint32_t attrs
void mmu_map_page(uint32_t cr3, vaddr_t virt, paddr_t phy, uint8_t mmu_us, uint8_t mmu_rw) {

	uint32_t directory_idx = virt >> 22;
	uint32_t table_idx = (virt >> 12) & 0x3FF;
	page_directory_entry* pd = (page_directory_entry*) cr3;
	
	if (!pd[directory_idx].present){
		page_table_entry* npt = (page_table_entry*) mmu_next_free_kernel_page();
		for (int i = 0; i < 1024; ++i){
			npt[i] = (page_table_entry){0};
		}
		pd[directory_idx].user_supervisor = mmu_us; 
		pd[directory_idx].present = MMU_PRESENT;
		pd[directory_idx].base_table_address = ((uint32_t) npt) >> 12;
	}

	pd[directory_idx].read_write = mmu_rw || pd[directory_idx].read_write;

	page_table_entry* pt = (page_table_entry*) (pd[directory_idx].base_table_address << 12);
	pt[table_idx].user_supervisor = mmu_us;
	pt[table_idx].read_write = mmu_rw;
	pt[table_idx].present = MMU_PRESENT;
	pt[table_idx].base_page_address = phy >> 12;
	tlbflush();
}


paddr_t mmu_unmap_page(uint32_t cr3, vaddr_t virt) {

	uint32_t directory_idx = virt >> 22;
	uint32_t table_idx = (virt >> 12) & 0x3FF;

	page_directory_entry* pd = (page_directory_entry*) cr3;
	page_table_entry* pt = (page_table_entry*) (pd[directory_idx].base_table_address << 12);
	pt[table_idx].present = !MMU_PRESENT;
	tlbflush();
	
	return pt[table_idx].base_page_address; 
}

paddr_t mmu_init_task_dir(paddr_t phy_start) {

	page_directory_entry* pd = (page_directory_entry*) mmu_next_free_kernel_page();

	for (int i = 0; i < 1024; ++i) pd[i] = (page_directory_entry){0};

	// Identity mapping 4MB
	for (int i = 0; i < 1024; ++i) mmu_map_page((uint32_t)pd, PAGE_SIZE * i, PAGE_SIZE * i, MMU_USER_SUPERVISOR, MMU_READ_WRITE);

	// Mapeo 2 paginas de tareas
	for (int i = 0; i < TASK_CODE_PAGES; ++i){
		mmu_map_page((uint32_t) pd, TASK_CODE_VIRTUAL + PAGE_SIZE * i, phy_start + PAGE_SIZE * i, MMU_USER_UNPRIVILEGED, MMU_READ_ONLY);
	}

	// Mapeo del stack de la tarea
	mmu_map_page((uint32_t) pd, TASK_STACK_BASE - 0x1000, mmu_next_free_user_page(), MMU_USER_UNPRIVILEGED, MMU_READ_WRITE);

	return (paddr_t) pd;
}

