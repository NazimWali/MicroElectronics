/* p3_5.c: Matrix keypad scanning and Output to LCD
 *
 * This program scans a 4x4 matrix keypad and returns a unique
 * number for each key pressed.
 * The LCD is used to output the returned number.
 *
 * PC0-3 are connected to the columns and PC4-7 are
 * connected to the rows of the keypad.
 */

#include "stm32f4xx.h"

#define RS 0x20     /* PA5 mask for reg select */
#define EN 0x80     /* PA7 mask for enable */

void delayMs(int n);
void keypad_init(void);
char keypad_getkey(void);

void LCD_nibble_write(char data, unsigned char control);
void LCD_command(unsigned char command);
void LCD_data(char data);
void LCD_init(void);
void PORTS_init(void);

int main(void) {
	
		unsigned char nums[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
		LCD_init();
	
    unsigned char key;

    keypad_init();

    while(1) {
        key = keypad_getkey();  /* read the keypad */
        if (key != 0)
            LCD_data(nums[key-1]);     /* set LEDs according to the key code */
						delayMs(1000);
				
				
				/* clear LCD display */
       // LCD_command(1);
       // delayMs(1000);
    }
}
/* this function initializes PC0-3 (column) and PC4-7 (row).
 * The column pins need to have the pull-up resistors enabled.
 */
void keypad_init(void) {
    RCC->AHB1ENR |=  0x14;	        /* enable GPIOC clock */
    GPIOC->MODER &= ~0x0000FFFF;    /* clear pin mode to input */
    GPIOC->PUPDR =   0x00000055;    /* enable pull up resistors for column pins */
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
    GPIOC->MODER = 0x00005500;      /* make all row pins output */
    GPIOC->BSRR =  0x00F00000;      /* drive all row pins low */
    delayMs(1000);                        /* wait for signals to settle */
    col = GPIOC->IDR & 0x000F;      /* read all column pins */
    GPIOC->MODER &= ~0x0000FF00;    /* disable all row pins drive */
    if (col == 0x000F)              /* if all columns are high */
        return 0;                       /* no key pressed */

    /* If a key is pressed, it gets here to find out which key.
     * It activates one row at a time and read the input to see
     * which column is active. */
    for (row = 0; row < 4; row++) {
        GPIOC->MODER &= ~0x0000FF00;    /* disable all row pins drive */
        GPIOC->MODER |= row_mode[row];  /* enable one row at a time */
        GPIOC->BSRR = row_low[row];     /* drive the active row low */
        delayMs(1000);                        /* wait for signal to settle */
        col = GPIOC->IDR & 0x000F;      /* read all columns */
        GPIOC->BSRR = row_high[row];    /* drive the active row high */
        if (col != 0x000F) break;       /* if one of the input is low, some key is pressed. */
    }
    GPIOC->BSRR = 0x000000F0;           /* drive all rows high before disable them */
    GPIOC->MODER &= ~0x0000FF00;        /* disable all rows */
    if (row == 4)
        return 0;                       /* if we get here, no key is pressed */

    /* gets here when one of the rows has key pressed, check which column it is */
    if (col == 0x000E) return row * 4 + 1;    /* key in column 0 */
    if (col == 0x000D) return row * 4 + 2;    /* key in column 1 */
    if (col == 0x000B) return row * 4 + 3;    /* key in column 2 */
    if (col == 0x0007) return row * 4 + 4;    /* key in column 3 */

    return 0;   /* just to be safe */
}

/* initialize GPIOA/B then initialize LCD controller */
void LCD_init(void) {
    PORTS_init();

    delayMs(20);                /* LCD controller reset sequence */
    LCD_nibble_write(0x30, 0);
    delayMs(5);
    LCD_nibble_write(0x30, 0);
    delayMs(1);
    LCD_nibble_write(0x30, 0);
    delayMs(1);

    LCD_nibble_write(0x20, 0);  /* use 4-bit data mode */
    delayMs(1);
    LCD_command(0x28);          /* set 4-bit data, 2-line, 5x7 font */
    LCD_command(0x06);          /* move cursor right */
    LCD_command(0x01);          /* clear screen, move cursor to home */
    LCD_command(0x0F);          /* turn on display, cursor blinking */
}

void PORTS_init(void) {
    RCC->AHB1ENR |=  0x06;          /* enable GPIOA/B clock */
		RCC->AHB1ENR |=  1;             /* enable GPIOA clock */

    /* PORTA 5 for LCD R/S */
    /* PORTA 7 for LCD EN */
    GPIOA->MODER &= ~0x0000CC00;    /* clear pin mode */
    GPIOA->MODER |=  0x00004400;    /* set pin output mode */
    GPIOA->BSRR = 0x00800000;       /* turn off EN */

    /* PB4-PB7 for LCD D4-D7, respectively. */
    GPIOB->MODER &= ~0x0000FF00;    /* clear pin mode */
    GPIOB->MODER |=  0x00005500;    /* set pin output mode */
}

void LCD_nibble_write(char data, unsigned char control) {
    /* populate data bits */
    GPIOB->BSRR = 0x00F00000;       /* clear data bits */
    GPIOB->BSRR = data & 0xF0;      /* set data bits */

    /* set R/S bit */
    if (control & RS)
        GPIOA->BSRR = RS;
    else
        GPIOA->BSRR = RS << 16;

    /* pulse E */
    GPIOA->BSRR = EN;
    delayMs(0);
    GPIOA->BSRR = EN << 16;
}

void LCD_command(unsigned char command) {
    LCD_nibble_write(command & 0xF0, 0);    /* upper nibble first */
    LCD_nibble_write(command << 4, 0);      /* then lower nibble */

    if (command < 4)
        delayMs(2);         /* command 1 and 2 needs up to 1.64ms */
    else
        delayMs(1);         /* all others 40 us */
}

void LCD_data(char data) {
    LCD_nibble_write(data & 0xF0, RS);      /* upper nibble first */
    LCD_nibble_write(data << 4, RS);        /* then lower nibble */

    delayMs(1);
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
