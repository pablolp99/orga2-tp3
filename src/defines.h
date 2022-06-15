/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definiciones globales del sistema.
*/

#ifndef __DEFINES_H__
#define __DEFINES_H__
/* MMU */
/* -------------------------------------------------------------------------- */

#define MMU_P (1 << 0)
#define MMU_W (1 << 1)
#define MMU_U (1 << 2)

#define PAGE_SIZE 4096

/* Misc */
/* -------------------------------------------------------------------------- */
// Y Filas
#define SIZE_N 40
#define ROWS   SIZE_N

// X Columnas
#define SIZE_M 80
#define COLS   SIZE_M

/* Selectores de segmento */
/* -------------------------------------------------------------------------- */
#define CODE_SELECTOR_0 0x0040
#define DATA_SELECTOR_0 0x0050
#define CODE_SELECTOR_3 0x004B
#define DATA_SELECTOR_3 0x005B
#define IDLE_SELECTOR   0x0078

/* Indices en la gdt */
/* -------------------------------------------------------------------------- */
#define GDT_IDX_NULL_DESC 0
#define GDT_COUNT         35

#define GDT_IDX_CODE_0 8
#define GDT_IDX_CODE_3 9
#define GDT_IDX_DATA_0 10
#define GDT_IDX_DATA_3 11

#define GDT_IDX_VIDEO_0 12

#define GDT_IDX_INITIAL_TSS  	   14
#define GDT_IDX_IDLE_TSS	 	   15
#define GDT_IDX_FIRST_LEMMINGS_TSS 16

/* Bases y limites de segmentos de la gdt */
/* -------------------------------------------------------------------------- */

#define GDT_DESC_BASE_15_0_FLAT 0x0000
#define GDT_DESC_BASE_23_16_FLAT 0x00
#define GDT_DESC_BASE_31_24_FLAT 0x00
#define GDT_DESC_LIMIT_15_0 0x30FF
#define GDT_DESC_LIMIT_19_16 0x3

#define GDT_DESC_BASE_15_0_VIDEO 0x8000
#define GDT_DESC_BASE_23_16_VIDEO 0x0B
#define GDT_DESC_BASE_31_24_VIDEO 0x00
#define GDT_DESC_LIMIT_15_0_VIDEO 0x7FFF
#define GDT_DESC_LIMIT_19_16_VIDEO 0x0

#define GDT_DESC_TSS_LIMIT_15_0  0x67
#define GDT_DESC_TSS_LIMIT_19_16 0x0
#define GDT_DESC_TSS_TYPE 	     0x9 //1001
#define GDT_DESC_TSS_S 		     0x0

/* Offsets en la gdt */
/* -------------------------------------------------------------------------- */
#define GDT_OFF_NULL_DESC (GDT_IDX_NULL_DESC << 3)

/* IDT */
/* -------------------------------------------------------------------------- */
//                                          P DPL D                   12    8    4    0
#define KERNEL_LEVEL_INTERRUPTION 0x8E00 // 1 00 01110 000 00000 -> 1000 1110 0000 0000 
#define USER_LEVEL_INTERRUPTION 0xEE00   // 1 11 01110 000 00000 -> 1110 1110 0000 0000  

/* Direcciones de memoria */
/* -------------------------------------------------------------------------- */

// direccion fisica de comienzo del bootsector (copiado)
#define BOOTSECTOR 0x00001000
// direccion fisica de comienzo del kernel
#define KERNEL 0x00001200
// direccion fisica del buffer de video
#define VIDEO 0x000B8000
// direccion fisica de la copia del estado de la pantalla
#define VIDEO_SAVED_COPY 0x00027000

/* Direcciones virtuales de código, pila y datos */
/* -------------------------------------------------------------------------- */

// direccion virtual del codigo
#define TASK_CODE_VIRTUAL 0x08000000
#define TASK_CODE_PAGES   2
#define TASK_STACK_BASE   0x08003000

/* Direcciones fisicas de codigos */
/* -------------------------------------------------------------------------- */
/* En estas direcciones estan los códigos de todas las tareas. De aqui se
 * copiaran al destino indicado por TASK_<X>_PHY_START.
 */
#define USER_TASK_SIZE (PAGE_SIZE * 2)

#define TASK_LEMMING_A_CODE_START (0x00018000)
#define TASK_LEMMING_B_CODE_START (0x0001A000)
#define TASK_IDLE_START           (0x0001C000)

/* Direcciones fisicas de directorios y tablas de paginas del KERNEL */
/* -------------------------------------------------------------------------- */
#define KERNEL_PAGE_DIR     (0x00025000)
#define KERNEL_PAGE_TABLE_0 (0x00026000)
#define KERNEL_STACK        (0x00025000)

/* MMU Defines */
/* -------------------------------------------------------------------------- */

#define MMU_USER_SUPERVISOR 0
#define MMU_USER_UNPRIVILEGED 1
#define MMU_READ_WRITE 1
#define MMU_READ_ONLY 0
#define MMU_PRESENT 1

#define KERNEL_FREE_SPACE 0x100000
#define USER_FREE_SPACE   0x400000
#define SHARED_PAGES_SPACE_LIMIT 0x13FFFFF
#define SHARED_PAGES_MAX 4096

/* TSS Defines */
/* -------------------------------------------------------------------------- */
// valor por defecto para EFLAGS (para las TSS)
#define EFLAGS_DEFAULT_VALUE 0x00000202 // (...) 0010 0000 0010, habilita IF y valida EFLAGS 
// valor para deshabilitar todos los puertos I/O para una tarea (definido en la TSS)
#define TSS_IOMAP_ALL_PORTS_DISABLED 0xFFFF

/* Game Defines */
/* -------------------------------------------------------------------------- */
#define MAX_LEMMINGS_TASKS 10
#define MAX_LEMMINGS_PER_TEAM 5
#define TICKS_TO_KILL 2001
#define TICKS_TO_CREATE 401
#define TEAM_A_LEMMING_INIT_X 0
#define TEAM_A_LEMMING_INIT_Y 39
#define TEAM_B_LEMMING_INIT_X 79
#define TEAM_B_LEMMING_INIT_Y 0

/* Screen Defines */
/* -------------------------------------------------------------------------- */
// DEBUG SCREEN OFFSETS
#define DEBUG_SCREEN_START_X 20
#define DEBUG_SCREEN_END_X 59
#define DEBUG_SCREEN_START_Y 1
#define DEBUG_SCREEN_END_Y 40
#define VIDEO_FILS 50
#define VIDEO_COLS 80


#endif //  __DEFINES_H__
