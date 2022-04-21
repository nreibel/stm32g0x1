#include "main.h"

volatile uint32_t systick = 0;

void delay(uint32_t ms)
{
    uint32_t beg = systick;
    while(systick - beg < ms);
}

void port_write(GPIO_TypeDef * port, uint8_t pin, bool value)
{
    SET_BIT(port->BSRR, value ? pin : pin + 16);
}

int port_configure(GPIO_TypeDef * port, uint8_t pin, GPIO_Mode mode)
{
    // Set mode
    switch(mode)
    {
        case GPIO_MODE_INPUT_FLOATING:
        case GPIO_MODE_INPUT_PULLUP:
        case GPIO_MODE_INPUT_PULLDOWN:
            RESET_MASK(port->MODER, 0x3 << (2*pin));
            SET_MASK(port->MODER, GPIO_LL_MODE_INPUT << (2*pin));
            break;

        case GPIO_MODE_OUTPUT_OPEN_DRAIN:
        case GPIO_MODE_OUTPUT_PUSH_PULL:
            RESET_MASK(port->MODER, 0x3 << (2*pin));
            SET_MASK(port->MODER, GPIO_LL_MODE_OUTPUT << (2*pin));
            break;

        case GPIO_MODE_ALTERNATE_OPEN_DRAIN:
        case GPIO_MODE_ALTERNATE_PUSH_PULL:
            RESET_MASK(port->MODER, 0x3 << (2*pin));
            SET_MASK(port->MODER, GPIO_LL_MODE_ALTERNATE << (2*pin));
            break;

        case GPIO_MODE_ANALOG:
            RESET_MASK(port->MODER, 0x3 << (2*pin));
            SET_MASK(port->MODER, GPIO_LL_MODE_ANALOG << (2*pin));
            break;

        default:
            return -1;
    }

    // Set output mode
    switch(mode)
    {
        case GPIO_MODE_ALTERNATE_OPEN_DRAIN:
        case GPIO_MODE_OUTPUT_OPEN_DRAIN:
            SET_BIT(port->OTYPER, pin);
            break;

        case GPIO_MODE_ALTERNATE_PUSH_PULL:
        case GPIO_MODE_OUTPUT_PUSH_PULL:
            RESET_BIT(port->OTYPER, pin);
            break;

        default:
            // Do nothing
            break;
    }

    // Set input pullup/pulldown
    switch(mode)
    {
        case GPIO_MODE_INPUT_FLOATING:
            RESET_MASK(port->PUPDR, 0x3 << (2*pin));
            SET_MASK(port->PUPDR, GPIO_LL_PUPD_NONE << (2*pin));
            break;

        case GPIO_MODE_INPUT_PULLUP:
            RESET_MASK(port->PUPDR, 0x3 << (2*pin));
            SET_MASK(port->PUPDR, GPIO_LL_PUPD_PULLUP << (2*pin));
            break;

        case GPIO_MODE_INPUT_PULLDOWN:
            RESET_MASK(port->PUPDR, 0x3 << (2*pin));
            SET_MASK(port->PUPDR, GPIO_LL_PUPD_PULLDOWN << (2*pin));
            break;

        default:
            break;
    }

    return 0;
}

void port_set_alternate_mode(GPIO_TypeDef * port, uint8_t pin, uint8_t mode)
{
    // Select AFR register
    // AFR0 => AFSEL0..7
    // AFR1 => AFSEL8..15
    uint8_t idx = pin / 8;

    // Select AFR offset
    // AFRx[0..3]   => AFSEL0/9
    // AFRx[4..7]   => AFSEL1/9
    // AFRx[8..11]  => AFSEL2/10
    // AFRx[12..15] => AFSEL3/11
    // AFRx[16..19] => AFSEL4/12
    // AFRx[20..25] => AFSEL5/13
    // AFRx[26..27] => AFSEL6/14
    // AFRx[28..31] => AFSEL7/15
    uint8_t off = 4 * (pin % 8);

    RESET_MASK(port->AFR[idx], 0xF << off);
    SET_MASK(port->AFR[idx], mode << off);
}

void nvic_enable_int(IRQn_Type irq)
{
    SET_BIT(NVIC->ISER[0], irq);
}

