

void I2C_Tx_2_integers(unsigned int, unsigned int);
void I2C_Tx(char, char, char*);
void send_byte_with_Ack(char);
void send_byte_with_Nack(char);
void Timer_T1_sub(char, unsigned int);
unsigned int PRN_16bit_GEN(unsigned int );


#define T1_delay_50ms 5,0xFE78
#define T1_delay_100ms 5,0xFCF2
#define T1_delay_250ms 5,0xF85F
#define T1_delay_500ms 5,0xF0BE


/*****************************************************************************/
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdio.h>


/*****************************************************************************/
#define SW_reset {wdt_enable(WDTO_30MS);while(1);}

/*****************************************************************************/
#define setup_HW \
setup_watchdog;\
Unused_I_O;\
set_up_I2C;\
ADMUX |= (1 << REFS0);


/*****************************************************************************/
#define setup_watchdog \
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;

#define wdr()  __asm__ __volatile__("wdr")


/*****************************************************************************/
#define set_up_I2C \
TWAR = 0x02;


/*****************************************************************************/
#define Unused_I_O \
MCUCR &= (~(1 << PUD));\
DDRB = 0;\
DDRC = 0;\
DDRD = 0;\
PORTB = 0xFF;\
PORTC  = 0xFF;\
PORTD  = 0xFF;


/*****************************************************************************/
#define waiting_for_I2C_master \
TWCR = (1 << TWEA) | (1 << TWEN);\
while (!(TWCR & (1 << TWINT)));\
TWDR;

#define clear_I2C_interrupt \
TWCR = (1 << TWINT);


/*
int main (void){								//The "main" routine lies between a pair of {} brackets.

unsigned long PORT_1, PORT_2;
 
setup_HW;					

while(1)
{PORT_1=1; PORT_2 = 0x80000000;

	for(int m = 1; m < 33; m++){				//Code between the {} brackets is repeated 16 times 						
												//as m increments from 1 to 16	(m++ is shorthand for increment m)
		I2C_Tx_2_integers(PORT_1, PORT_2);				

		Timer_T1_sub(T1_delay_50ms);			//This subroutine generates a 10mS delay that is repeated 6 times		
		PORT_1 = PORT_1 << 1;					//The contents of address "PORT_1" are shifted one place left
		PORT_2 = PORT_2 >> 1;
		}										//every time that the "for-loop" is executed
}}*/


int main (void){	
unsigned int PRN;											//Memory location used to store "pseudo random numbers"

setup_HW;
wdt_enable(WDTO_250MS);										//Set up the watchdog timer to generate a reset after 250mS.
//config_sw2_for_PCI;
sei();
while(1){													//Infinite while loop	
PRN = PRN_16bit_GEN (0);									//Generate a new PRN (0) tells subroutine to use the EEPROM
I2C_Tx_2_integers(PRN, (PRN<<1));							//Display two "pseudo random numbers"
Timer_T1_sub(T1_delay_100ms);								//Pause before repeating
wdr();}}													//Reset the watchdog timer which avoids the possibility
															//of a reset for another 250mS



void Timer_T1_sub(char Counter_speed, unsigned int Start_point){ 
TCNT1H = (Start_point >> 8);
TCNT1L = Start_point;
TCCR1B = Counter_speed;
while(!(TIFR1 & (1<<TOV1)));
TIFR1 |= (1<<TOV1); 
TCCR1B = 0;}


void I2C_Tx_2_integers(unsigned int s1, unsigned int s2){			
char num_bytes=4; char mode=4; char s[4];
for (int m = 0;  m < 4; m++){
switch (m){
case 0: s[m] = s1; break; 											//Send S1 lower byte
case 1: s[m] = s1 >> 8; break; 									//Send S1 higher byte									
case 2: s[m] = s2; 	break;											//Send S1 lower byte
case 3: s[m] = s2 >> 8; break;}}									//Send S1 higher byte
I2C_Tx(num_bytes,mode, s);}


void I2C_Tx(char num_bytes, char mode, char s[]){
waiting_for_I2C_master;
send_byte_with_Ack(num_bytes);
send_byte_with_Ack(mode);
for (int m = 0;  m < num_bytes; m++){
if (m==num_bytes-1){send_byte_with_Nack(s[m]);}
else {send_byte_with_Ack(s[m]);}}
TWCR = (1 << TWINT);}

/***********************************************************/
void send_byte_with_Ack(char byte){
TWDR = byte;											//Send payload size: Zero in this case
TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);		//clear interrupt and set Enable Acknowledge
while (!(TWCR & (1 << TWINT)));}

/***********************************************************/
void send_byte_with_Nack(char byte){
TWDR = byte;										//Send payload size: Zero in this case
TWCR = (1 << TWINT) | (1 << TWEN);		//clear interrupt and set Enable Acknowledge
while (!(TWCR & (1 << TWINT)));}



unsigned int PRN_16bit_GEN(unsigned int start){
unsigned int bit, lfsr;

if(!(start)) lfsr = (eeprom_read_byte((uint8_t*)(0x1FF)) << 8) + eeprom_read_byte((uint8_t*)(0x1FE));
else lfsr = start;
bit = (( lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
lfsr = (lfsr >> 1) | (bit << 15);
if(!(start)){
eeprom_write_byte((uint8_t*)(0x1FF),(lfsr>>8));
eeprom_write_byte((uint8_t*)(0x1FE),lfsr);}

return lfsr;}
	
