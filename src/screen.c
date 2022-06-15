/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definicion de funciones de impresion por pantalla.
*/

#include "screen.h"
#include "i386.h"
#include "sched.h"
#include "types.h"
#include "game.h"
#include "mapa.h"

char* interruption_codes[20] = {
	"0 - Divide Error",
	"1 - RESERVED", // No Use
	"2 - NMI Interrupt",
	"3 - Breakpoint",
	"4 - Overflow",
	"5 - Bound range exceded",
	"6 - Invalid Opcode",
	"7 - Device not available",
	"8 - Double Fault",
	"9 - Coprocessor Segment overrun",
	"10 - Invalid TSS",
	"11 - Segment not present",
	"12 - Stack-segment fault",
	"13 - General protection",
	"14 - Page fault",
	"15 - RESERVED", // No use
	"16 - x87 FPU Floating-Point error",
	"17 - Alignment check",
	"18 - Machine check",
	"19 - SIMD Floating-Point exception"
};

uint32_t dec_scancodes_values[12] = {
  0, // No use
  0, // No use
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  0
};

char* clock_states[4] = {"|","/","-","\\"};

ca(*saved_screen_state)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])&game_state.debug_state.saved_screen_state_buffer;

void print(const char* text, uint32_t x, uint32_t y, uint16_t attr) {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO; // magia
  int32_t i;
  for (i = 0; text[i] != 0; i++) {
    p[y][x].c = (uint8_t)text[i];
    p[y][x].a = (uint8_t)attr;
    x++;
    if (x == VIDEO_COLS) {
      x = 0;
      y++;
    }
  }
}

void print_char(char text, uint32_t x, uint32_t y, uint16_t attr) {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO; // magia
  p[y][x].c = (uint8_t)text;
  p[y][x].a = (uint8_t)attr;
}

void print_dec(uint32_t numero, uint32_t size, uint32_t x, uint32_t y,
               uint16_t attr) {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO; // magia
  uint32_t i;
  uint8_t letras[16] = "0123456789";

  for (i = 0; i < size; i++) {
    uint32_t resto = numero % 10;
    numero = numero / 10;
    p[y][x + size - i - 1].c = letras[resto];
    p[y][x + size - i - 1].a = attr;
  }
}

void print_hex(uint32_t numero, int32_t size, uint32_t x, uint32_t y,
               uint16_t attr) {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO; // magia
  int32_t i;
  uint8_t hexa[8];
  uint8_t letras[16] = "0123456789ABCDEF";
  hexa[0] = letras[(numero & 0x0000000F) >> 0];
  hexa[1] = letras[(numero & 0x000000F0) >> 4];
  hexa[2] = letras[(numero & 0x00000F00) >> 8];
  hexa[3] = letras[(numero & 0x0000F000) >> 12];
  hexa[4] = letras[(numero & 0x000F0000) >> 16];
  hexa[5] = letras[(numero & 0x00F00000) >> 20];
  hexa[6] = letras[(numero & 0x0F000000) >> 24];
  hexa[7] = letras[(numero & 0xF0000000) >> 28];
  for (i = 0; i < size; i++) {
    p[y][x + size - i - 1].c = hexa[i];
    p[y][x + size - i - 1].a = attr;
  }
}

void screen_draw_box(uint32_t fInit, uint32_t cInit, uint32_t fSize,
                     uint32_t cSize, uint8_t character, uint8_t attr) {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO;
  uint32_t f;
  uint32_t c;
  for (f = fInit; f < fInit + fSize; f++) {
    for (c = cInit; c < cInit + cSize; c++) {
      p[f][c].c = character;
      p[f][c].a = attr;
    }
  }
}

void print_interruption(uint32_t isr_number) {	
	print(interruption_codes[isr_number], 0, 0, 0x7F);
}

void print_dec_input(uint8_t scan_code) {
  // Check if scan_code is between [0x02, 0x0B] (make of 0 to 9)
  if (scan_code >= 0x02 && scan_code <= 0x0B) {
    print_dec(dec_scancodes_values[scan_code], 1, 0, 0, 0x7F);
  }
}

