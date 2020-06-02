



char watch_dog_reset = 0;
char User_response;
char MCUSR_copy;


#define T0_delay_5ms 5,220
#define T0_delay_4ms 4,227
#define T0_delay_10ms 5,178

#define T1_delay_50ms 5,0xFE78
#define T1_delay_100ms 5,0xFCF2
#define T1_delay_250ms 5,0xF85F
#define T1_delay_500ms 5,0xF0BE

#define T1_delay_1sec 5,0xE17D
#define T1_delay_2sec 5,0xC2FB
#define T1_delay_4sec 5,0x85F7

#define T2_delay_2ms 4,0
#define T2_delay_8ms 7,192
#define T2_delay_32ms 7,0

/*****************************************************************************/
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdio.h>

/*****************************************************************************/
#define SW_reset {wdt_enable(WDTO_30MS);while(1);}
#define clear_digits {for(int m = 0; m<=7; m++)digits[m]=0;}
#define pause {String_to_PC(" AK");waitforkeypress();}
#define shift_digits_left {for (int n = 0; n < 7; n++){digits[7-n] = digits[6-n];}}
#define shift_digits_right \
{int n = 0; while(digits[n+1] && (n<=6)){digits[n] = digits[n+1]; n++;} digits[n]=0;}


/*****************************************************************************/

#define setup_HW \
setup_watchdog;\
set_up_I2C;\
ADMUX |= (1 << REFS0);\
set_up_switched_inputs;\
Unused_I_O;\
Timer_T0_10mS_delay_x_m(5);\
USART_init(0,16);\
\
MCUSR_copy = eeprom_read_byte((uint8_t*)0x3FC);\
if (MCUSR_copy & (1 << PORF)){MCUSR_copy = (1 << PORF);\
eeprom_write_byte((uint8_t*)0x3F4,0);}\
\
User_app_commentary_mode;\
\
I2C_Tx_LED_dimmer();


/*****************************************************************************/
#define User_app_commentary_mode \
\
if(eeprom_read_byte((uint8_t*)0x3F4) == 0x40){\
for(int m = 0; m < 10; m++)String_to_PC("\r\n");\
String_to_PC("Project commentary: Press 'X' to escape or AOK\r\n");\
\
eeprom_write_byte((uint8_t*)0x3F4,0x41);}\
\
if ((eeprom_read_byte((uint8_t*)0x3F4) & 0x40)){\
eeprom_write_byte((uint8_t*)0x3F4,\
(eeprom_read_byte((uint8_t*)0x3F4) | 0x80));\
\
asm("jmp 0x6C60");}		////6C30????????????????


/*****************************************************************************/
#define setup_watchdog \
if (MCUSR & (1 << WDRF))watch_dog_reset = 1;\
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;

#define wdr()  __asm__ __volatile__("wdr")

#define wd_timer_off \
wdr();\
MCUSR &= (~(1 << WDRF));\
WDTCSR |= (1<<WDCE) | (1<<WDE);\
WDTCSR = 0x00;


/*****************************************************************************/
#define One_25ms_WDT_with_interrupt \
wdr();\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = (1<< WDE) | (1 << WDIE) |  (1 << WDP0)  |  (1 << WDP1);


/*****************************************************************************/
#define set_up_I2C \
TWAR = 0x02;


/*****************************************************************************/
#define set_up_switched_inputs \
MCUCR &= (~(1 << PUD));\
DDRD &= (~((1 << PD2)|(1 << PD7)));\
PORTD |= ((1 << PD2) | (1 << PD7));\
DDRB &= (~(1 << PB2));\
PORTB |= (1 << PB2);


/*****************************************************************************/
#define Unused_I_O \
MCUCR &= (~(1 << PUD));\
DDRB &= (~((1 << PB6)|(1 << PB7)));\
DDRC &= (~((1 << PC0)|(1 << PC1)|(1 << PC2)));\
DDRD &= (~((1 << PD3)|(1 << PD4)|(1 << PD5)|(1 << PD6)));\
PORTB |= ((1 << PB6)|(1 << PB7));\
PORTC |= ((1 << PC0)|(1 << PC1)|(1 << PC2));\
PORTD |= ((1 << PD3)|(1 << PD4)|(1 << PD5)|(1 << PD6));



/*****************************************************************************/
#define User_prompt \
while(1){\
do{String_to_PC("R?    ");}	 while((isCharavailable(250) == 0));\
User_response = receiveChar();\
if((User_response == 'R') || (User_response == 'r'))break;} String_to_PC("\r\n");


/*****************************************************************************/
#define waiting_for_I2C_master \
TWCR = (1 << TWEA) | (1 << TWEN);\
while (!(TWCR & (1 << TWINT)));\
TWDR;


/*****************************************************************************/
#define clear_I2C_interrupt \
TWCR = (1 << TWINT);


