

#include "Resources_V15/Header_files/Basic_HW_setup.h"
#include "Resources_V15/Subroutines/Basic_HW_plus_Timer.c"
#include "Resources_V15/I2C_Subroutines/I2C_subroutines_1.c"
#include "Resources_V15/I2C_Subroutines/I2C_subroutines_2.c"
#include "Resources_V15/Subroutines/Basic_PC_comms.c"
#include "Resources_V15/I2C_Subroutines/I2C_slave_Rx_Tx.c"
#include "Resources_V15/Subroutines/Integers_to_from_PC.c"	
#include "Resources_V15/Subroutines/Random_and_prime_nos.c"
#include "Resources_V15/Header_files/I_O_from_switches_UNO.h"

void fraction_to_decimal_string_Local(long, long, char*);
long product_AB(long, long);
long divide_A_by_B(long, long);
long A_modulo_B(long, long);
