/*
 * ADCTimer.h
 *
 * Created: 17/05/2023 19:46:39
 *  Author: ASUS
 */ 


#ifndef ADCTIMER_H_
#define ADCTIMER_H_
/* Definition */
#define LS_BUFFER_SIZE 10

/* Prototypes */

void delay1ms(unsigned int length);
void delay1us(unsigned int length);
void delay50us(unsigned int length);

void ADCInit(void);
int ADCRead(unsigned char channel);

class LDRData
{
private:
	unsigned char _input_pin;
	
	float linear_approx(float _volt);
public:
	int adc_value;
	float volt_value;
	float lux_value;
	char lux_data[LS_BUFFER_SIZE];
	
	LDRData(unsigned char used_pin);
	void getADCValue(void);
	void getIntensity(void);
};




#endif /* ADCTIMER_H_ */