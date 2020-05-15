

#include "Resources_V15/Header_files/Basic_HW_setup.h"
#include "Resources_V15/Subroutines/Basic_HW_plus_Timer.c"
#include "Resources_V15/I2C_Subroutines/I2C_subroutines_1.c"
#include "Resources_V15/Subroutines/Basic_PC_comms.c"
#include "Resources_V15/I2C_Subroutines/I2C_slave_Rx_Tx.c"


long AtoB_local(const char*);
void BtoA_local(long,char*);
void NumericString_to_PC_local(const char*);
//void NumericString_to_display_local(const char*);
void NumericString_to_display_R_local(const char*);

