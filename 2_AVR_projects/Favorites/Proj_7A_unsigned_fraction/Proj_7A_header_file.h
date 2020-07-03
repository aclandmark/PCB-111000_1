

#include "../../Resources_V15/Header_files/Basic_HW_setup.h"
#include "../../Resources_V15/Subroutines/Basic_HW_plus_Timer.c"
#include "../../Resources_V15/I2C_Subroutines/I2C_subroutines_1.c"
#include "../../Resources_V15/Subroutines/Basic_PC_comms.c"
#include "../../Resources_V15/I2C_Subroutines/I2C_subroutines_2.c"

#include "../../Resources_V15/Subroutines/Integers_to_from_PC.c"

#include "../../Resources_V15/Header_files/I_O_from_switches_UNO.h"
#include "../../Resources_V15/I2C_Subroutines/I2C_slave_Rx_Tx.c"

#include "../../Resources_V15/Subroutines/Arithmetic.c"
#include "../../Resources_V15/Subroutines/Floating_point_arithmetic.c"
#include "../../Resources_V15/Subroutines/Real_numbers_to_from_PC.c"

unsigned long Fraction_to_Binary_Local(unsigned long,unsigned long);
unsigned long Binary_points_to_Decimal_Local (unsigned long);
void Decimal_to_PC_local(char, unsigned long, int);

