
volatile char   Return_key, data_counter;
char  digits[8];

#include "Resources_V15/Header_files/Basic_HW_setup.h"
#include "Resources_V15/Subroutines/Basic_HW_plus_Timer.c"
#include "Resources_V15/I2C_Subroutines/I2C_subroutines_1.c"
#include "Resources_V15/I2C_Subroutines/I2C_subroutines_2.c"
#include "Resources_V15/Subroutines/Basic_PC_comms.c"
#include "Resources_V15/Header_files/I_O_from_switches_UNO.h"		//Responds to switch activity
#include "Resources_V15/I2C_Subroutines/I2C_slave_Rx_Tx.c"



/*
#include "Resources_V15/Subroutines/Data_to_from_EEPROMs.c"
#include "Resources_V15/Subroutines/Random_and_prime_nos.c"
#include "Resources_V15/I2C_Subroutines/I2C_diagnostic.c"
*/




void int_plus_short_from_IO(void);
void int_arithmetic_sw2_isr(void);
void enter_number(void);

#define Op_1 'P'
#define Op_2 'R'
#define inc_op  if (Op=='P') Op='R'; else Op='P';


#define message_1 \
"\r\n\
INTEGER NUMBER Ops\r\n\
Press sw1 for roots or sw2 for powers\r\n\
repeatedly press sw3 to scroll through the digits\r\n"

#define message_2 \
"when final digit is being pressed press sw1 before releasing sw3 then release sw1\r\n\
press sw3 for next number (only one digit is allowed)\r\n"


#define message_3 \
"release sw3 and root or power will be calculated\r\n\
pulse sw3 to repeat calculation\r\n\
hold sw1 or sw2 down while pulsing sw3 then press it again to reset the device\r\n"


#define User_instructions \
String_to_PC(message_1);\
String_to_PC(message_2);\
String_to_PC(message_3);
