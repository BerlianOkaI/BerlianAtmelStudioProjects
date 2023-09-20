/*
 * ControlAndCommunication.cpp
 *
 * Created: 19/05/2023 21:54:13
 *  Author: ASUS
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include "ADCTimer.h"
#include "ControlAndCommunication.h"

/* FUNCTIONS AND PROCEDURES */
void UART_init(unsigned char receive_IE, unsigned char transmit_IE){
	UBRRH = (unsigned char)(BAUD_PRESCALE >> 8);						// Untuk Baud Prescale besar, gunakan juga ini
	UBRRL = (unsigned char)BAUD_PRESCALE;								// Tentukan Baud Rate
	UCSRB = (1 << RXEN)|(1 << TXEN);									// Enabling Rx and Tx
	UCSRC = (1<<URSEL)|(1<<USBS)|(3<<UCSZ0);							// Set frame format: 8 data and 2 stop bit 
	
	
	// If you want to enabling interrupt enable, then
	if (receive_IE == 1){
		UCSRB = UCSRB | (1 << RXCIE);
	}
	if (transmit_IE == 1){
		UCSRB = UCSRB | (1 << UDRIE);
	}
}
unsigned char UART_RxChar(void){
	while((UCSRA & (1 << RXC)) == 0);									// Wait until data is received
	return (UDR);														// Read data
}
void UART_TxChar(char chara){
	while((UCSRA & (1 << UDRE)) == 0);									// Wait until transmit buffer is empty
	UDR = chara;														// Write char to UDR to transmit data
}
void UART_sendString(char *str){
	unsigned int _iter;
	_iter = 0;
	while(str[_iter] != 0){												// Send string till str[_iter] = null (or 0)
		UART_TxChar(str[_iter]);										// Send data one by one
		_iter++;
	}
}
void lampControlInit(void){
	// Setting the port as output
	LAMP_DDR = LAMP_DDR | (1 << LAMP1_PIN)|(1 << LAMP2_PIN);
}
void lampActuator(unsigned char status){
	/*
	For configuring the port that is used to control Lamp, check
	header of this library. Please check also the pin and port that is used
	*/
	switch (status){
	case 0:
		// Semua lampu mati
		LAMP_CONTROLPORT = LAMP_CONTROLPORT & (~(1 << LAMP1_PIN));
		LAMP_CONTROLPORT = LAMP_CONTROLPORT & (~(1 << LAMP2_PIN));
		break;
	case 1:
		// Lampu 1 saja yang nyala
		LAMP_CONTROLPORT = LAMP_CONTROLPORT | (1 << LAMP1_PIN);
		LAMP_CONTROLPORT = LAMP_CONTROLPORT & (~(1 << LAMP2_PIN));
		break;
	case 2:
		// Lampu 2 saja yang nyala
		LAMP_CONTROLPORT = LAMP_CONTROLPORT & (~(1 << LAMP1_PIN));
		LAMP_CONTROLPORT = LAMP_CONTROLPORT | (1 << LAMP2_PIN);
		break;
	case 3:
		// Kedua lampu menyala
		LAMP_CONTROLPORT = LAMP_CONTROLPORT | (1 << LAMP1_PIN);
		LAMP_CONTROLPORT = LAMP_CONTROLPORT | (1 << LAMP2_PIN);
		break;
	}
}