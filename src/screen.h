/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Declaracion de funciones del scheduler
*/

#ifndef __SCREEN_H__
#define __SCREEN_H__

#include "colors.h"
#include "defines.h"
#include "stdint.h"
#include "types.h"

void print(const char* text, uint32_t x, uint32_t y, uint16_t attr);
void print_dec(uint32_t numero, uint32_t size, uint32_t x, uint32_t y,
               uint16_t attr);
void print_hex(uint32_t numero, int32_t size, uint32_t x, uint32_t y,
               uint16_t attr);

void screen_draw_box(uint32_t fInit, uint32_t cInit, uint32_t fSize,
                     uint32_t cSize, uint8_t character, uint8_t attr);

void screen_draw_layout(void);

void print_interruption(uint32_t isr_number);

void print_dec_input(uint8_t scan_code);

void print_libretas(void);

void print_teams_data(void);

void print_teams_score(void);

void print_screen_data(void);

void clean_clock_of_task(uint8_t task_id);

void print_debug();

void close_debug_screen();

void print_lemming(team_e from_team, coords_t at);

void clean_lemming(uint8_t task_id);

void print_char(char c, uint32_t x, uint32_t y, uint16_t attr);

#endif //  __SCREEN_H__