void print_debug_screen(void){
  uint8_t data_colour = 0x02;
  uint8_t title_colour = 0x07;

  /* PAINT IT BLACK */
  screen_draw_box(1, 20, 40, 40, 0, 0x00);
  /* END PAINT IT BLACK*/

  /* PRINT INTERRUPTION AND TASK INFO */

  // Print interruption title with code
  print(interruption_codes[game_state.error_state.exception_id], DEBUG_SCREEN_START_X + 1, DEBUG_SCREEN_START_Y + 1, data_colour);
  // Print task id that throwed the exception
  print_dec(get_current_task_id(), 3, DEBUG_SCREEN_END_X - 3, DEBUG_SCREEN_START_Y + 1, data_colour);
    
  /* END PRINT INTERRUPTION AND TASK INFO */

  /* PRINT REGS */

  // Print eax
  print("eax", DEBUG_SCREEN_START_X + 2, DEBUG_SCREEN_START_Y + 6, title_colour);
  print_hex(game_state.error_state.eax, 8, DEBUG_SCREEN_START_X + 6, DEBUG_SCREEN_START_Y + 6, data_colour);
  // Print ebx
  print("ebx", DEBUG_SCREEN_START_X + 2, DEBUG_SCREEN_START_Y + 8, title_colour);
  print_hex(game_state.error_state.ebx, 8, DEBUG_SCREEN_START_X + 6, DEBUG_SCREEN_START_Y + 8, data_colour);
  // Print ecx
  print("ecx", DEBUG_SCREEN_START_X + 2, DEBUG_SCREEN_START_Y + 10, title_colour);
  print_hex(game_state.error_state.ecx, 8, DEBUG_SCREEN_START_X + 6, DEBUG_SCREEN_START_Y + 10, data_colour);
  // Print edx
  print("edx", DEBUG_SCREEN_START_X + 2, DEBUG_SCREEN_START_Y + 12, title_colour);
  print_hex(game_state.error_state.edx, 8, DEBUG_SCREEN_START_X + 6, DEBUG_SCREEN_START_Y + 12, data_colour);
  // Print esi
  print("esi", DEBUG_SCREEN_START_X + 2, DEBUG_SCREEN_START_Y + 14, title_colour);
  print_hex(game_state.error_state.esi, 8, DEBUG_SCREEN_START_X + 6, DEBUG_SCREEN_START_Y + 14, data_colour);
  // Print edi
  print("edi", DEBUG_SCREEN_START_X + 2, DEBUG_SCREEN_START_Y + 16, title_colour);
  print_hex(game_state.error_state.edi, 8, DEBUG_SCREEN_START_X + 6, DEBUG_SCREEN_START_Y + 16, data_colour);
  // Print ebp
  print("ebp", DEBUG_SCREEN_START_X + 2, DEBUG_SCREEN_START_Y + 18, title_colour);
  print_hex(game_state.error_state.ebp, 8, DEBUG_SCREEN_START_X + 6, DEBUG_SCREEN_START_Y + 18, data_colour);
  // Print esp
  print("esp", DEBUG_SCREEN_START_X + 2, DEBUG_SCREEN_START_Y + 20, title_colour);
  print_hex(game_state.error_state.esp, 8, DEBUG_SCREEN_START_X + 6, DEBUG_SCREEN_START_Y + 20, data_colour);
  // Print eip
  print("eip", DEBUG_SCREEN_START_X + 2, DEBUG_SCREEN_START_Y + 22, title_colour);
  print_hex(game_state.error_state.eip, 8, DEBUG_SCREEN_START_X + 6, DEBUG_SCREEN_START_Y + 22, data_colour);
  // Print cs
  print("cs", DEBUG_SCREEN_START_X + 3, DEBUG_SCREEN_START_Y + 24, title_colour);
  print_hex(game_state.error_state.cs, 8, DEBUG_SCREEN_START_X + 6, DEBUG_SCREEN_START_Y + 24, data_colour);
  // Print ds
  print("ds", DEBUG_SCREEN_START_X + 3, DEBUG_SCREEN_START_Y + 26, title_colour);
  print_hex(game_state.error_state.ds, 8, DEBUG_SCREEN_START_X + 6, DEBUG_SCREEN_START_Y + 26, data_colour);
  // Print es
  print("es", DEBUG_SCREEN_START_X + 3, DEBUG_SCREEN_START_Y + 28, title_colour);
  print_hex(game_state.error_state.es, 8, DEBUG_SCREEN_START_X + 6, DEBUG_SCREEN_START_Y + 28, data_colour);
  // Print fs
  print("fs", DEBUG_SCREEN_START_X + 3, DEBUG_SCREEN_START_Y + 30, title_colour);
  print_hex(game_state.error_state.fs, 8, DEBUG_SCREEN_START_X + 6, DEBUG_SCREEN_START_Y + 30, data_colour);
  // Print gs
  print("gs", DEBUG_SCREEN_START_X + 3, DEBUG_SCREEN_START_Y + 32, title_colour);
  print_hex(game_state.error_state.gs, 8, DEBUG_SCREEN_START_X + 6, DEBUG_SCREEN_START_Y + 32, data_colour);
  // Print ss
  print("ss", DEBUG_SCREEN_START_X + 3, DEBUG_SCREEN_START_Y + 34, title_colour);
  print_hex(game_state.error_state.ss, 8, DEBUG_SCREEN_START_X + 6, DEBUG_SCREEN_START_Y + 34, data_colour);

  /* END PRINT REGS */

  /* PRINT CR_X and ERR */

  // print CR_0
  print("cr0", DEBUG_SCREEN_END_X - 12, DEBUG_SCREEN_START_Y + 7, title_colour);
  print_hex(game_state.error_state.cr0, 8, DEBUG_SCREEN_END_X - 8, DEBUG_SCREEN_START_Y + 7, data_colour);
  // print CR_2
  print("cr2", DEBUG_SCREEN_END_X - 12, DEBUG_SCREEN_START_Y + 9, title_colour);
  print_hex(game_state.error_state.cr2, 8, DEBUG_SCREEN_END_X - 8, DEBUG_SCREEN_START_Y + 9, data_colour);
  // print CR_3
  print("cr3", DEBUG_SCREEN_END_X - 12, DEBUG_SCREEN_START_Y + 11, title_colour);
  print_hex(game_state.error_state.cr3, 8, DEBUG_SCREEN_END_X - 8, DEBUG_SCREEN_START_Y + 11, data_colour);
  // print CR_4
  print("cr4", DEBUG_SCREEN_END_X - 12, DEBUG_SCREEN_START_Y + 13, title_colour);
  print_hex(game_state.error_state.cr4, 8, DEBUG_SCREEN_END_X - 8, DEBUG_SCREEN_START_Y + 13, data_colour);
  // print Error code
  print("err", DEBUG_SCREEN_END_X - 12, DEBUG_SCREEN_START_Y + 15, title_colour);
  print_hex(game_state.error_state.error_code, 8, DEBUG_SCREEN_END_X - 8, DEBUG_SCREEN_START_Y + 15, data_colour);
  
  /* END PRINT CR_X and ERR */

  /* PRINT STACK */
  
  // print stack title
  print("stack", DEBUG_SCREEN_END_X - 18, DEBUG_SCREEN_START_Y + 19, title_colour);
  // print stack_2
  print_hex(game_state.error_state.stack2, 8, DEBUG_SCREEN_END_X - 18, DEBUG_SCREEN_START_Y + 21, data_colour);
  // print stack_1
  print_hex(game_state.error_state.stack1, 8, DEBUG_SCREEN_END_X - 18, DEBUG_SCREEN_START_Y + 23, data_colour);
  // print stack_0
  print_hex(game_state.error_state.stack0, 8, DEBUG_SCREEN_END_X - 18, DEBUG_SCREEN_START_Y + 25, data_colour);

  /* END STACK */

  /* PRINT EFLAGS */

  // Print eflags
  print("eflags", DEBUG_SCREEN_START_X + 2, DEBUG_SCREEN_END_Y - 2, title_colour);
  print_hex(game_state.error_state.eflags, 8, DEBUG_SCREEN_START_X + 9, DEBUG_SCREEN_END_Y - 2, data_colour);

  /* END EFLAGS */
}

