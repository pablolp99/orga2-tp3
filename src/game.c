/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "game.h"
#include "mapa.h"
#include "types.h"
#include "i386.h"
#include "sched.h"


size_t count_spawned = 0;
size_t count_killed = 0;
game_state_t game_state = {0};

bool stack_pointer_in_range(uint32_t esp) {
	return esp < TASK_STACK_BASE;
}

void save_game_state_error_code(uint32_t exception_id, uint32_t edi, uint32_t esi, uint32_t ebp, uint32_t current_esp, uint32_t ebx, uint32_t edx, 
                        uint32_t ecx, uint32_t eax, uint32_t error_code, uint32_t eip, uint32_t cs, uint32_t eflags, uint32_t error_esp, uint32_t ss){
	game_state.error_state = (error_state_t) {0};
	game_state.error_state.exception_id = exception_id;
    game_state.error_state.edi = edi;
    game_state.error_state.esi = esi;
    game_state.error_state.ebp = ebp;
    game_state.error_state.ebx = ebx;
    game_state.error_state.edx = edx;
    game_state.error_state.ecx = ecx;
    game_state.error_state.eax = eax;
    game_state.error_state.ss  = ss;
    game_state.error_state.esp = error_esp;
    game_state.error_state.eip = eip;
    game_state.error_state.eflags = eflags;

    game_state.error_state.cs = cs;
    game_state.error_state.ds = ss;
    game_state.error_state.es = ss;
    game_state.error_state.fs = ss;
    game_state.error_state.gs = ss;

    game_state.error_state.cr0 = rcr0();
    game_state.error_state.cr2 = rcr2();
    game_state.error_state.cr3 = rcr3();
    game_state.error_state.cr4 = rcr4();
	
    game_state.error_state.stack0 = stack_pointer_in_range(error_esp)? *((uint32_t*)(error_esp)) : 0;
    game_state.error_state.stack1 = stack_pointer_in_range(error_esp + 4)? *((uint32_t*)(error_esp + 4)) : 0;
    game_state.error_state.stack2 = stack_pointer_in_range(error_esp + 8)? *((uint32_t*)(error_esp + 8)) : 0;

    game_state.error_state.error_code = error_code;

	    // Esto existe para que no rompa por unused_param 
    game_state.error_state.current_esp = current_esp;
}

void save_game_state_without_err_code(uint32_t exception_id, uint32_t edi, uint32_t esi, uint32_t ebp, uint32_t current_esp, uint32_t ebx, uint32_t edx, 
                        uint32_t ecx, uint32_t eax, uint32_t eip, uint32_t cs, uint32_t eflags, uint32_t esp, uint32_t ss){
	save_game_state_error_code(exception_id, edi, esi, ebp, current_esp, ebx, edx, ecx, eax, 0x00000000, eip, cs, eflags, esp, ss);
}

void notify_debug_key_pressed(uint8_t scan_code) {
	// Check if scan_code is y (scan_code for y is 0x15)
	if (scan_code == 0x15) {
		game_state.debug_state.debug_enabled = true;
        print((game_state.debug_state.debug_enabled?"D":""),0,0,0x0F);
		close_debug_screen();
	} 
    if(scan_code == 0x16){
        int a = 5;
        a = 1 / (5-a);
    }
}

bool position_free_of_lemmings(coords_t coords) {
    for (int i = 0; i < MAX_LEMMINGS_TASKS; ++i) {
        if (game_state.lemmings_coords[i].x == coords.x &&
            game_state.lemmings_coords[i].y == coords.y) return false;
    }

    return true;
}

bool position_in_map_range(coords_t coords) {
    bool in_range = coords.x >= 0 && coords.x <= 79 && coords.y >= 0 && coords.y <= 39;
    return in_range;
}

void game_tick(void) {
    game_state.ticks_to_kill++;
    game_state.ticks_to_create++;

    // Borramos los lemmings si el equipo x esta lleno
    if (game_state.ticks_to_kill == TICKS_TO_KILL) {
        game_state.ticks_to_kill = 0;

        if (game_state.lemmings_state.team_a_lemmings_count == MAX_LEMMINGS_PER_TEAM) {
            unallocate_task(look_for_oldest_task(0));
        }

        if (game_state.lemmings_state.team_b_lemmings_count == MAX_LEMMINGS_PER_TEAM) {
            unallocate_task(look_for_oldest_task(1));
        }
    }

    // Creamos los lemmings si el equipo x no esta lleno
    if (game_state.ticks_to_create == TICKS_TO_CREATE) {
        game_state.ticks_to_create = 0;

        if(game_state.lemmings_state.team_a_lemmings_count < MAX_LEMMINGS_PER_TEAM) {
            create_sched_lemming_task(TeamA);
        }

        if(game_state.lemmings_state.team_b_lemmings_count < MAX_LEMMINGS_PER_TEAM) {
            create_sched_lemming_task(TeamB);
        }
    }
}

