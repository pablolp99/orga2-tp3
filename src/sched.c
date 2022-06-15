/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definicion de funciones del scheduler
*/

#include "sched.h"
#include "gdt.h"
#include "game.h"
#include "tss.h"
#include "screen.h"
#include "i386.h"
#include "mapa.h"
#include "mmu.h"

shared_map_t shared_map = {0};
sched_task_entry_t tasks[MAX_LEMMINGS_TASKS] = {0};
uint8_t INVALID_TASK_ID = MAX_LEMMINGS_TASKS + 1;

void fill_tss_gdt_entries(void) {
	for (int i = 0; i < MAX_LEMMINGS_TASKS; i++) {
		// Fill gdt without tss
		gdt[GDT_IDX_FIRST_LEMMINGS_TSS + i] = (gdt_entry_t) {
			.base_15_0 = 0,  //filled later
			.base_23_16 = 0, //filled later
			.base_31_24 = 0, //filled later
			.limit_15_0 = GDT_DESC_TSS_LIMIT_15_0,
			.limit_19_16 = GDT_DESC_TSS_LIMIT_19_16,
			.type = GDT_DESC_TSS_TYPE, 
			.s = GDT_DESC_TSS_S,
			.dpl = 0, //kernel
			.p = 1,   //present
			.avl = 0,
			.l = 0,
			.db = 0,
			.g = 0
		};
	}
}

uint8_t look_for_first_unallocated_id(uint8_t initial_idx) {
	for (int i = initial_idx; i < initial_idx + (MAX_LEMMINGS_TASKS); i+=2) {
		if (!tasks[i % MAX_LEMMINGS_TASKS].allocated) {
			return i;
		}
	}

	return INVALID_TASK_ID;
}

uint8_t look_for_oldest_task(uint8_t initial_idx) {
	for (uint8_t i = initial_idx; i < initial_idx + MAX_LEMMINGS_TASKS; i+=2) {
		if (tasks[i % MAX_LEMMINGS_TASKS].allocated && 
				tasks[i % MAX_LEMMINGS_TASKS].creation_order == 0) return i;
	}

	return INVALID_TASK_ID;
}

uint8_t next_creation(uint8_t initial_idx) {
	uint8_t max_creation_order = 0;
	bool all_unallocated = true;

	for (int i = initial_idx; i < initial_idx + (MAX_LEMMINGS_TASKS); i+=2) {
		if (tasks[i % MAX_LEMMINGS_TASKS].allocated && 
				max_creation_order <= tasks[i % MAX_LEMMINGS_TASKS].creation_order) {
			max_creation_order = tasks[i % MAX_LEMMINGS_TASKS].creation_order;
			all_unallocated = false;
		}
	}

	if (all_unallocated) { return 0; } else { return max_creation_order + 1; }
}

bool is_current_task_from_team_a() { return game_state.last_task_state.team_executed == TeamA; }

bool create_sched_lemming_task(team_e from_team) {
	paddr_t phy_code_start = 0;
	uint8_t team_initial_idx = 0;

	// Definimos que tarea a ejecutar y las posiciones del array segun el equipo
	if (from_team == TeamA) {
		phy_code_start = TASK_LEMMING_A_CODE_START;
		team_initial_idx = 0;
	} else {
		phy_code_start = TASK_LEMMING_B_CODE_START;
		team_initial_idx = 1;
	}

	uint8_t task_id = look_for_first_unallocated_id(team_initial_idx);
	// Si task_id es invalido, entonces es que no hay lugar para crear una tarea nueva
	if (task_id == INVALID_TASK_ID) {
		return false;
	}

	// Le ponemos las coords iniciales al lemming en el juego
	coords_t initial_coords;
	if (from_team == TeamA) { initial_coords = (coords_t) { .x = TEAM_A_LEMMING_INIT_X, .y = TEAM_A_LEMMING_INIT_Y }; } 
		else { initial_coords = (coords_t) { .x = TEAM_B_LEMMING_INIT_X, .y = TEAM_B_LEMMING_INIT_Y }; }

	// Checkeamos que las coords iniciales sea suelo
	if (!(position_free_of_lemmings(initial_coords) && is_floor(initial_coords))) {
		return false;
	}

	// Alojamos al lemming en las coords
	game_state.lemmings_coords[task_id] = initial_coords;
	// Updateamos el mapa con el lemming
	print_lemming(from_team, initial_coords);

	// Sumamos el contador de lemmigs del equipo correspondiente
	if (from_team == TeamA) {
		game_state.lemmings_state.team_a_lemmings_count++;
		game_state.team_a_score++;
	} else {
		game_state.lemmings_state.team_b_lemmings_count++;
		game_state.team_b_score++;
	}

	// Creamos la tss con entrada en la GDT a partir de la posicion GDT_IDX_FIRST_LEMMINGS_TSS
	uint8_t gdt_idx = GDT_IDX_FIRST_LEMMINGS_TSS + task_id;
	create_user_task_tss(gdt_idx, phy_code_start);

	// Llenamos la entrada en el array de tasks
	tasks[task_id] = (sched_task_entry_t) { .tss_selector = gdt_idx << 3, 
		.creation_order = next_creation(team_initial_idx), .allocated = true, .clock_state = 0 };

	// Printeamos los scores de ambos equipos.
	print_teams_score();

	return true;
}

