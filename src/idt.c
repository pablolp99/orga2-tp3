/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de las rutinas de atencion de interrupciones
*/

#include "idt.h"
#include "defines.h"
#include "game.h"
#include "i386.h"
#include "isr.h"
#include "screen.h"

idt_entry_t idt[255] = {0};

idt_descriptor_t IDT_DESC = {sizeof(idt) - 1, (uint32_t)&idt};

#define IDT_ENTRY(numero, dpl_attr) \
    idt[numero].offset_15_0 = (uint16_t) ((uint32_t)(&_isr ## numero) & (uint32_t) 0xFFFF); \
    idt[numero].segsel = (uint16_t) CODE_SELECTOR_0; \
    idt[numero].attr = (uint16_t) dpl_attr; \
	idt[numero].offset_31_16 = (uint16_t) ((uint32_t)(&_isr ## numero) >> 16 & (uint32_t) 0xFFFF);


void idt_init() {
	IDT_ENTRY(0, KERNEL_LEVEL_INTERRUPTION);
	IDT_ENTRY(2, KERNEL_LEVEL_INTERRUPTION);
	IDT_ENTRY(3, KERNEL_LEVEL_INTERRUPTION);
	IDT_ENTRY(4, KERNEL_LEVEL_INTERRUPTION);
	IDT_ENTRY(5, KERNEL_LEVEL_INTERRUPTION);
	IDT_ENTRY(6, KERNEL_LEVEL_INTERRUPTION);
	IDT_ENTRY(7, KERNEL_LEVEL_INTERRUPTION);
	IDT_ENTRY(8, KERNEL_LEVEL_INTERRUPTION);
	IDT_ENTRY(9, KERNEL_LEVEL_INTERRUPTION);
	IDT_ENTRY(10, KERNEL_LEVEL_INTERRUPTION);
	IDT_ENTRY(11, KERNEL_LEVEL_INTERRUPTION);
	IDT_ENTRY(12, KERNEL_LEVEL_INTERRUPTION);
	IDT_ENTRY(13, KERNEL_LEVEL_INTERRUPTION);
	IDT_ENTRY(14, KERNEL_LEVEL_INTERRUPTION);
	IDT_ENTRY(16, KERNEL_LEVEL_INTERRUPTION);
	IDT_ENTRY(17, KERNEL_LEVEL_INTERRUPTION);
	IDT_ENTRY(18, KERNEL_LEVEL_INTERRUPTION);
	IDT_ENTRY(19, KERNEL_LEVEL_INTERRUPTION);
	IDT_ENTRY(32, KERNEL_LEVEL_INTERRUPTION);
	IDT_ENTRY(33, KERNEL_LEVEL_INTERRUPTION);

	IDT_ENTRY(88, USER_LEVEL_INTERRUPTION);
	IDT_ENTRY(98, USER_LEVEL_INTERRUPTION);
	IDT_ENTRY(108, USER_LEVEL_INTERRUPTION);
	}

