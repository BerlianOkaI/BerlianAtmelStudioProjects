/*
 * LiquidCrystal.cpp
 *
 * Created: 18/05/2023 05:17:56
 *  Author: ASUS
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "ADCTimer.h"
#include "LiquidCrystal.h"

/* Functions and Procedures */
void LCD_command(unsigned char com){
	LCD_dataPort = com;
	// Select RS as command register 0 and RW as write 0
	LCD_comPort &= ~(1 << RS);
	LCD_comPort &= ~(1 << RW);
	// Give falling edge pulse on EN pin to enabling Command
	LCD_comPort |= (1 << EN);
	delay1us(10);
	LCD_comPort &= ~(1 << EN);
	delay1us(40);
}
void LCD_char(unsigned char char_in){
	LCD_dataPort = char_in;
	// Select RS as data register 1 and RW as write 0
	LCD_comPort |= (1 << RS);
	LCD_comPort &= ~(1 << RW);
	// Give falling edge pulse on EN pin for writing data into LCD
	LCD_comPort |= (1 << EN);
	delay1us(10);
	LCD_comPort &= ~(1 << EN);
	delay1us(40);
}
void LCD_init(void){
	LCD_dataDir = 0xFF;							// Set all of data port as Output
	LCD_comDir = LCD_comDir | (1 << RS)|(1<<RW)|(1<<EN);				
	delay1ms(30);
	// Setting out the LCD
	LCD_command(0x38);							// Initialization of LCD in 8 bit mode
	LCD_command(0x0C);							// Display ON Cursor OFF
	LCD_command(0x06);							// Auto increment cursor
	LCD_command(0x01);							// Clear display
	LCD_command(0x80);							// Cursor at home position
}
void LCD_string(char *string_data){
	unsigned char iter_;
	iter_ = 0;
	while (string_data[iter_] != 0){
		// Print seluruh char dalam string input sampai string[iter_] menunjukkan nilai null (0)
		LCD_char(string_data[iter_]);
		iter_ ++;
	}
}
void LCD_stringXY(char row_, char col_, char *string_data){
	if (row_ == 0 && col_ < 16){
		// 0x80 is required for configuring out the row position (row 0)
		// The last four bit is used to set up the col position
		LCD_command((0x0F & col_)|0x80);
		LCD_string(string_data);
	}
	else if (row_ == 1 && col_ < 16){
		// 0xC0 is required for configuring out the row position (row 1)
		LCD_command(0xC0 | (0x0F & col_));
		LCD_string(string_data);
	}
}
void LCD_numString(unsigned int num_in){
	// Print num_in into LCD
	unsigned int num_;
	unsigned int num_s;						// Single num that will be displayed
	unsigned int exponent_ = 1;
	
	num_ = num_in;
	while(num_ / (exponent_ * 10) != 0){
		exponent_ = exponent_ * 10;
	}
	while(exponent_ != 0){
		num_s = num_ / exponent_;
		num_ = num_ % exponent_;
		exponent_ = exponent_ / 10;
		
		LCD_char(48 + num_s);
	}
}
void LCD_numStringXY(char row_, char col_, unsigned int num_in){
	if (row_ == 0 && col_ < 16){
		// 0x80 is required for configuring out the row position (row 0)
		// The last four bit is used to set up the col position
		LCD_command((0x0F & col_)|0x80);
		LCD_numString(num_in);
	}
	else if (row_ == 1 && col_ < 16){
		// 0xC0 is required for configuring out the row position (row 1)
		LCD_command(0xC0 | (0x0F & col_));
		LCD_numString(num_in);
	}
}
void LCD_clear(void){
	LCD_command(0x01);			// Clear display
	LCD_command(0x80);			// Cursor at home position
}