uint8_t get_current_task_id() {
	if (is_current_task_from_team_a()) {
		return game_state.last_task_state.team_a_idx;
	} else {
		return game_state.last_task_state.team_b_idx;
	}
}

void unallocate_task(uint8_t task_id) {
	// Reorganizamos los creation orders (restamos 1 a cada uno)
	for (int i = task_id + 2; i < task_id + (MAX_LEMMINGS_TASKS); i+=2) {
		if (tasks[i % MAX_LEMMINGS_TASKS].allocated &&
			tasks[i % MAX_LEMMINGS_TASKS].creation_order > tasks[task_id].creation_order) {
			tasks[i % MAX_LEMMINGS_TASKS].creation_order = tasks[i % MAX_LEMMINGS_TASKS].creation_order - 1;
		}
	}

	// Restamos a los lemmings del equipo
	if (task_id % 2 == 0) { game_state.lemmings_state.team_a_lemmings_count--; } 
		else { game_state.lemmings_state.team_b_lemmings_count--; }
	// Sacamos la task
	tasks[task_id].allocated = false;

	// Limpiamos el espacio del clock
	clean_clock_of_task(task_id);
	// Limpiamos el lemming en el mapa
	clean_lemming(task_id);
	// Limpiamos las coords del lemming
	game_state.lemmings_coords[task_id] = (coords_t) { .x = -1, .y = -1 };
}

void unallocate_current_task() { unallocate_task(get_current_task_id()); }

uint8_t sched_map_shared(vaddr_t vaddr, uint32_t cr3) {
	if (vaddr >= USER_FREE_SPACE && vaddr <= SHARED_PAGES_SPACE_LIMIT) {
		paddr_t page = 0;
		// Le restamos el offset de 0x400000 y luego dividimos por PAGE_SIZE
		uint16_t page_number = (vaddr - USER_FREE_SPACE) / PAGE_SIZE; // Obtenemos un valor entre 0 y 4095

		// Checkeamos si ya estaba mapeada la pagina
		if (is_current_task_from_team_a()) {
			page = shared_map.team_a[page_number];
			// Si no se mapeo, pedimos una pagina nueva y mapeamos alli
			if (page == 0) { page = mmu_next_free_user_page(); shared_map.team_a[page_number] = page; }
		} else {
			page = shared_map.team_b[page_number];
			// Si no se mapeo, pedimos una pagina nueva y mapeamos alli
			if (page == 0) { page = mmu_next_free_user_page(); shared_map.team_b[page_number] = page; }
		}

		mmu_map_page(cr3, vaddr, page, MMU_USER_UNPRIVILEGED, MMU_READ_WRITE);
		return 0;

	} else {
		unallocate_current_task();

		return 1;
	}
}

void sched_init(void) {
	fill_tss_gdt_entries();
	// Creamos un Lemming para cada equipo
	// create_sched_lemming_task(TeamA);
	// create_sched_lemming_task(TeamA);
	// create_sched_lemming_task(TeamA);
	// create_sched_lemming_task(TeamA);
	// create_sched_lemming_task(TeamB);
	// create_sched_lemming_task(TeamB);
	// create_sched_lemming_task(TeamB);
	// create_sched_lemming_task(TeamB);

	// Inicializamos el estado de las tareas en el game_state
	game_state.last_task_state = (sched_task_state_t) { .team_a_idx = 0, .team_b_idx = 1, .team_executed = TeamB };
}

uint16_t sched_next_task(void) {
	int selector = IDLE_SELECTOR;
	int last_task_idx;

	// Si el debugger esta mostrandose, no hacemos nada
	if (game_state.debug_state.is_showing) {
		return selector;
	}

	// Seteamos el equipo de este request de tarea
	if (is_current_task_from_team_a()) { 
		last_task_idx = game_state.last_task_state.team_b_idx;
	} else {
		last_task_idx = game_state.last_task_state.team_a_idx;
	}	

	for (int i = last_task_idx + 2 ; i <= last_task_idx + MAX_LEMMINGS_TASKS; i+=2) {
		if (tasks[i % MAX_LEMMINGS_TASKS].allocated) {
			if (is_current_task_from_team_a()) { 
				game_state.last_task_state.team_b_idx = i % MAX_LEMMINGS_TASKS; 
			} else {
				game_state.last_task_state.team_a_idx = i % MAX_LEMMINGS_TASKS;
			}
			selector = tasks[i % MAX_LEMMINGS_TASKS].tss_selector;
			break;
		}
	}
	// Seteamos el equipo de este request de tarea
	if (is_current_task_from_team_a()) { 
		game_state.last_task_state.team_executed = TeamB;
	} else {
		game_state.last_task_state.team_executed = TeamA;
	}
	return selector;
}