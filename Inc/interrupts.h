#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

void reset_handler();
void hardfault_handler();
void systick_handler();
void exti_4_15_handler();

#endif // __INTERRUPTS_H
