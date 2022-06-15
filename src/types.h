/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Declaracion de tipos comunes en el kernel.
*/

#ifndef __TYPES_H__
#define __TYPES_H__

#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"
#include "defines.h"

typedef uint32_t vaddr_t; // direccion virtual.
typedef uint32_t paddr_t; // direccion fisica.

/* Estructura de para acceder a memoria de video */
typedef struct ca_s {
  uint8_t c;
  uint8_t a;
} ca;

typedef enum direction_e { UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3 } direction_e;

typedef enum move_result_e {
  SUCCESS = 0,
  WALL = 1,
  LAKE = 2,
  BORDER = 3,
  LEMMING = 4
} move_result_e;

typedef struct str_page_directory_entry {
  uint8_t  present:1;
  uint8_t  read_write:1;
  uint8_t  user_supervisor:1;
  uint8_t  pwt:1;
  uint8_t  pcd:1;
  uint8_t  accessed:1;
  uint8_t  ignored:1;
  uint8_t  ps:1;
  uint8_t  global:1;
  uint8_t  available:3;
  uint32_t base_table_address:20;
} __attribute__((packed)) page_directory_entry;

typedef struct str_page_table_entry {
  uint8_t  present:1;
  uint8_t  read_write:1;
  uint8_t  user_supervisor:1;
  uint8_t  pwt:1;
  uint8_t  pcd:1;
  uint8_t  accessed:1;
  uint8_t  dirty:1;
  uint8_t  pat:1;
  uint8_t  global:1;
  uint8_t  available:3;
  uint32_t base_page_address:20;
} __attribute__((packed)) page_table_entry;

typedef enum e_team {
  NoTeam = 0,
  TeamA  = 1,
  TeamB  = 2
} team_e;

typedef struct str_coords {
  int x;
  int y;
} __attribute__((packed)) coords_t;

typedef struct str_sched_task_entry {
  uint16_t tss_selector;
  uint8_t  creation_order;
  bool     allocated;
  uint8_t  clock_state;
} __attribute__((packed)) sched_task_entry_t;

typedef struct str_sched_task_state{
  uint8_t team_a_idx;
  uint8_t team_b_idx;
  team_e team_executed;
} __attribute__((packed)) sched_task_state_t;

typedef struct str_sched_lemmings_state{
  uint32_t team_a_lemmings_count;
  uint32_t team_b_lemmings_count;
} __attribute__((packed)) sched_lemmings_state_t;

typedef struct str_error_state {
  uint32_t   eax;
  uint32_t   ebx;
  uint32_t   ecx;
  uint32_t   edx;
  uint32_t   esi;
  uint32_t   edi;
  uint32_t   ebp;
  uint32_t   esp;
  uint32_t   eip;
  uint16_t   cs;
  uint16_t   ds;
  uint16_t   es;
  uint16_t   fs;
  uint16_t   gs;
  uint16_t   ss;
  uint32_t   eflags;
  uint32_t   cr0;
  uint32_t   cr2;
  uint32_t   cr3;
  uint32_t   cr4;
  uint32_t   stack0;
  uint32_t   stack1;
  uint32_t   stack2;
  uint32_t   exception_id;
  uint32_t   error_code;
  uint32_t   current_esp; // Esto existe para que no rompa por unused_param 
} __attribute__((__packed__)) error_state_t;

typedef struct str_debug_state{
  bool debug_enabled;
  bool is_showing;
  ca saved_screen_state_buffer[VIDEO_COLS * VIDEO_FILS]; // Lo usamos para obtener dinamicamente un espacio de memoria
} __attribute__((packed)) debug_state_t;

typedef struct str_game_state {
  sched_task_state_t last_task_state;
  sched_lemmings_state_t lemmings_state;
  error_state_t error_state;
  debug_state_t debug_state;
  coords_t lemmings_coords[10];
  uint32_t ticks_to_create;
  uint32_t ticks_to_kill;
  uint8_t team_a_score;
  uint8_t team_b_score;
  move_result_e lemmings_last_movement[MAX_LEMMINGS_TASKS];
} __attribute__((packed)) game_state_t;

typedef struct str_shared_map{
  paddr_t team_a[SHARED_PAGES_MAX];
  paddr_t team_b[SHARED_PAGES_MAX];
} __attribute__((packed)) shared_map_t;

#endif //  __TYPES_H__
