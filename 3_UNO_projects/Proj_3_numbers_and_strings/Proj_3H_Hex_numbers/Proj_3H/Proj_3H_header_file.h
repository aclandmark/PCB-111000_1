

#include "Resources_V15/Header_files/Basic_HW_setup.h"
#include "Resources_V15/Subroutines/Basic_HW_plus_Timer.c"
#include "Resources_V15/I2C_Subroutines/I2C_subroutines_1.c"
#include "Resources_V15/I2C_Subroutines/I2C_subroutines_2.c"
#include "Resources_V15/Subroutines/Basic_PC_comms.c"
#include "Resources_V15/I2C_Subroutines/I2C_slave_Rx_Tx.c"
#include "Resources_V15/Header_files/I_O_from_switches_UNO.h"

void NumericString_to_PC_local(char*);
void Local_BtoA(long,char*);
void Binary_to_PC_local(int);
