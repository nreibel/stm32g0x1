#ifndef __MAIN_H
#define __MAIN_H

#include "stdint.h"
#include "stdbool.h"
#include "stm32g071xx.h"

extern volatile uint32_t systick;

extern void port_rising_edge(unsigned int pin);
extern void port_falling_edge(unsigned int pin);

#define CPU_FREQ 16000000 // in Hz

#define UNUSED(x) (void) x

/*
 * Pin mapping
 */
#define LED_PORT GPIOA
#define LED_PIN 5

#define BTN_PORT GPIOC
#define BTN_PIN  13

#define USART_TX_PORT GPIOA
#define USART_TX_PIN  2

#define USART_RX_PORT GPIOA
#define USART_RX_PIN  3

/*
 * Bits manipulation
 */

#define IS_SET_BIT(a, b) (((a) & (1 << (b))) != 0)
#define RESET_MASK(a, m) (a &= ~(m))
#define SET_MASK(a, m)   (a |= (m))
#define SET_BIT(a, b)    (a |= (1 << (b)))
#define RESET_BIT(a, b)  (a &= ~(1 << (b)))


/*
 * GPIO
 */

#define GPIO_LL_MODE_INPUT     0x0
#define GPIO_LL_MODE_OUTPUT    0x1
#define GPIO_LL_MODE_ALTERNATE 0x2
#define GPIO_LL_MODE_ANALOG    0x3

#define GPIO_LL_PUPD_NONE      0x0
#define GPIO_LL_PUPD_PULLUP    0x1
#define GPIO_LL_PUPD_PULLDOWN  0x2

typedef enum {
    GPIO_MODE_INPUT_FLOATING,
    GPIO_MODE_INPUT_PULLUP,
    GPIO_MODE_INPUT_PULLDOWN,
    GPIO_MODE_OUTPUT_OPEN_DRAIN,
    GPIO_MODE_OUTPUT_PUSH_PULL,
    GPIO_MODE_ALTERNATE_OPEN_DRAIN,
    GPIO_MODE_ALTERNATE_PUSH_PULL,
    GPIO_MODE_ANALOG
} GPIO_Mode;

#endif // __MAIN_H
