; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================
;
; Definicion de rutinas de atencion de interrupciones

%include "print.mac"

BITS 32

sched_task_offset:     dd 0xFFFFFFFF
sched_task_selector:   dw 0xFFFF
syscall_returnable:    dd 0x00000000

;; PIC
extern pic_finish1

;; Sched
extern sched_next_task
extern unallocate_current_task
extern sched_map_shared

;; Print
extern print_interruption
extern print_dec_input
extern print_current_task_clock
extern print_debug

; Game
extern save_game_state_error_code
extern save_game_state_without_err_code
extern notify_debug_key_pressed
extern game_tick
extern execute_move
extern execute_bridge
extern explode_lemming
extern get_last_movement
;;
;; Definición de MACROS
;; -------------------------------------------------------------------------- ;;
%macro ISR 1
global _isr%1

_isr%1:
	
	pushad

	push %1
	; call print_interruption
	; Guardamos el estado para el debug
    call save_game_state_without_err_code
    add esp, 4

    ; Mostramos el debug si es necesario
    call print_debug

    ; Desalojamos a la tarea que causo el error
    call unallocate_current_task

    ; Buscamos la proxima tarea a ejecutar
    call sched_next_task

    ; Saltamos a la tarea
    mov [sched_task_selector], ax
    jmp far [sched_task_offset]

    .fin:

	popad
	iret

%endmacro

%macro ISR_ERROR_CODE 1
global _isr%1

_isr%1:

    pushad

    push %1
    ; call print_interruption
    ; Guardamos el estado para el debug
    call save_game_state_error_code
    add esp, 4

    ; Mostramos el debug si es necesario
    call print_debug

    ; Desalojamos a la tarea que causo el error
    call unallocate_current_task

    ; Buscamos la proxima tarea a ejecutar
    call sched_next_task

    ; Saltamos a la tarea
    mov [sched_task_selector], ax
    jmp far [sched_task_offset]

    .fin:
    popad
    iret

%endmacro

global _isr14

_isr14:    
    pushad

    push 14
    ; call print_interruption
    ; Guardamos el estado para el debug
    call save_game_state_error_code
    add esp, 4

    ; Mostramos el debug si es necesario
    call print_debug

    mov eax, cr3
    mov ebx, cr2
    push eax
    push ebx
    call sched_map_shared
    add esp, 8

    cmp eax, 1
    jne .fin

    .bad_access:
    ; Buscamos la proxima tarea a ejecutar
    call sched_next_task

    ; Saltamos a la tarea
    mov [sched_task_selector], ax
    jmp far [sched_task_offset]

    .fin:
    popad

    ; Eliminamos el error code
    add esp, 4
    iret


;; Rutina de atención de las EXCEPCIONES
;; -------------------------------------------------------------------------- ;;
ISR 0
ISR 2
ISR 3
ISR 4
ISR 5
ISR 6
ISR 7
ISR_ERROR_CODE 8
ISR 9
ISR_ERROR_CODE 10
ISR_ERROR_CODE 11
ISR_ERROR_CODE 12
ISR_ERROR_CODE 13
; Definimos la 14 a mano
ISR 16
ISR_ERROR_CODE 17
ISR 18
ISR 19

;; Rutina de atención del RELOJ
;; -------------------------------------------------------------------------- ;;
global _isr32
_isr32:
    pushad

    ; Atendemos la interrupcion
    call pic_finish1

    ; Printeamos el reloj
    call next_clock

    ; Avanzamos el tick del juego
    call game_tick

    ; Obtennemos el selector de la siguiente task
    call sched_next_task
    
    ; Obtenemos el selector de la task actual y comparamos si la siguiente es distinta
    str cx
    cmp ax, cx
    je .fin

    ; Guardamos el selector para el jump
    mov [sched_task_selector], ax
    
    ; Printeamos el clock de la tarea a ejecutar
    call print_current_task_clock
    
    jmp far [sched_task_offset]
    
    .fin:
    popad
    iret

;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;
global _isr33
_isr33:
    pushad

    in al, 0x60

    ; Printeamos si corresponde el input
    push eax
    ;call print_dec_input ;descomentar si se quiere ver las interrupciones de numeros
    call notify_debug_key_pressed
    add esp, 4

    ; Atendemos la interrupcion
    call pic_finish1

    popad     
    iret

;; Rutinas de atención de las SYSCALLS
;; -------------------------------------------------------------------------- ;;
global _isr88
_isr88:

    pushad
    push eax
    call execute_move
    pop  ebx

    ; Ponemos la IDLE
    mov WORD [sched_task_selector], 0x78
    jmp far [sched_task_offset]

    popad

    push eax
    call get_last_movement
    add esp, 4

    iret

global _isr98
_isr98:
    pushad

	; Destruye las paredes de ladrillo 
	; y los lemmings alrededor del mismo
	call explode_lemming

    ; Desalojamos a la tarea que causo el error
    call unallocate_current_task

    ; Buscamos la proxima tarea a ejecutar
    call sched_next_task

    ; Saltamos a la tarea
    mov [sched_task_selector], ax
    jmp far [sched_task_offset]

    popad     
    iret

global _isr108
_isr108:
    pushad

    push eax
    call execute_bridge
    pop  ebx

    ; Desalojamos a la tarea que causo el error
    call unallocate_current_task

    ; Ponemos la IDLE
    mov WORD [sched_task_selector], 0x78
    jmp far [sched_task_offset]

    popad     
    iret

;; Funciones Auxiliares
;; -------------------------------------------------------------------------- ;;
isrNumber:           dd 0x00000000
isrClock:            db '|/-\'
next_clock:
        pushad
        inc DWORD [isrNumber]
        mov ebx, [isrNumber]
        cmp ebx, 0x4
        jl .ok
                mov DWORD [isrNumber], 0x0
                mov ebx, 0
        .ok:
                add ebx, isrClock
                print_text_pm ebx, 1, 0x0f, 49, 79
                popad
        ret
