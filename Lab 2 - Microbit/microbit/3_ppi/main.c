#include "ppi.h"
#include "gpiote.h"
#include "gpio.h"

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


    // GPIOTE channel 0 set to Button A
    GPIOTE->CONFIG[0] = (1 << 20) | (3 << 16) | (0 << 12) | (14 << 8) | 3; // Set initial outinit to high (no effect for event mode), set event and task to toggle on trigger, set port, set pin, set task or event mode

    // GPIOTE channel 1 set to Row 1
    GPIOTE->CONFIG[1] = (1 << 20) | (3 << 16) | (0 << 12) | (21 << 8) | 1; // Set port as 0 and pin as 14. 0b11000000000

    // GPIOTE channel 2 set to Row 2
    GPIOTE->CONFIG[2] = (1 << 20) | (3 << 16) | (0 << 12) | (22 << 8) | 1; // Set port as 0 and pin as 14. 0b11000000000

    // GPIOTE channel 3 set to Row 3
    GPIOTE->CONFIG[3] = (1 << 20) | (3 << 16) | (0 << 12) | (15 << 8) | 1; // Set port as 0 and pin as 14. 0b11000000000

    // GPIOTE channel 4 set to Row 4
    GPIOTE->CONFIG[4] = (0 << 20) | (3 << 16) | (0 << 12) | (24 << 8) | 1; // Set port as 0 and pin as 14. 0b11000000000

    // GPIOTE channel 5 set to Row 5
    GPIOTE->CONFIG[5] = (1 << 20) | (3 << 16) | (0 << 12) | (19 << 8) | 1; // Set port as 0 and pin as 14. 0b11000000000

    for(int channel = 0; channel <= 5; channel++)
    {
        PPI->CHEN = 1 << channel;
        PPI->CHENSET = 1 << channel;
    }
    PPI->PPI_CH[1].EEP = (uint32_t)&(GPIOTE->EVENTS_IN[0]);
    PPI->PPI_CH[1].TEP = (uint32_t)&(GPIOTE->TASKS_OUT[1]);

    PPI->PPI_CH[2].EEP = (uint32_t)&(GPIOTE->EVENTS_IN[0]);
    PPI->PPI_CH[2].TEP = (uint32_t)&(GPIOTE->TASKS_OUT[2]);

    PPI->PPI_CH[3].EEP = (uint32_t)&(GPIOTE->EVENTS_IN[0]);
    PPI->PPI_CH[3].TEP = (uint32_t)&(GPIOTE->TASKS_OUT[3]);

    PPI->PPI_CH[4].EEP = (uint32_t)&(GPIOTE->EVENTS_IN[0]);
    PPI->PPI_CH[4].TEP = (uint32_t)&(GPIOTE->TASKS_OUT[4]);

    PPI->PPI_CH[5].EEP = (uint32_t)&(GPIOTE->EVENTS_IN[0]);
    PPI->PPI_CH[5].TEP = (uint32_t)&(GPIOTE->TASKS_OUT[5]);

	while(1){}

    return 0;
}

// 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