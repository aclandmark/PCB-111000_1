#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdio.h>


void display_num_string (const char*, int);

void USART_init (unsigned char, unsigned char);
char receiveChar(void);
char isCharavailable(char);
void Timer_T0_10mS_delay_x_m(int);
void I2C_Tx_LED_dimmer(void);
void I2C_Tx_any_segment_clear_all(void);
void I2C_Tx_any_segment(char, char);
void Timer_T1_sub(char, unsigned int);
void Char_to_PC(char);
void String_to_PC(const char*);
char waitforkeypress(void);


char MCUSR_copy;


/*****************************************************************************/

#define T0_delay_10ms 5,178


#define T1_delay_50ms 5,0xFE78
#define T1_delay_100ms 5,0xFCF2
#define T1_delay_250ms 5,0xF85F
#define T1_delay_500ms 5,0xF0BE



/*****************************************************************************/
#define SW_reset {wdt_enable(WDTO_30MS);while(1);}



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
for(int m = 0; m < 4; m++)String_to_PC("\r\n");\
String_to_PC("Project commentary: Press 'X' to escape or AOK\r\n");\
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




/*****************************************************************************/
#define clear_I2C_interrupt \
TWCR = (1 << TWINT);



/*****************************************************************************/
#define set_up_switched_inputs \
MCUCR &= (~(1 << PUD));\
DDRD &= (~((1 << PD2)|(1 << PD7)));\
PORTD |= ((1 << PD2) | (1 << PD7));\
DDRB &= (~(1 << PB2));\
PORTB |= (1 << PB2);





