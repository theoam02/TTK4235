#include "uart.h"

void uart_init(){
    GPIO0->PIN_CNF[6] = 0; // Set RX to input
    GPIO1->PIN_CNF[8] = 1; // Set TX to output

    UART->PSEL_RXD = 0x28; // Set Connect to 0 (connected), port to 0 and pin to 6. Together makes 000110
    UART->PSEL_TXD = 6; // Set Connect to 0 (connected), port to 1 and pin to 8. Together makes 011000

    UART->BAUDRATE = 0x00275000; // 9600 Baudrate

    UART->PSEL_RTS = 1 << 31;
    UART->PSEL_CTS = 1 << 31;

    UART->ENABLE = 4;

    UART->TASKS_STARTRX = 1;
}

void uart_send(char letter){
    UART->TASKS_STARTTX = 1;

    UART->TXD = letter;
    while(!UART->EVENTS_TXDRDY);
    UART->EVENTS_TXDRDY = 0;

    UART->TASKS_STOPTX = 1;
}

char uart_read(){
    if(!UART->EVENTS_RXDRDY) return '\0';

    UART->EVENTS_RXDRDY = 0;

    char received_char = UART->RXD;

    uart_send(received_char);
    
    return received_char;
}

void uart_send_str(char ** str){
    UART->TASKS_STARTTX = 1;
    char * letter_ptr = *str;
    while(*letter_ptr != '\0'){
        UART->TXD = *letter_ptr;
        while(!UART->EVENTS_TXDRDY);
        UART->EVENTS_TXDRDY = 0;
        letter_ptr++;
    }
}