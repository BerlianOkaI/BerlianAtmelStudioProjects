/*
 * BDE IBJ rev 2.cpp
 *
 * Created: 17/05/2023 19:43:48
 * Author : ASUS
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include "ADCTimer.h"
#include "Ultrasonic.h"
#include "ControlAndCommunication.h"

#define MSG_BUFFER_SIZE 70
#define MSG_SEND_DELAY_MS 500
#define LED_PORT PORTC
#define LED_DDR DDRC
#define LED_PIN 7

unsigned int timer2_cpms;
unsigned char lamp_status;
char msg[MSG_BUFFER_SIZE];
int msg_index;
char rec_enable;

LDRData LS0(0);
LDRData LS1(1);
LDRData LS2(2);
LDRData LS3(3);

USSensor US1(3,2);
USSensor US2(5,4);

/* FUNCTIONS AND PROCEDURES */
void LED_Init(void){
	LED_DDR = LED_DDR | (1 << LED_PIN);
}
void timer2_interruptInit(void){
	// Using Fosc (internal RC Oscillator) 8 MHz
	// Use 64 as prescaler
	TIMSK = TIMSK | (1 << TOIE2);					// Enabling interrupt at overflown flag of timer2
	TCNT2 = 0x84;									// Load timer/counter register
	TCCR2 = 0x03;									// Start timer2, using 64 as prescaler
}
void dataToESP8266(unsigned char _lpstatus){
	/* Kirim data ke ESP8266 */
	unsigned char lp1, lp2;
	lp1 = 0;
	lp2 = 0;
	// Kosongkan msg
	for (int j=0; j<MSG_BUFFER_SIZE; j++){
		msg[j] = 0;
	}
	// Tentukan status LP1 dan LP2
	switch (_lpstatus){
	case 0:
		lp1 = 0;
		lp2 = 0;
		break;
	case 1:
		lp1 = 1;
		lp2 = 0;
		break;
	case 2:
		lp1 = 0;
		lp2 = 1;
		break;
	case 3:
		lp1 = 1;
		lp2 = 1;
		break;
	}
	// kirim data ke ESP8266
	snprintf(
		msg,
		MSG_BUFFER_SIZE,
		"@;%d;%d;%d;%d;%s;%s;%d;%d;#%c",
		LS0.adc_value, LS1.adc_value, LS2.adc_value, LS3.adc_value,
		US1.dist_data, US2.dist_data, lp1, lp2, '\n'
	);
	UART_sendString(msg);
}

/* INTERRUPT SERVICE ROUTINE */
ISR(USART_RXC_vect){
	unsigned char holder;
	// Read UART Data Register
	holder = (unsigned char)UDR;
	if (rec_enable == 1){
		if (holder == '1'){
			lamp_status = 1;
		}
		else if (holder == '0'){
			lamp_status = 0;
		}
	}
	else if (holder == '@'){
		rec_enable = 1;
	}
	else if (holder == '#'){
		rec_enable = 0;
	}
}
ISR(TIMER2_OVF_vect){
	/* Interrupt Service Routine for Timer2 */
	// Update timer2 check point (in ms)
	timer2_cpms ++;
	// reload TCNT2
	TCNT2 = 0x83;
	// If timer2_cpms exceed MSG_SEND_DELAY, send data to ESP8266
	if (timer2_cpms >= MSG_SEND_DELAY_MS){
		LED_PORT = (LED_PORT&(~(1<<LED_PIN)))|((~LED_PORT)&(1<<LED_PIN));
	}
}
ISR(TIMER0_OVF_vect){
	// This interrupt will only be called by the time when US Sensor make a measurement
	US_timer0_IntrCallback();			// Callback function in Ultrasonic Module
}

/* MAIN PROGRAM */
int main(void){
	DDRC = 0xFF;
	LED_Init();
	lamp_status = 0;
	msg_index = 0;
	rec_enable = 0;
	
	// Initiating UART
	UART_init(1,0);
	
	// Initiating Lamp Controller
	lampControlInit();
	lampActuator(lamp_status);
	
	// Initiating Sensors
	ADCInit();
	US1.sensorInit();
	US2.sensorInit();
	
	// Initiate timer2 interrupt service routine
	sei();
	timer2_interruptInit();
    while (1) {
		// Read data from light sensors
		LS0.getADCValue();
		LS1.getADCValue();
		LS2.getADCValue();
		LS3.getADCValue();
		
		LS0.getIntensity();
		LS1.getIntensity();
		LS2.getIntensity();
		LS3.getIntensity();
		
		// Read data from ultrasonic sensors
		US1.getDistanceCM();
		delay1ms(50);
		US2.getDistanceCM();
		
		// Control the lamp
		lampActuator(lamp_status);
		
		// Send data
		if (timer2_cpms >= MSG_SEND_DELAY_MS){
			timer2_cpms = 0;
			dataToESP8266(lamp_status);
		}
    }
}

