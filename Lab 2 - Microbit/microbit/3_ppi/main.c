#include "ppi.h"
#include "gpio.h"
#include "uart.h"

int main(){

    int tgl = 0;

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

		if(!(GPIO0->IN & (1 << 14))) // If button A pressed
		{
			uart_send('A');
		}

		if(!(GPIO0->IN & (1 << 23))) // If button B pressed
		{
			uart_send('B');
		}

        if(uart_read() != '\0')
        {
            if(tgl == 0)
            {
                tgl = 1;
                GPIO0->OUTSET |= (1 << 21);
                GPIO0->OUTSET |= (1 << 22);
                GPIO0->OUTSET |= (1 << 15);
                GPIO0->OUTSET |= (1 << 24);
                GPIO0->OUTSET |= (1 << 19);
            }else if(tgl == 1){
                tgl = 0;
                GPIO0->OUTCLR |= (1 << 21);
                GPIO0->OUTCLR |= (1 << 22);
                GPIO0->OUTCLR |= (1 << 15);
                GPIO0->OUTCLR |= (1 << 24);
                GPIO0->OUTCLR |= (1 << 19);
            }
        }

		sleep = 10000;
		while(--sleep);
	}

    return 0;
}

// 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