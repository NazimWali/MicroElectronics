
#include "stm32f4xx.h"

void delayMs(int n);

int main(void) {
		//RCC->AHB1ENR |= 4;								//enable GPIOC clock
		RCC->AHB1ENR	= 0x5;
	
		GPIOC->MODER = 0x00000000;        //clear pin mode to output mode
		//GPIOC->PUPDR = 0x00005555;
	
		GPIOA->MODER = 0x55550000;        //clear pin mode to output mode
		GPIOA->PUPDR = 0x55550000;

		unsigned int nums[10] = {0x3F00,0x0600,0x5B00,0x4F00,0x6600,0x6D00,0x7D00,0x0700,0x7F00,0x6F00};
		unsigned int repeat = 2;
	
    while(1) {
				if((GPIOC->IDR & 0x2000)==0x2000){					//coutndown begins each time button is pressed
																										//once pressed, countdown must finish before going again
				}
				else{
						for(int i = 9; i > -1; i--){						//countdown from 9, starting from last entry in array
								delayMs(1000);
								GPIOA->ODR = nums[i];
								delayMs(1000);
							
								if(i == 0){													//once i reaches 0, output 0 another 2 times
										for(int j = repeat; j>0; j--){
												GPIOA->ODR = 0x0000;
												delayMs(1000);
												GPIOA->ODR = nums[i];
												delayMs(1000);
										}
										GPIOA-> ODR = 0x0000;
								}
								
						}
				}
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
