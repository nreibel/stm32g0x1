#include <stdint.h>

#include "interrupts.h"

extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _estack;
extern uint32_t _sidata;

/*
 * Vector table
 */

__attribute__((section(".vector_table")))
uint32_t vtable[48] = {
    (uint32_t) &_estack,
    (uint32_t) &reset_handler,
    0, // NMI
    (uint32_t) &hardfault_handler,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0, // Service Call
    0,
    0,
    0, // Pendable Service Call
    (uint32_t) &systick_handler,
    0, // Window WatchDog
    0, // PVD through EXTI Line detect
    0, // RTC through the EXTI line
    0, // FLASH
    0, // RCC
    0, // EXTI Line 0 and 1
    0, // EXTI Line 2 and 3
    (uint32_t) &exti_4_15_handler,
    0, // UCPD1, UCPD2
    0, // DMA1 Channel 1
    0, // DMA1 Channel 2 and Channel 3
    0, // DMA1 Channel 4 to Channel 7, DMAMUX1 overrun
    0, // ADC1, COMP1 and COMP2
    0, // TIM1 Break, Update, Trigger and Commutation
    0, // TIM1 Capture Compare
    0, // TIM2
    0, // TIM3
    0, // TIM6, DAC and LPTIM1
    0, // TIM7 and LPTIM2
    0, // TIM14
    0, // TIM15
    0, // TIM16
    0, // TIM17
    0, // I2C1
    0, // I2C2
    0, // SPI1
    0, // SPI2
    0, // USART1
    0, // USART2
    0, // USART3, USART4 and LPUART1
    0, // CEC
    0, // AES, RNG
};

/*
 * Boot code
 */

void init()
{
    // init .data
    for ( uint32_t *from = &_sidata, *to = &_sdata; to < &_edata; to++)
    {
        *to = *(from++);
    }

    // init .bss to zero
    for ( uint32_t *ptr = &_sbss; ptr < &_ebss ; ptr++)
    {
        *ptr = 0;
    }
}
