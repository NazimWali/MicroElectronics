/* Program to Create a Rural Traffic Light
   Bryce Manz and Nazim Wali
   DUE: October 21 2020
 */

#include "stm32f4xx.h"

void delayMs(int n);


int main(void) {
    RCC->AHB1ENR |=  0x3;  							/* enable GPIOA&B clock */
    
    GPIOB->MODER =  0x00000000;  								/* clear pin mode */
    GPIOB->MODER =  0x00150015;    							/* set pin to output mode */
	
		GPIOA->MODER = 0x0;
		GPIOA->PUPDR =  0x00000005;                 /* set pull-up resistors */

    while(1) {
        GPIOB->ODR =  0x401; 							/* EWR, NSG */
        delayMs(5000);	
        
			while(GPIOA->IDR &= 0x00000002)	
			{                            							/*isolate PA9, when PA9=1, do nothing*/
			}  												   							/* when PA9 is not equal to 1, moves to yellow light*/
						
				GPIOB->ODR = 0x402; 						  	/* EWR, NSY */
        delayMs(2000);
				
				GPIOB->ODR =  0x104;  							/* EWG, NSR */
        delayMs(5000);
        
			while(GPIOA->IDR &= 0x00000001)						/*isolate PA8, when PA8=1, do nothing*/
			{																					/* when PA8 is not equal to 1, moves to yellow light*/
			}
			
				GPIOB->ODR = 0x204;  							/* EWY, NSR */
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

