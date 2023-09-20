/*
 * ADCTimer.cpp
 *
 * Created: 17/05/2023 21:29:51
 *  Author: ASUS
 */ 

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "ADCTimer.h"


/* Variables, Data, and Constants */
/*
const float LS0_data[6] = {
	-333.86, 786.43, -705.3, 303.21, -62.697, 5.0413
};
const float LS1_data[6] = {
	-1716.1, 3243.5, -2401.4, 871.93, -155.45, 10.93
};
const float LS2_data[6] = {
	-120.59, 321.41, -322.97, 154.99, -35.474, 3.1583
};
const float LS3_data[6] = {
	-170.99, 465.28, -458.55, 211.53, -46.302, 3.9256
};
*/
float volt_threshold[40] = {
	0.007, 0.009, 0.012, 0.016, 0.021, 0.028, 0.037, 0.049, 0.064, 0.085,
	0.112, 0.147, 0.194, 0.253, 0.331, 0.429, 0.554, 0.709, 0.898, 1.124,
	1.388, 1.686, 2.011, 2.353, 2.699, 3.036, 3.351, 3.635, 3.884, 4.095,
	4.269, 4.411, 4.524, 4.614, 4.683, 4.737, 4.779, 4.810, 4.834, 4.852
};
float data_LS[40] = {
	0.100, 0.120, 0.144, 0.173, 0.207, 0.249, 0.299, 0.358, 0.430, 0.516,
	0.619, 0.743, 0.892, 1.070, 1.284, 1.541, 1.849, 2.219, 2.662, 3.195,
	3.834, 4.601, 5.521, 6.625, 7.950, 9.540, 11.45, 13.74, 16.48, 19.78,
	23.74, 28.49, 34.18, 41.02, 49.22, 59.07, 70.88, 85.06, 102.1, 122.5
};


/* Functions and Procedures */

void delay1ms(unsigned int length){
	// Creating delay of 1 ms times length
	// Using Fosc = Using Internal Clock Source (8 MHz)
	unsigned int _iter;
	for (_iter=0; _iter<length; _iter++){
		TCNT0 = 0x83;
		TCCR0 = 0x03;						// Start Timer using 64 as clk divider. Thus, we have period = 8us
		while((TIFR & (1 << TOV0)) == 0);	// Polling TimerOverflown0 Flag Until It is Set
		TCCR0 = 0x00;
		TIFR = TIFR | (1 << TOV0);			// Put 1 to TOV0 to clear the flag
	}
}
void delay1us(unsigned int length){
	// Creating delay of 1 us times length
	// Using Fosc = internal clock Sources (1 MHz)
	unsigned int _iter;
	for (_iter=0; _iter<length; _iter++){
		TCNT0 = 0xFB;
		TCCR0 = 0x01;						// Start Timer using no prescaler. Thus, we have period = 0.125us
		while((TIFR & (1 << TOV0)) == 0);	// Polling TimerOverflown0 Flag Until It is Set
		TCCR0 = 0x00;
		TIFR = TIFR | (1 << TOV0);			// Put 1 to TOV0 to clear the flag
	}
}
void delay50us(unsigned int length){
	// Creating delay of 50 us times length
	// Using Fosc = 8 (MHz) (Internal Calibrated Clock)
	unsigned int _iter;
	for (_iter=0; _iter<length; _iter++){
		TCNT0 = 0xD1;
		TCCR0 = 0x02;						// Start Timer using 8 as prescaler. Period = 1us
		while((TIFR & (1 << TOV0)) == 0);	// Polling TimerOverflown0 Flag Until It is Set
		TCCR0 = 0x00;
		TIFR = TIFR | (1 << TOV0);			// Put 1 to TOV0 to clear the flag
	}
}
void ADCInit(void){
	// Makes all pin of PORTA as input
	DDRA = 0x00;
	// Enabling ADC and select clock divisor (8)
	ADCSRA = (1 << ADEN) | (1 << ADPS0) | (1 << ADPS1);
}
int ADCRead(unsigned char channel){
	int _low_val, _high_val;
	// Select VRef of ADC as AVcc
	ADMUX = (1 << REFS0);
	// Choose the channel pin used in ADMUX
	ADMUX = (ADMUX & 0xF8) | (channel & 0xFF);
	// Start conversion and polling until ADIF is set
	ADCSRA = ADCSRA | (1 << ADSC);
	while ((ADCSRA & (1 << ADIF)) == 0);
	delay1us(25);
	// Read the value of ADC from ADCH and ADCL
	_low_val = (int)ADCL;
	_high_val = (int)ADCH * 256;
	return _high_val + _low_val;
}

LDRData::LDRData(unsigned char used_pin){
	// Set _input_pin as used_pin
	_input_pin = used_pin;
}
float LDRData::linear_approx(float _volt){
	/* Interpolasi linear */
	// Cari index terdekat
	int _index;
	char _stop;
	_stop = 0;
	_index = 19;
	while (_volt < volt_threshold[_index] && _stop == 0){
		if (_index == 0){
			_stop = 1;
		}
		else{
			_index = _index - 1;
		}
	}
	// Di sini index terdekat telah diperoleh
	// Selanjutnya gunakan aproksimasi linear
	// Hitung gradien dan interseksi
	double _grad;
	double _intersect;
	_grad = 0;
	_intersect = 0;
	if (_index < 19){
		_grad = (((double)data_LS[_index+1] - (double)data_LS[_index])/((double)volt_threshold[_index+1] - (double)volt_threshold[_index]));
		_intersect = (((double)volt_threshold[_index+1]*(double)data_LS[_index]) - ((double)volt_threshold[_index]*(double)data_LS[_index+1]));
		_intersect = _intersect / ((double)volt_threshold[_index+1] - (double)volt_threshold[_index]);
	}
	else {
		_grad = ((double)data_LS[_index] - (double)data_LS[_index-1])/((double)volt_threshold[_index] - (double)volt_threshold[_index-1]);
		_intersect = (((double)volt_threshold[_index]*(double)data_LS[_index-1]) - ((double)volt_threshold[_index-1]*(double)data_LS[_index]));
		_intersect = _intersect / ((double)volt_threshold[_index] - (double)volt_threshold[_index-1]);
	}
	// Hitung Lux yang didapat
	float result;
	result = (float)_grad * _volt + (float)_intersect;
	return result;
}
void LDRData::getADCValue(void){
	// Read adc value;
	adc_value = ADCRead(_input_pin);
	// Calculate the read voltage
	volt_value = 5.0 * (float)adc_value/1023.0;
}
void LDRData::getIntensity(void){
	// Before using this method, make sure that we have read the adc data
	// and convert it into voltage
	lux_value = 0;
	// This switch procedure will executed for specific case that depend on _input_pin
	/*
	switch (_input_pin) {
		case 0:
		for (_iter=0; _iter<6; _iter++){
			lux_value = lux_value + LS0_data[_iter]*_x;
			_x = _x * volt_value;
		}
		break;
		case 1:
		for (_iter=0; _iter<6; _iter++){
			lux_value = lux_value + LS1_data[_iter]*_x;
			_x = _x * volt_value;
		}
		break;
		case 2:
		for (_iter=0; _iter<6; _iter++){
			lux_value = lux_value + LS2_data[_iter]*_x;
			_x = _x * volt_value;
		}
		break;
		case 3:
		for (_iter=0; _iter<6; _iter++){
			lux_value = lux_value + LS3_data[_iter]*_x;
			_x = _x * volt_value;
		}
		break;
	}
	*/
	lux_value = linear_approx(volt_value);
	dtostrf(
		lux_value,
		6,
		2,
		lux_data
	);
}