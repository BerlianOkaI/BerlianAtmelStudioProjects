/*
 * ControlAndCommunication.h
 *
 * Created: 19/05/2023 21:53:53
 *  Author: ASUS
 */ 


#ifndef CONTROLANDCOMMUNICATION_H_
#define CONTROLANDCOMMUNICATION_H_
/* CUSTOM DEFINITIONS */
#define F_CPU 8000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALE 51				// For F_CPU 8 MHz and Baud Rate 9600
#define LAMP_CONTROLPORT PORTC
#define LAMP_DDR DDRC
#define LAMP1_PIN 0
#define LAMP2_PIN 1


/* PROTOTYPES */

// USART Part
void UART_init(unsigned char receive_IE, unsigned char transmit_IE);
unsigned char UART_RxChar(void);
void UART_TxChar(char chara);
void UART_sendString(char *str);

// Lamp Control Part
void lampControlInit(void);
void lampActuator(unsigned char status);



#endif /* CONTROLANDCOMMUNICATION_H_ */