void recover_previous_screen_state() {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO; //magia

  for (int x = 0; x < VIDEO_COLS; ++x) {
    for (int y = 0; y < VIDEO_FILS; ++y) {
      p[y][x].c = saved_screen_state[y][x].c;
      p[y][x].a = saved_screen_state[y][x].a;
    }
  }
}

void save_screen_state() {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO; //magia

  for (int x = 0; x < VIDEO_COLS; ++x) {
    for (int y = 0; y < VIDEO_FILS; ++y) {
      saved_screen_state[y][x].c = p[y][x].c;
      saved_screen_state[y][x].a = p[y][x].a;
    }
  }
}

void print_debug() {
  if (game_state.debug_state.debug_enabled && !game_state.debug_state.is_showing) {
    // Guardamos el estado de la pantalla actual
    save_screen_state();
    // Mostramos el debug
    print_debug_screen();

    game_state.debug_state.is_showing = true;
  }
}

void close_debug_screen() {
  if (game_state.debug_state.is_showing) {
    recover_previous_screen_state();

    game_state.debug_state.is_showing = false;
  }
}

void print_teams_data() {
  // Printeamos los nombres de cada equipo
  print("Amarin", 10, 42, 0x04); // Back negro, fore rojo
  print("Betarote", 63, 42, 0x01); // Back negro, fore azul
  // Printeamos los titulos de cada lemming
  for (int i = 0; i < MAX_LEMMINGS_PER_TEAM; ++i) {
    print_dec(i, 2, 22 + (i * 4), 42, 0x04); // Back negro, fore rojo
  }
  for (int i = 0; i < MAX_LEMMINGS_PER_TEAM; ++i) {
    print_dec(i, 2, 42 + (i * 4), 42, 0x01); // Back negro, fore azul
  }
  // Printeamos los puntajes iniciales
  print_dec(0, 6, 12, 45, 0x47); // Back rojo, fore gris
  print_dec(0, 6, 65, 45, 0x17); // Back azul, fore gris
}

