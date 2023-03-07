#include "stm32f4xx.h"

unsigned int num1 = 0, num2 = 0, result = 0;
int temp = 0;

int main(void) {
    
		RCC->AHB1ENR |=  2;									// enable GPIOB clock
		RCC->AHB1ENR |=  4;                 /* enable GPIOC clock */
			
		GPIOB->MODER = 0x00000000;					//clear pin modes
		GPIOC->MODER = 0x00000000;
	
		GPIOB->MODER = 0x00000000;					//set pins to input mode
		GPIOC->MODER = 0x00005555;					//set pins to input mode
	
		while(1) {
				if((GPIOC->IDR & 0x2000)==0x2000){
					
				}
				else{
						num1 = GPIOC->IDR &0x0000000F;			//read and save first number in num1, masking all others
						num2 = GPIOC->IDR &0x000000F0;			//read and save second number in num2, masking all others
						num2 = num2 >> 4;										//shift second number by 4 bits to the right to 
																								//allow the functions to operate on them
				}

				if ((GPIOB->IDR & 0x03)==0x00){					//If function is 00
						result = num1 + num2;								//Add the two numbers
				}
				else if((GPIOB->IDR & 0x03)==0x01){			//If function is 01
						temp = num1 - num2;									//Subtract the two numbers
					
						if(temp<0){
								temp = ~temp+1;									//2's complement if negative
						}
						result = temp;
				}
				else if((GPIOB->IDR & 0x03)==0x02){			//If function is 10
						result = num1 & num2;								//AND the two numbers
				}
				else{																		//If function is 11
						result = num1 | num2;								//OR the two numbers
				}
				
				GPIOA->ODR = result << 4;								//Shift reult by 4 bits to the left to
																								//align the bits with the pin outputs	
		}
}
