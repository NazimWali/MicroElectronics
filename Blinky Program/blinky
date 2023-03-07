/* p2_1.c Toggle Green LED (LD2) on STM32F446RE Nucleo64 board at 1 Hz
 *
 * This program toggles LD2 for 0.5 second ON and 0.5 second OFF
 * by writing 0 or 1 to bit 5 of the Port A Output Data Register.
 * The green LED (LD2) is connected to PA5.
 * The LED is high active (a '1' turns on the LED).
 * The default system clock is running at 16 MHz.
 *
 * This program was tested with Keil uVision v5.31
 *
 * Revision A: Exchanged original delayMs (software down count based delay)
 * with on-board timer (SysTick) on 09/09/2020.
 */

#include "stm32f4xx.h"

void delayMs(int n);

int main(void) {
    RCC->AHB1ENR |=  1;             /* enable GPIOA clock */
    
    GPIOA->MODER &= ~0x00000C00;    /* clear pin mode */
    GPIOA->MODER |=  0x00000400;    /* set pin to output mode */

    while(1) {
        GPIOA->ODR |=  0x00000020;  /* turn on LED */
        delayMs(1000);
        GPIOA->ODR &= ~0x00000020;  /* turn off LED */
        delayMs(1000);
    }
}

/* 16 MHz SYSCLK */
void delayMs(int n) {
    int i;

    /* Configure SysTick */
    SysTick->LOAD = 16000;  /* reload with number of clocks per millisecond */
    SysTick->VAL = 0;       /* clear current value register */
    SysTick->CTRL = 0x5;    /* Enable the timer */

    for(i = 0; i < n; i++) {
        while((SysTick->CTRL & 0x10000) == 0) /* wait until the COUNTFLAG is set */
            { }
    }
    SysTick->CTRL = 0;      /* Stop the timer (Enable = 0) */
}