
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>



#include "Resources_PP_UNO\ATMEGA_Programmer.h"
#include "Resources_PP_UNO\ATMEGA_Programmer_HW_subs.c"
#include "Resources_PP_UNO\Basic_IO_and_Timer_subs.c"
#include "Resources_PP_UNO\ASKII_subroutines.c"
#include "Resources_PP_UNO\Flash_Programmer_subs.c"
#include "Resources_PP_UNO\EEPROM_programmer_sub.c"


unsigned char receive_byte_with_Ack(void);
unsigned char receive_byte_with_Nack(void);

/*****************************************************************************/
#define waiting_for_I2C_master \
TWCR = (1 << TWEA) | (1 << TWEN);\
while (!(TWCR & (1 << TWINT)));\
TWDR;


/*****************************************************************************/
#define clear_I2C_interrupt \
TWCR = (1 << TWINT);


/*****************************************************************************/
#define set_up_I2C \
TWAR = 0x02;
