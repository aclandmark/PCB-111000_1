

#include "Resources_V15/Header_files/Basic_HW_setup.h"
#include "Resources_V15/Subroutines/Basic_HW_plus_Timer.c"
#include "Resources_V15/I2C_Subroutines/I2C_subroutines_1.c"
#include "Resources_V15/Subroutines/Basic_PC_comms.c"
#include "Resources_V15/I2C_Subroutines/I2C_slave_Rx_Tx.c"
#include "Resources_V15/Header_files/I_O_from_switches_UNO.h"
#include "Resources_V15/Subroutines/Data_to_from_EEPROMs.c"

#define Thirty_mS_watch_dog_with_interrupt \
wdr();\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = (1<< WDE) | (1 << WDIE) |  (1 << WDP0);


char isCharavailable_Local (char);
