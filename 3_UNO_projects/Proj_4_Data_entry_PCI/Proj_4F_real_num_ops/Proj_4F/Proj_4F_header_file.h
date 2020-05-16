volatile char   Return_key_press;
char  digits[8];
volatile signed char expnt;

void Two_URNs_from_IO(void);
void RN_arithmetic_sw1_and_sw3_isr(void);
void RN_arithmetic_sw2_isr(void);


#define Op_1 'M'		//multiply positive real numbers
#define Op_2 'D'		//divide positive real numbers


#include "Resources_V15/Header_files/Basic_HW_setup.h"
#include "Resources_V15/Subroutines/Basic_HW_plus_Timer.c"
#include "Resources_V15/I2C_Subroutines/I2C_subroutines_1.c"
#include "Resources_V15/I2C_Subroutines/I2C_subroutines_2.c"
#include "Resources_V15/Subroutines/Basic_PC_comms.c"
#include "Resources_V15/Header_files/I_O_from_switches_UNO.h"	
#include "Resources_V15/I2C_Subroutines/I2C_slave_Rx_Tx.c"



#define message_1 \
"\r\n\
REAL NUMBER OPs Press SW1 to select  divide or SW2 to select multiply and intialise display\r\n\
Hold SW1 down to scroll through the digits:\r\n"

#define message_2 \
"Pulse SW3 down to select 'P' the symbol used to select a decimal point\r\n\
Press SW2 to select the digit\r\n\
Repeat for second digit (only select P once per number)\r\n"


#define message_3 \
"Decimal digits flash\r\n\
Press sw3 to enter the real number\r\n\
Enter a second number\r\n\
Pulse SW3 to perform Op_1 or restore the input data\r\n"

#define message_4 \
"To reset the device\r\n\
when an answer is being displayed press sw1 or sw2 before sw3 then \r\n\
release sw3 afer sw1 or 2\r\n\
Return to start\r\n"

#define User_instructions \
String_to_PC(message_1);\
String_to_PC(message_2);\
String_to_PC(message_3);\
String_to_PC(message_4);
