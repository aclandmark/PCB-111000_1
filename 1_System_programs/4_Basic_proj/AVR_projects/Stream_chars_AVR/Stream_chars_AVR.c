
/*****************************************************************************/
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdio.h>

 


void I2C_Tx_2_integers(unsigned int, unsigned int);
void Timer_T1_sub(char, unsigned int);
void USART_init (unsigned char, unsigned char);
void Char_to_PC(char);



/*****************************************************************************/
#include "Stream_chars_header_file.h"
#include "Stream_chars_subroutines.c"



int main (void){								
unsigned int n=33;

CLKPR = (1 << CLKPCE);
CLKPR = (1 << CLKPS0);
							
setup_watchdog;
set_up_I2C;
ADMUX |= (1 << REFS0);
Unused_I_O;
USART_init(0,16);

while(1){
while(n < 127){Char_to_PC(n++);Timer_T1_sub(T1_delay_100ms);}n = 33;}}
