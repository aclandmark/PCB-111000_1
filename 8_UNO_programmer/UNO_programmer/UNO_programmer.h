
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>

const char *Device = "328";

#include "Resources_UNO_programmer\\UNO_Programmer.h"
#include "Resources_UNO_programmer\Device_characteristics.c"
#include "Resources_UNO_programmer\UNO_Programmer_HW_subs.c"
#include "Resources_UNO_programmer\Basic_IO_and_Timer_subs.c"
#include "Resources_UNO_programmer\ASKII_subroutines.c"
#include "Resources_UNO_programmer\Hex_Programmer_subs.c"
#include "Resources_UNO_programmer\EEPROM_programmer_sub.c"
#include "Resources_UNO_programmer\Text_programmer_subs.c"
#include "Resources_UNO_programmer\Hex_Programmer.c"
