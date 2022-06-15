/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Declaracion de las rutinas asociadas al juego.
*/

#ifndef __GAME_H__
#define __GAME_H__

#include "types.h"
#include "screen.h"
#include "mapa.h"

void game_tick(void);
void game_init(void);

void save_game_state_error_code(uint32_t exception_id, uint32_t edi, uint32_t esi, uint32_t ebp, uint32_t current_esp, uint32_t ebx, uint32_t edx, 
                        uint32_t ecx, uint32_t eax, uint32_t error_code, uint32_t eip, uint32_t cs, uint32_t eflags, uint32_t esp, uint32_t ss);

void save_game_state_without_err_code(uint32_t exception_id, uint32_t edi, uint32_t esi, uint32_t ebp, uint32_t current_esp, uint32_t ebx, uint32_t edx, 
                        uint32_t ecx, uint32_t eax, uint32_t eip, uint32_t cs, uint32_t eflags, uint32_t esp, uint32_t ss);

void notify_debug_key_pressed(uint8_t scan_code);

bool position_free_of_lemmings(coords_t coords);

move_result_e execute_move(direction_e direction);

void execute_bridge(direction_e direction);
void explode_lemming(void);

// Map functions
uint8_t map_object_color(char object);
bool is_floor(coords_t coords);
bool is_water(coords_t coords);
bool is_wall(coords_t coords);
void bridge_if_water(coords_t coords);
void explode_if_wall(coords_t coords);
move_result_e get_last_movement(void);
extern game_state_t game_state;

#endif //  __GAME_H__
