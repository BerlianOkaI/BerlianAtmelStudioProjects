/*
 * LiquidCrystal.h
 *
 * Created: 18/05/2023 05:17:36
 *  Author: ASUS
 */ 


#ifndef LIQUIDCRYSTAL_H_
#define LIQUIDCRYSTAL_H_
/* Defines */
#define LCD_dataDir DDRC
#define LCD_comDir DDRB
#define LCD_dataPort PORTC
#define LCD_comPort PORTB
#define RS 5
#define RW 6
#define EN 7

/* Prototype of Functions and Procedures */
void LCD_command(unsigned char com);
void LCD_char(unsigned char char_in);
void LCD_init(void);
void LCD_string(char *string_data);
void LCD_stringXY(char row_, char col_, char *string_data);
void LCD_numString(unsigned int num_in);
void LCD_numStringXY(char row_, char col_, unsigned int num_in);
void LCD_clear(void);

#endif /* LIQUIDCRYSTAL_H_ */