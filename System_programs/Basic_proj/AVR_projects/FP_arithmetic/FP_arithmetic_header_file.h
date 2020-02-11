#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdio.h>

#include "FP_arithmetic_subroutines.c"


void USART_init (unsigned char, unsigned char);
char receiveChar(void);
char isCharavailable (char);
char decimal_digit (char);
void Timer_T0_10mS_delay_x_m(int);
void I2C_Tx_LED_dimmer(void);

int uart_putchar(char c, FILE *mystr_output);
int uart_getchar(FILE *mystr_input);				//accepts any char
int uart_getDouble(FILE *mystr_input);				//Ignores non-double chars

FILE  uart_output = FDEV_SETUP_STREAM (uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE mystdin = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);
FILE uart_input_Double = FDEV_SETUP_STREAM(NULL, uart_getDouble, _FDEV_SETUP_READ);

char MCUSR_copy;



#define setup_HW \
setup_watchdog;\
set_up_I2C;\
ADMUX |= (1 << REFS0);\
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
for(int m = 0; m < 4; m++)printf("\r\n");\
printf("Project commentary: Press 'X' to escape or AOK\r\n");\
\
eeprom_write_byte((uint8_t*)0x3F4,0x41);}\
\
if ((eeprom_read_byte((uint8_t*)0x3F4) & 0x40)){\
eeprom_write_byte((uint8_t*)0x3F4,\
(eeprom_read_byte((uint8_t*)0x3F4) | 0x80));\
\
asm("jmp 0x6C60");}


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



