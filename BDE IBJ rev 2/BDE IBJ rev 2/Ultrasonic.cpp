/*
 * Ultrasonic.cpp
 *
 * Created: 18/05/2023 10:20:32
 *  Author: ASUS
 */ 

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include "ADCTimer.h"
#include "Ultrasonic.h"

/* Constants and Variables */
unsigned long timer_overflown;
unsigned char break_status;


/* ALGORTIHMS */
double distance_cm(unsigned long int tov_timer, unsigned int timer){
	/* Using Timer0 as timing */
	double _dist_cm;
	// Calculate total time of wave for traveling from transmitter to back to receiver
	_dist_cm = MC_PERIOD_US * ((double)timer + 256.00 * (double)tov_timer);
	// Calculate the distance from target to the sensor
	_dist_cm = _dist_cm * 0.5 * SOUNDSPEED_CMPS / 1000000.00;
	
	return _dist_cm;
}
void US_timer0_IntrCallback(void){
	// Callback function for interrupt service routine
	timer_overflown ++;
	if (timer_overflown > 749){
		break_status = 1;
	}
}
USSensor::USSensor(unsigned char trig_pin, unsigned char echo_pin){
	// Set trig pin and echo pin of sensor
	_trig = trig_pin;
	_echo = echo_pin;
}
void USSensor::sensorInit(void){
	// Setup _trig as output pin and _echo as input pin
	// give 1 to trig_ddr to make it output
	USDDR |= (1 << _trig);				
	// give 0 to echo_ddr to make it input
	USDDR |= (1 << _echo);
	USPORT &= (~(1 << _echo));
	USDDR &= (~(1 << _echo));
}
void USSensor::getDistanceCM(void){
	/* Measuring distance (in cm) using HC-SR04 */
	// Initialize value of timer
	_timer = 0;
	_overflown_timer = 0;
	timer_overflown = 0;
	break_status = 0;
	// Set initial value of TCNT0
	TCNT0 = 0x00;
	// Give 10 us Pulse to _trig_pin
	USPORT |= (1 << _trig);
	delay1us(10);
	USPORT &= (~(1 << _trig));
	// Enable Timer0 Interrupt
	TIMSK = TIMSK | (1 << TOIE0);
	while ((USPIN & (1 << _echo)) == 0);							// Poll _echo_pin until it HIGH
	TCCR0 = 0x01;													// Start timer0 without prescaller
	while ((USPIN & (1 << _echo)) != 0 && break_status == 0);		// Poll _echo_pin until it LOW
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //
	/*
	while ((USPIN & (1 << _echo)) != 0 && _break == 0){		// Poll _echo_pin until it LOW
		if ((TIFR & (1 << TOV0)) == 1){			// Update the value of overflown timer
			// Clear TOV0 flag
			TIFR = TIFR | (1 << TOV0);
			// Update overflown_timer
			_overflown_timer ++;
		}
		// Case when there is something wrong with _echo_pin
		if (_overflown_timer > 749){
			_break = 1;
		}
	}
	*/
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //
	TCCR0 = 0x00;								// Stop timer0 
	TIMSK = TIMSK & (~(1 << TOIE0));			// Disable timer0 interrupt
	_timer = (unsigned int)TCNT0;				// Read the value of TCNTO
	_overflown_timer = (unsigned long int)timer_overflown;
	// Calculate the readed distance
	dist_read = distance_cm(_overflown_timer, _timer);
	// Update the string form of dist data
	dtostrf(
		dist_read,
		6,
		2,
		dist_data
	);
}	