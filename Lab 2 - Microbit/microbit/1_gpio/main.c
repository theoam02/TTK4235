#include <stdint.h>

#define GPIO0 ((NRF_GPIO_REGS0*)0x50000000) //gpio 0 base adress
#define GPIO1 ((NRF_GPIO_REGS1*)0x50000300) //gpio 1 base adress

typedef struct {
	volatile uint32_t RESERVED0[321];
	volatile uint32_t OUT;
	volatile uint32_t OUTSET;
	volatile uint32_t OUTCLR;
	volatile uint32_t IN;
	volatile uint32_t DIR;
	volatile uint32_t DIRSET;
	volatile uint32_t DIRCLR;
	volatile uint32_t LATCH;
	volatile uint32_t DETECTMODE;
	volatile uint32_t RESERVED1_0[118]; // 0x700-0x528 = 0x1D8 = 472 472/4 = 118
	volatile uint32_t PIN_CNF[32];
} NRF_GPIO_REGS0;

typedef struct {
	volatile uint32_t RESERVED0[321];
	volatile uint32_t OUT;
	volatile uint32_t OUTSET;
	volatile uint32_t OUTCLR;
	volatile uint32_t IN;
	volatile uint32_t DIR;
	volatile uint32_t DIRSET;
	volatile uint32_t DIRCLR;
	volatile uint32_t LATCH;
	volatile uint32_t DETECTMODE;
	volatile uint32_t RESERVED1_1[118];
	volatile uint32_t PIN_CNF[10];
} NRF_GPIO_REGS1;

int led_row_array[5] = {21, 22, 15, 24, 19};
int led_col_array[5] = {28, 11, 31, 5, 30};

void led_set(int row, int col)
{
	/*if(col == 5)
	{
		GPIO1->OUTCLR |= 1 << col;
	}else{
		GPIO0->OUTCLR |= 1 << col;
	}*/
	GPIO1->OUTSET |= 1 << row;
}

void led_clr(int row, int col)
{
	if(col == 5)
	{
		GPIO1->OUTSET |= 1 << col;
	}else{
		GPIO0->OUTSET |= 1 << col;
	}
	GPIO1->OUTCLR |= 1 << row;
}

int main(){

	// Configure leds (dere må sjekke selv hvilken GPIO modul de ulike knappene tilhører)
	GPIO0->PIN_CNF[21] = 1; //Row 1
	GPIO0->PIN_CNF[22] = 1; //Row 2
	GPIO0->PIN_CNF[15] = 1; //Row 3
	GPIO0->PIN_CNF[24] = 1; //Row 4
	GPIO0->PIN_CNF[19] = 1; //Row 5

	GPIO0->PIN_CNF[28] = 1; //Col 1
	GPIO0->PIN_CNF[11] = 1; //Col 2
	GPIO0->PIN_CNF[31] = 1; //Col 3
	GPIO1->PIN_CNF[5] = 1;  //Col 4
	GPIO0->PIN_CNF[30] = 1; //Col 5 
	
	// Configure buttons (dere må sjekke selv hvilken GPIO modul de ulike knappene tilhører)
	GPIO0->PIN_CNF[14] = 0; // button A 
	GPIO0->PIN_CNF[23] = 0; // button B
	
	int sleep = 0;
	while(1){

		if(!(GPIO0->IN & (1 << 14)))
		{
			GPIO0->OUTSET |= (1 << 21);
			GPIO0->OUTSET |= (1 << 22);
			GPIO0->OUTSET |= (1 << 15);
			GPIO0->OUTSET |= (1 << 24);
			GPIO0->OUTSET |= (1 << 19);
		}

		if(!(GPIO0->IN & (1 << 23)))
		{
			GPIO0->OUTCLR |= (1 << 21);
			GPIO0->OUTCLR |= (1 << 22);
			GPIO0->OUTCLR |= (1 << 15);
			GPIO0->OUTCLR |= (1 << 24);
			GPIO0->OUTCLR |= (1 << 19);
		}
		sleep = 10000;
		while(--sleep);
	}
	return 0;
}
