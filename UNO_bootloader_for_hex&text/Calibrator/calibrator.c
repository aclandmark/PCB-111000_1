
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>

#define OSC_CAL \
eeprom_write_byte((uint8_t*)0x3FD, OSCCAL);\
if ((eeprom_read_byte((uint8_t*)0x3FE) > 0x0F)\
&&  (eeprom_read_byte((uint8_t*)0x3FE) < 0xF0) && (eeprom_read_byte((uint8_t*)0x3FE)\
== eeprom_read_byte((uint8_t*)0x3FF))) {OSCCAL = eeprom_read_byte((uint8_t*)0x3FE);} 

void sendString(char*);
void sendChar(char);
char isCharavailable (int);
char receiveChar(void);

int main (void){

OSC_CAL;
eeprom_write_byte((uint8_t*)0x3FC,	(eeprom_read_byte((uint8_t*)0x3FC) | 0x80));

wdt_enable(WDTO_15MS);
while(1);
return 1;}