int port_enable_extint(GPIO_TypeDef * port, uint8_t pin)
{
    uint32_t tmp = 0;
    uint8_t regval = 0;

    if (port == GPIOA) regval = 0x0;
    else if (port == GPIOB) regval = 0x1;
    else if (port == GPIOC) regval = 0x2;
    else if (port == GPIOD) regval = 0x3;
    else if (port == GPIOF) regval = 0x5;
    else return -1;

    // Select EXTICRx register
    // EXTICR0 => EXTI0..3
    // EXTICR1 => EXTI4..7
    // EXTICR2 => EXTI8..11
    // EXTICR3 => EXTI12..15
    uint8_t idx = pin/4;

    // Offset inside of EXTICRx register
    // EXTICRx[0..7]   => EXTI0/4/8/12
    // EXTICRx[8..15]  => EXTI1/5/9/13
    // EXTICRx[16..23] => EXTI2/6/10/14
    // EXTICRx[24..31] => EXTI3/7/11/15
    uint8_t off = 8 * (pin % 4);

    // Set EXTI Mux to desired GPIO Port
    tmp = EXTI->EXTICR[idx];
    RESET_MASK(tmp, 0x07 << off);
    SET_MASK(tmp, regval << off);
    EXTI->EXTICR[idx] = tmp;

    // Enable rising+falling edge triggers
    SET_BIT(EXTI->RTSR1, pin);
    SET_BIT(EXTI->FTSR1, pin);

    // Enable event
    // SET_BIT(EXTI->EMR1, pin);

    // Enable interrupt
    SET_BIT(EXTI->IMR1, pin);

    if (pin < 2)       nvic_enable_int(EXTI0_1_IRQn);
    else if (pin < 4)  nvic_enable_int(EXTI2_3_IRQn);
    else if (pin < 16) nvic_enable_int(EXTI4_15_IRQn);
    else return -1;

    return 0;
}

void port_rising_edge(unsigned int pin)
{
    port_write(LED_PORT, LED_PIN, false);
}

void port_falling_edge(unsigned int pin)
{
    port_write(LED_PORT, LED_PIN, true);
}

void systick_enable(uint32_t ms)
{
    // Disable timer
    SysTick->CTRL = 0x0;

    // Compute reset value
    SysTick->LOAD = CPU_FREQ / (ms * 1000);

    // Write 0 to reset value
    SysTick->VAL = 0;

    // Use CPU clock, Enable IRQ, Enable timer
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

void usart_init(USART_TypeDef * usart, unsigned int baudrate)
{
    usart->CR1 = 0;
    usart->BRR = CPU_FREQ/baudrate;
    usart->CR1 |= ( USART_CR1_RE | USART_CR1_TE | USART_CR1_UE );
}

bool usart_ready(USART_TypeDef * usart)
{
    return usart->ISR & USART_ISR_TC ? true : false;
}

void usart_write_string(USART_TypeDef * usart, char * string)
{
    while(*string != 0)
    {
        while( !usart_ready(usart) );
        usart->TDR = *(string++);
    }
}

int main(void)
{
    // Enable system peripherals
    RCC->APBENR2 |= RCC_APBENR2_SYSCFGEN;
    RCC->APBENR1 |= RCC_APBENR1_PWREN;

    // Enable ports A and C
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    RCC->IOPENR |= RCC_IOPENR_GPIOCEN;

    // Enable USART2
    RCC->APBENR1 |= RCC_APBENR1_USART2EN;

    // 1ms tick
    systick_enable(1);

    // Init TX, TX
    port_configure(USART_TX_PORT, USART_TX_PIN, GPIO_MODE_ALTERNATE_PUSH_PULL);
    port_configure(USART_RX_PORT, USART_RX_PIN, GPIO_MODE_ALTERNATE_PUSH_PULL);

    // Set USART2 alternate mode for TX and RX
    port_set_alternate_mode(USART_TX_PORT, USART_TX_PIN, 0x01);
    port_set_alternate_mode(USART_RX_PORT, USART_RX_PIN, 0x01);

    // Init LED and BTN
    port_configure(LED_PORT, LED_PIN, GPIO_MODE_OUTPUT_PUSH_PULL);
    port_configure(BTN_PORT, BTN_PIN, GPIO_MODE_INPUT_PULLUP);
    port_enable_extint(BTN_PORT, BTN_PIN);

    // Init USART2
    usart_init(USART2, 9600);

    for(;;)
    {
        // Heartbeat pattern
        port_write(LED_PORT, LED_PIN, true);
        delay(50);
        port_write(LED_PORT, LED_PIN, false);
        delay(100);
        port_write(LED_PORT, LED_PIN, true);
        delay(50);
        port_write(LED_PORT, LED_PIN, false);
        delay(800);

        usart_write_string(USART2, "hello, world!\r\n");
    }
}
