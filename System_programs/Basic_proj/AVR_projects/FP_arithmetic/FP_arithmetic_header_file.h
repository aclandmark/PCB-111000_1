#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdio.h>

void USART_init (unsigned char, unsigned char);
char receiveChar(void);
char isCharavailable (char);
char decimal_digit (char);


int uart_putchar(char c, FILE *mystr_output);
int uart_getchar(FILE *mystr_input);				//accepts any char
int uart_getDouble(FILE *mystr_input);				//Ignores non-double chars

FILE  uart_output = FDEV_SETUP_STREAM (uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE mystdin = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);
FILE uart_input_Double = FDEV_SETUP_STREAM(NULL, uart_getDouble, _FDEV_SETUP_READ);



#define setup_HW \
setup_watchdog; \
ADMUX |= (1 << REFS0);\
MCUCR &= (~(1 << PUD));\
DDRB = 0;\
DDRC = 0;\
DDRD = 0;\
PORTB = 0xFF;\
PORTC = 0xFF;\
PORTD = 0xFF;\
USART_init(0,16);

#define setup_watchdog \
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;

#define wdr()  __asm__ __volatile__("wdr")

