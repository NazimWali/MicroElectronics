/* Revised Blinky Program to create a program for a 4-way traffic light
 */

#include "stm32f4xx.h"

void delayMs(int n);


int main(void) {
    RCC->AHB1ENR |=  1;             /* enable GPIOA clock */
    
    GPIOA->MODER &= ~0x00000FFF;    /* clear pin mode */
    GPIOA->MODER |=  0x00055555;    /* set pin to output mode */

    while(1) {
        GPIOA->ODR =  0x00000081;  /* EWR, NSG */
        delayMs(5000);
        
				GPIOA->ODR = 0x00000082;  /* EWR, NSY */
        delayMs(2000);
				
				GPIOA->ODR =  0x00000030;  /* EWG, NSR */
        delayMs(5000);
        
				GPIOA->ODR = 0x00000050;  /* EWY, NSR */
        delayMs(2000);
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