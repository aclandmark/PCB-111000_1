

#include "Resources_V15/Header_files/Basic_HW_setup.h"
#include "Resources_V15/Subroutines/Basic_HW_plus_Timer.c"
#include "Resources_V15/I2C_Subroutines/I2C_subroutines_1.c"
#include "Resources_V15/Subroutines/Basic_PC_comms.c"
#include "Resources_V15/Subroutines/Random_and_prime_nos.c"
#include "Resources_V15/I2C_Subroutines/I2C_subroutines_2.c"
#include "Resources_V15/Header_files/I_O_from_switches_UNO.h"
#include "Resources_V15/I2C_Subroutines/I2C_slave_Rx_Tx.c"
#include "Resources_V15/Subroutines/Integers_to_from_PC.c"



#include "Resources_V15/Subroutines/Data_to_from_EEPROMs.c"
#include "Resources_V15/I2C_Subroutines/I2C_diagnostic.c"



#define line_control {if(line_counter==4) {line_counter = 0;newline();}\
 else {Char_to_PC('\t');line_counter++;}}

#define User_instructions \
String_to_PC(message_1);\
String_to_PC(message_2);

#define message_1 \
"\r\nUses a modified prime number generator that includes an percentage of non_prime numbers\r\n\
Review the numbers and try to guess the non-prime ones\r\n"
#define message_2 \
"Type them in terminating each with a -return- keypress\r\n\
Enter zero to exit and press sw3 when requested\r\n"
