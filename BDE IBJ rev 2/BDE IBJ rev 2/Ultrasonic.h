/*
 * Ultrasonic.h
 *
 * Created: 18/05/2023 10:20:02
 *  Author: ASUS
 */ 


#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_
/* Custom defines */
#define USPORT PORTB
#define USPIN PINB
#define USDDR DDRB
#define MC_PERIOD_US 0.125
#define SOUNDSPEED_CMPS 34300.0
#define US_BUFFER_SIZE 10

/* PROTOTYPES */
double distance_cm(unsigned int tov_timer, unsigned int timer);
void US_timer0_IntrCallback(void);

class USSensor
{
private:
	unsigned char _trig;
	unsigned char _echo;
	unsigned long int _overflown_timer;
	unsigned int _timer;
public:
	double dist_read;		// in cm
	char dist_data[US_BUFFER_SIZE];
	
	USSensor(unsigned char trig_pin, unsigned char echo_pin);
	void sensorInit(void);
	void getDistanceCM(void);
};


#endif /* ULTRASONIC_H_ */