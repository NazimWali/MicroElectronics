/* p3_2.c:  Initialize and display "Hello" on the LCD using 8-bit data mode.
 *
 * Data pins use Port C; control pins use Port B.
 * Polling of the busy bit of the LCD status register is used for timing.
 *
 * The LCD controller is connected to the Nucleo-F446RE
 * board as follows:
 *
 * PC0-PC7 for LCD D0-D7, respectively.
 * PB5 for LCD R/S
 * PB6 for LCD R/W
 * PB7 for LCD EN
 *
 * This program was tested with Keil uVision v5.24a with DFP v2.11.0
 */

#include "stm32f4xx.h"

#define RS 0x20     /* PB5 mask for reg select */
#define RW 0x40     /* PB6 mask for read/write */
#define EN 0x80     /* PB7 mask for enable */

void delayMs(int n);
void LCD_command(unsigned char command);
void LCD_command_noPoll(unsigned char command);
void LCD_data(char data);
char keypad_getkey(void);
void LCD_init(void);
void LCD_ready(void);
void PORTS_init(void);
void keypad_init(void);
void delay(void);

int main(void) {
    /* initialize LCD controller */
    LCD_init();
		keypad_init();
	
		unsigned char nums[17] = {'0','1','2','3','4','5','6','7','8','9','10','11','12','13','14','15','16'};

    while(1) {
        
				unsigned char key;
				key = keypad_getkey();
				
        LCD_data(nums[key]);
//        LCD_data('a');
//        LCD_data('z');
//        LCD_data('i');
//        LCD_data('m');
        delayMs(500);

        /* clear LCD display */
        LCD_command(1);
        delayMs(500);
    }
}

/* Initialize port pins then initialize LCD controller */
void LCD_init(void) {
    PORTS_init();

    delayMs(30);            /* initialization sequence */
    LCD_command_noPoll(0x30);	/* LCD does not respond to status poll yet */
    delayMs(10);
    LCD_command_noPoll(0x30);
    delayMs(1);
    LCD_command_noPoll(0x30);   /* busy flag cannot be polled before this */

    LCD_command(0x38);      /* set 8-bit data, 2-line, 5x7 font */
    LCD_command(0x06);      /* move cursor right after each char */
    LCD_command(0x01);      /* clear screen, move cursor to home */
    LCD_command(0x0F);      /* turn on display, cursor blinking */
}

void PORTS_init(void) {
    RCC->AHB1ENR |=  0x06;          /* enable GPIOB/C clock */

    /* PB5 for LCD R/S */
    /* PB6 for LCD R/W */
    /* PB7 for LCD EN */
    GPIOB->MODER &= ~0x0000FC00;    /* clear pin mode */
    GPIOB->MODER |=  0x00005400;    /* set pin output mode */
    GPIOB->BSRR = 0x00C00000;       /* turn off EN and R/W */

    /* PC0-PC7 for LCD D0-D7, respectively. */
    GPIOC->MODER &= ~0x0000FFFF;    /* clear pin mode */
    GPIOC->MODER |=  0x00005555;    /* set pin output mode */
}

/* This function waits until LCD controller is ready to
 * accept a new command/data before returns.
 * It polls the busy bit of the status register of LCD controller.
 * In order to read the status register, the data port of the
 * microcontroller has to change to an input port before reading
 * the LCD. The data port of the microcontroller is return to
 * output port before the end of this function.
 */
void LCD_ready(void) {
    char status;

    /* change to read configuration to poll the status register */
    GPIOC->MODER &= ~0x0000FFFF;    /* clear pin mode */
    GPIOB->BSRR = RS << 16;         /* RS = 0 for status register */
    GPIOB->BSRR = RW;               /* R/W = 1 for read */

    do {    /* stay in the loop until it is not busy */
        GPIOB->BSRR = EN;           /* pulse E high */
        delayMs(0);
        status = GPIOC->IDR;        /* read status register */
        GPIOB->BSRR = EN << 16;     /* clear E */
        delayMs(0);
    } while (status & 0x80);        /* check busy bit */

    /* return to default write configuration */
    GPIOB->BSRR = RW << 16;         /* R/W = 0, LCD input */
    GPIOC->MODER |=  0x00005555;    /* Port C as output */
}

void LCD_command(unsigned char command) {
    LCD_ready();            /* wait for LCD controller ready */
    GPIOB->BSRR = (RS | RW) << 16;  /* RS = 0, R/W = 0 */
    GPIOC->ODR = command;           /* put command on data bus */
    GPIOB->BSRR = EN;               /* pulse E high */
    delayMs(0);
    GPIOB->BSRR = EN << 16;         /* clear E */
}

