/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Declaracion de funciones del scheduler.
*/

#ifndef __SCHED_H__
#define __SCHED_H__

#include "types.h"

void sched_init();
uint16_t sched_next_task();
bool is_current_task_from_team_a();
uint8_t get_current_task_id();
uint8_t look_for_oldest_task(uint8_t initial_idx);
void unallocate_task(uint8_t task_idx);
void unallocate_current_task();
bool create_sched_lemming_task(team_e from_team);
uint8_t sched_map_shared(vaddr_t vaddr, uint32_t cr3);

extern sched_task_entry_t tasks[];

#endif //  __SCHED_H__
