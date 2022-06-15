; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================

%include "print.mac"

global start

extern GDT_DESC
extern IDT_DESC

;; IDT
extern idt_init

;; Pic
extern pic_reset
extern pic_enable

;; MMU
extern mmu_init
extern mmu_init_kernel_dir
extern mmu_init_task_dir

;; Screen
extern print_libretas
extern print_screen_data

;; TSS
extern tss_init

;; Game
extern game_init

;; Scheduler
extern sched_init

%define CODE_SELECTOR_0 0x40
%define DATA_SELECTOR_0 0x50
%define VIDEO_SELECTOR_0 0x60

%define STACK_BASE 0x25000

%define COLOR_BLACK 0x0000
%define COLOR_GREEN 0xA000
%define COLOR_RED   0x4F00
%define COLOR_BLUE  0x1F00

%define TSS_INIT_SELECTOR 0x70
%define TSS_IDLE_SELECTOR 0x78

BITS 16
;; Saltear seccion de datos
jmp start

;;
;; Seccion de datos.
;; -------------------------------------------------------------------------- ;;
start_rm_msg db     'Iniciando kernel en Modo Real'
start_rm_len equ    $ - start_rm_msg

start_pm_msg db     'Iniciando kernel en Modo Protegido'
start_pm_len equ    $ - start_pm_msg

;;
;; Seccion de código.
;; -------------------------------------------------------------------------- ;;

;; Punto de entrada del kernel.
BITS 16
start:
	; Deshabilitar interrupciones
	cli

	; Cambiar modo de video a 80 X 50
	mov ax, 0003h
	int 10h ; set mode 03h
	xor bx, bx
	mov ax, 1112h
	int 10h ; load 8x8 font

	; Imprimir mensaje de bienvenida
	print_text_rm start_rm_msg, start_rm_len, 0x07, 0, 0

	; Habilitar A20
	call A20_disable
	call A20_check
	call A20_enable
	call A20_check
	
	; Cargar la GDT
	lgdt [GDT_DESC]

	; Setear el bit PE del registro CR0
	mov eax, cr0
	or eax, 1
	mov cr0, eax
	
	; Saltar a modo protegido
	jmp CODE_SELECTOR_0:protected

BITS 32
protected:
	; Establecer selectores de segmentos
	mov ax, DATA_SELECTOR_0
	mov ds, ax
	mov es, ax
	mov gs, ax
	mov fs, ax
	mov ss, ax

	; Establecer la base de la pila
	mov ebp, STACK_BASE
	mov esp, STACK_BASE

	; Imprimir mensaje de bienvenida
	print_text_pm start_pm_msg, start_pm_len, 0x07, 5, 0

	; Inicializar pantalla
	mov ax, VIDEO_SELECTOR_0
	mov es, ax

	call init_screen

	; Inicializar el manejador de memoria
	call mmu_init 

	; Inicializar el directorio de paginas
	call mmu_init_kernel_dir

	; Cargar directorio de paginas
	mov cr3, eax

	; Habilitar paginacion
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax

	; Pruebas, si se quiere descomentar
	;push DWORD 0x600000
	;call mmu_init_task_dir
	;pop ebx
	;mov cr3, eax

	;call print_libretas
	
	; Inicializar tss
	call tss_init	

	; Inicializar el juego
	call game_init
	; Inicializar el scheduler
	call sched_init
	; Printeamos los valores iniciales de la pantalla
	call print_screen_data

	; Inicializar la IDT
	call idt_init	

	; Cargar IDT
	lidt [IDT_DESC] 
	
	; Configurar controlador de interrupciones
	call pic_reset
	call pic_enable

	; Cargar tarea inicial
	mov ax, TSS_INIT_SELECTOR
	ltr ax

	; Habilitar interrupciones
	sti

	; Saltar a la primera tarea: Idle
	jmp TSS_IDLE_SELECTOR:0x0
	;xchg bx, bx
	;jmp 0x80:0x0

	; Ciclar infinitamente (por si algo sale mal...)
	; mov eax, 0xFFFF
	; mov ebx, 0xFFFF
	; mov ecx, 0xFFFF
	; mov edx, 0xFFFF
	jmp $

;; -------------------------------------------------------------------------- ;;

init_screen:
	pushad

	; Pintamos primera fila de color negro
	mov ecx, 80
	mov eax, 0

	.loop_first_column:
		mov WORD [es:eax], COLOR_BLACK 
		add eax, 2
		dec ecx
		cmp ecx, 0
		jg .loop_first_column


	; Pintamos el area principal del juego
	mov ecx, 3200
	; (el eax se mantiene)
	.loop_main_screen:
		mov WORD [es:eax], COLOR_GREEN
		add eax, 2
		dec ecx
		cmp ecx, 0
		jg .loop_main_screen

	; Pintamos el area de puntajes principal
	mov ecx, 720
	.loop_score_area:
		mov WORD [es:eax], COLOR_BLACK
		add eax, 2
		dec ecx
		cmp ecx, 0
		jg .loop_score_area

	; Pintamos los recuadros de puntaje de los jugadores
	; (10x3)
	; Rojo X:10 Y:45
	; Azul X:63 Y:45
	mov ecx, 30
	mov eax, 7060 ; Red
	mov ebx, 7166 ; Blue
	mov edx, 0

	.loop_player_score_area:
		mov WORD [es:eax], COLOR_RED
		mov WORD [es:ebx], COLOR_BLUE

		add eax, 2
		add ebx, 2
		inc edx

		cmp edx, 10
		jne .continue
		mov edx, 0
		add eax, 140
		add ebx, 140
		
		.continue:
		dec ecx
		cmp ecx, 0
		jg .loop_player_score_area

	popad
	ret

;; -------------------------------------------------------------------------- ;;

%include "a20.asm"
