#include "interrupts.h"
#include "main.h"

void hardfault_handler()
{
    for(;;){}
}

void systick_handler()
{
    systick++;
}

void exti_4_15_handler()
{
    for (unsigned int i = 4 ; i < 15 ; i++)
    {
        // Rising edge
        if ( IS_SET_BIT(EXTI->RPR1, i) )
        {
            SET_BIT(EXTI->RPR1, i);
            port_rising_edge(i);
        }

        // Falling edge
        if ( IS_SET_BIT(EXTI->FPR1, i) )
        {
            SET_BIT(EXTI->FPR1, i);
            port_falling_edge(i);
        }
    }
}