void game_init(void) {
	// Inicializamos el modo debug apagado
    game_state.ticks_to_kill = 0;
    game_state.ticks_to_create = 0;
	game_state.debug_state.debug_enabled = false;
    game_state.team_a_score = 0;
    game_state.team_b_score = 0;
    for (size_t i = 0; i < MAX_LEMMINGS_TASKS; ++i)
    {
        game_state.lemmings_coords[i] = (coords_t) { .x = -1, .y = -1 };
    }
    
}
void end_game() {
    if(is_current_task_from_team_a())
        print("Winner: Amarin", 1,0, 0x0F);
    else
        print("Winner: Betarote", 1,0, 0x0F);

    for (uint8_t i = 0; i < MAX_LEMMINGS_TASKS; ++i) {
        unallocate_task(i);
    }
    hlt();
}
bool move_to_win(coords_t next_coords) {
    return (is_current_task_from_team_a() && next_coords.x == 79) ||
        (!is_current_task_from_team_a() && next_coords.x == 0);
}
move_result_e move_if_possible(int delta_y, int delta_x) {
    coords_t current_coords = game_state.lemmings_coords[get_current_task_id()];
    coords_t next_coords = (coords_t) { .x = current_coords.x + delta_x, .y = current_coords.y + delta_y };

    move_result_e fail_move = 99;
    // Chequeamos si el movimiento hace que termine el juego
    if (move_to_win(next_coords)) end_game();
    // Checkeamos que no se vaya del mapa
    if (!position_in_map_range(next_coords)) fail_move = BORDER;
    // Checkeamos que en la proxima posición no haya un lemming
    if (!position_free_of_lemmings(next_coords)) fail_move = LEMMING;
    // Checkeamos que no haya agua en la proxima posicion
    if (is_water(next_coords)) fail_move = LAKE;
    // Checkeamos que no haya ladrillo en la proxima posicion
    if (is_wall(next_coords)) fail_move = WALL;

    if(fail_move != 99){
        game_state.lemmings_last_movement[get_current_task_id()] = fail_move;
        return fail_move;
    }
    // Podemos hacer el movimiento
    // Limpiamos el casillero viejo del lemming
    clean_lemming(get_current_task_id());
    // Le asignamos la nueva posicion al lemming
    game_state.lemmings_coords[get_current_task_id()] = next_coords;
    // Printeamos el lemming en el nuevo casillero
    team_e team = is_current_task_from_team_a()? TeamA : TeamB;
    print_lemming(team, next_coords);

    game_state.lemmings_last_movement[get_current_task_id()] = SUCCESS;
    return SUCCESS;
}


move_result_e execute_move(direction_e direction) {
    switch(direction) {
        case UP: return move_if_possible(-1, 0);
        case DOWN: return move_if_possible(1, 0);
        case LEFT: return move_if_possible(0, -1);
        case RIGHT: return move_if_possible(0, 1);
    }
    // La direccion es incorrecta, desalojamos y cortamos
    unallocate_current_task();
    return -1;
}

move_result_e get_last_movement(){
    return game_state.lemmings_last_movement[get_current_task_id()];
}

void bridge_if_possible(int delta_y, int delta_x) {
    coords_t current_coords = game_state.lemmings_coords[get_current_task_id()];
    coords_t next_coords = (coords_t) { .x = current_coords.x + delta_x, .y = current_coords.y + delta_y };
    bridge_if_water(next_coords);
}

void execute_bridge(direction_e direction) {
    switch(direction) {
        case UP: bridge_if_possible(-1, 0);break;
        case DOWN: bridge_if_possible(1, 0);break;
        case LEFT: bridge_if_possible(0, -1);break;
        case RIGHT: bridge_if_possible(0, 1);break;
    }
}

int abs(int a){
    if (a < 0){
        return -a;
    }
    return a;
}

bool is_nearby(coords_t current, coords_t nearby){
    return (abs(nearby.x - current.x) + abs(nearby.y - current.y) == 1);
}

void explode_lemming(void) {
    
    coords_t current_coords = game_state.lemmings_coords[get_current_task_id()];
    
    // Ir al mapa y "explotar" las paredes de ladrillo
    explode_if_wall((coords_t) { .x = current_coords.x + 1, .y = current_coords.y  });
    explode_if_wall((coords_t) { .x = current_coords.x, .y = current_coords.y + 1 });
    explode_if_wall((coords_t) { .x = current_coords.x - 1, .y = current_coords.y  });
    explode_if_wall((coords_t) { .x = current_coords.x, .y = current_coords.y -1 });
    
    // Ver si tiene algun lemming cerca, y kill
    for (int i = 0; i < MAX_LEMMINGS_TASKS; ++i){
        if (i != get_current_task_id()){
            bool lemming_nearby = is_nearby(current_coords, game_state.lemmings_coords[i]);
            if (lemming_nearby){
                // Desalojamos
                unallocate_task(i);
            }
        }
    }
}

// Map functions
uint8_t map_object_color(char object){
    switch(object) {
        case 'P': return 0x46; // back red, fore brown
        case 'L': return 0x11; // back blue, fore blue
        case '.': return 0x22; // back green, fore green
        case '+': return 0x17; // back blue, fore grey
        case 'X': return 0x42; // back red, fore green
        case 'A': return 0x47; // back red, fore grey
        case 'B': return 0x17; // back blue, fore grey
    }

    return 0x00;
}

bool is_floor(coords_t coords) {
    return mapa_original[coords.y][coords.x] == '.';
}

bool is_water(coords_t coords) {
    return mapa_original[coords.y][coords.x] == 'L';
}

bool is_wall(coords_t coords) {
    return mapa_original[coords.y][coords.x] == 'P';
}

void bridge_if_water(coords_t coords) {
    if(is_water(coords)) {
        mapa_original[coords.y][coords.x] = '+';
        //redibujar el mapa
        print_char('+', coords.x, coords.y+1, map_object_color('+'));
    }
}

void explode_if_wall(coords_t coords){
    if (is_wall(coords)){
        mapa_original[coords.y][coords.x] = 'X';
        print_char('X', coords.x, coords.y+1, map_object_color('X'));
    }
}