void print_teams_score() {
  // Printeamos los puntajes actuales
  print_dec(game_state.team_a_score, 6, 12, 45, 0x47); // Back rojo, fore gris
  print_dec(game_state.team_b_score, 6, 65, 45, 0x17); // Back azul, fore gris
}
void print_initial_map() {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO;

  // Recorremos el mapa
  for (int x = 0; x < 80; ++x) {
    for (int y = 1; y < 41; ++y) {
      p[y][x].c = mapa_original[y-1][x];
      p[y][x].a = map_object_color(mapa_original[y-1][x]);
    }
  }
}

void print_screen_data() {
  print_initial_map();
  print_teams_data();
}

void print_task_clock(char* to_print, uint8_t task_id) {  
  uint8_t base_x_coord;
  uint8_t modifier = 0;
  if (task_id % 2 == 0) { base_x_coord = 22; } else { base_x_coord = 42; modifier = 1;}

  // Printeamos el reloj
  print(to_print, base_x_coord + (task_id - modifier) * 2, 43, 0x07);
}

void print_current_task_clock() {
  uint8_t current_task_idx = get_current_task_id();
  char* current_state = clock_states[tasks[current_task_idx].clock_state];
  print_task_clock(current_state, current_task_idx);

  // Lo dejamos al siguiente state
  tasks[current_task_idx].clock_state = (tasks[current_task_idx].clock_state + 1) % 4;
}

void clean_clock_of_task(uint8_t task_id) {
  print_task_clock(" ", task_id);
}

void print_libretas(void){
	print("701/17 816/17 007/18", 13, 13, 0x7F);
}

void print_lemming(team_e from_team, coords_t at) {
  char c = from_team == TeamA? 'A' : 'B';
  uint8_t a = map_object_color(c);

  print_char(c, at.x, at.y + 1, a);
}

void clean_lemming(uint8_t task_id) {
  coords_t lemmings_coords = game_state.lemmings_coords[task_id];
  char c = mapa_original[lemmings_coords.y][lemmings_coords.x];

  print_char(c, lemmings_coords.x, lemmings_coords.y + 1, map_object_color(c));
}

