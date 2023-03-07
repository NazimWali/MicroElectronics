			
			
			EXPORT __Vectors
			EXPORT Reset_Handler
			AREA vectors, CODE, READONLY
				
__Vectors 	DCD 0x1001000 ; 0x20008000 ; Top of Stack
			DCD Reset_Handler 			; Reset Handler
				
RCC_AHB1ENR equ 0x40023830
GPIOA_MODER equ 0x40020000
GPIOA_ODR 	equ 0x40020014
	
			AREA PROG, CODE,			READONLY
Reset_Handler
			ldr		r4, = RCC_AHB1ENR		;enable GPIOA clock
			ldr 	r5, [r4]
			orr 	r5, #1
			str 	r5, [r4]
			
			/*
			
			ldr 	r4, =GPIOA_MODER 		;set pin to output mode
			mov 	r5, #0
			str 	r5, [r4]
			ldr 	r5, = 0x00005555
			str		r5, [r4]
			
			*/
			
			
			ldr r4, = GPIOA_MODER 			; set pin to output mode
			mov r5, #0
			
			str r5, [r4]
			ldr r5, = 0x00000FFF
			str r5, [r4]

L1			ldr r4, =GPIOA_ODR
			ldr r5, = 0x00000081			; EWR on, NSG on
			str r5, [r4]
			mov r0, #5000
			b1	delay
			
			ldr r4, =GPIOA_ODR
			ldr r5, = 0x00000082			; EWR on, NSY on
			str r5, [r4]
			mov r0, #2000
			b1	delay
			
			ldr r4, =GPIOA_ODR
			ldr r5, = 0x00000030			; EWR on, NSY on
			str r5, [r4]
			mov r0, #5000
			b1	delay
			
			ldr r4, =GPIOA_ODR
			ldr r5, = 0x00000050			; EWR on, NSY on
			str r5, [r4]
			mov r0, #2000
			b1	delay
			b	L1							; loop forever
			
delay 		ldr 	r1, = 5325 					; delay milleseconds in R0
DL1			subs 	r1, r1, #1
			bne 	DL1
			subs 	r0,r0, #1
			bne 	delay
			bx 		lr
			
			end
			