/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Declaracion de funciones del manejador de memoria
*/

#ifndef __MMU_H__
#define __MMU_H__

#include "types.h"

void mmu_init(void);

paddr_t mmu_next_free_kernel_page(void);

paddr_t mmu_next_free_user_page(void);

//void mmu_map_page(uint32_t cr3, vaddr_t virt, paddr_t phy, uint32_t attrs);
void mmu_map_page(uint32_t cr3, vaddr_t virt, paddr_t phy, uint8_t mmu_us, uint8_t mmu_rw);

paddr_t mmu_unmap_page(uint32_t cr3, vaddr_t virt);

paddr_t mmu_init_kernel_dir(void);

paddr_t mmu_init_task_dir(paddr_t phy_start);
#endif //  __MMU_H__
