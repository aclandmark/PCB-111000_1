

#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>



#include "Resources\Atmega328_app.h"
#include "Resources\Basic_IO_and_timer_subs.c"



const char * Device_95 = "328/P";
const char * Device_94 = "168/P";
const char * Device_93 = "88/P";
const char * Device_92 = "48/P";

const char * Device_ptr[4];
int Device;