/* This function is used at the beginning of the initialization
 * when the busy bit of the status register is not readable.
 */
void LCD_command_noPoll(unsigned char command) {
    GPIOB->BSRR = (RS | RW) << 16;  /* RS = 0, R/W = 0 */
    GPIOC->ODR = command;           /* put command on data bus */
    GPIOB->BSRR = EN;               /* pulse E high */
    delayMs(0);
    GPIOB->BSRR = EN << 16;         /* clear E */
}

void LCD_data(char data) {
    LCD_ready();            /* wait for LCD controller ready */
    GPIOB->BSRR = RS;               /* RS = 1 */
    GPIOB->BSRR = RW << 16;         /* R/W = 0 */
    GPIOC->ODR = data;              /* put data on data bus */
    GPIOB->BSRR = EN;               /* pulse E high */
    delayMs(0);
    GPIOB->BSRR = EN << 16;         /* clear E */
}

void keypad_init(void) {
    //RCC->AHB1ENR |=  0x14;	        /* enable GPIOC clock */
		RCC->AHB1ENR |=  1;                	/* enable GPIOA clock */
   
		GPIOA->MODER =   0x00000000;    /* clear pin mode to input */
    GPIOA->PUPDR =   0x00005500;    /* enable pull up resistors for column pins */
}


/* make a small delay */
void delay(void) {
    int i;
    for (i = 0; i < 20; i++) ;
}

/*
 * This is a non-blocking function to read the keypad.
 * If a key is pressed, it returns a unique code for the key.
 * Otherwise, a zero is returned.
 * PC6-9 are used as input and connected to the columns. Pull-up resistors are
 * enabled so when the keys are not pressed, these pins are pulled high.
 * PC4-7 are used as output that drives the keypad rows.
 * First, all rows are driven low and the input pins are read. If no key is
 * pressed, they will read as all one because of the pull up resistors.
 * If they are not all one, some key is pressed.
 * If some key is pressed, the program proceeds to drive only one row low at
 * a time and leave the rest of the rows inactive (float) then read the input pins.
 * Knowing which row is active and which column is active, the program can decide
 * which key is pressed.
 *
 * Only one row is driven so that if multiple keys are pressed and row pins are
 * shorted, the microcontroller will not be damaged. When the row is being
 * deactivated, it is driven high first otherwise the stray capacitance may keep
 * the inactive row low for some time.
 */
char keypad_getkey(void) {
    int row, col;
    const int row_mode[] = {0x00000100, 0x00000400, 0x00001000, 0x00004000}; /* one row is output */
    const int row_low[] =  {0x00100000, 0x00200000, 0x00400000, 0x00800000}; /* one row is low */
    const int row_high[] = {0x00000010, 0x00000020, 0x00000040, 0x00000080}; /* one row is high */

    /* check to see any key pressed */
    GPIOA->MODER = 0x00005500;      /* make all row pins output */
    GPIOA->BSRR =  0x00F00000;      /* drive all row pins low */
    delay();                        /* wait for signals to settle */
    col = GPIOA->IDR & 0x000F;      /* read all column pins */
    GPIOA->MODER &= ~0x0000FF00;    /* disable all row pins drive */
    if (col == 0x000F)              /* if all columns are high */
        return 0;                       /* no key pressed */

    /* If a key is pressed, it gets here to find out which key.
     * It activates one row at a time and read the input to see
     * which column is active. */
    for (row = 0; row < 4; row++) {
        GPIOA->MODER &= ~0x0000FF00;    /* disable all row pins drive */
        GPIOA->MODER |= row_mode[row];  /* enable one row at a time */
        GPIOA->BSRR = row_low[row];     /* drive the active row low */
        delay();                        /* wait for signal to settle */
        col = GPIOA->IDR & 0x000F;      /* read all columns */
        GPIOA->BSRR = row_high[row];    /* drive the active row high */
        if (col != 0x000F) break;       /* if one of the input is low, some key is pressed. */
    }
    GPIOA->BSRR = 0x000000F0;           /* drive all rows high before disable them */
    GPIOA->MODER &= ~0x0000FF00;        /* disable all rows */
    if (row == 4)
        return 0;                       /* if we get here, no key is pressed */

    /* gets here when one of the rows has key pressed, check which column it is */
    if (col == 0x000E) return row * 4 + 1;    /* key in column 0 */
    if (col == 0x000D) return row * 4 + 2;    /* key in column 1 */
    if (col == 0x000B) return row * 4 + 3;    /* key in column 2 */
    if (col == 0x0007) return row * 4 + 4;    /* key in column 3 */

    return 0;   /* just to be safe */
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
