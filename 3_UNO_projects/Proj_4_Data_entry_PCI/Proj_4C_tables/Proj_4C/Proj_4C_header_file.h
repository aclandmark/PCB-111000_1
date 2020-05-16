volatile char   Return_key;
char  digits[8];

#include "Resources_V15/Header_files/Basic_HW_setup.h"
#include "Resources_V15/Subroutines/Basic_HW_plus_Timer.c"
#include "Resources_V15/I2C_Subroutines/I2C_subroutines_1.c"
#include "Resources_V15/I2C_Subroutines/I2C_subroutines_2.c"
#include "Resources_V15/Subroutines/Basic_PC_comms.c"
#include "Resources_V15/Header_files/I_O_from_switches_UNO.h"		
#include "Resources_V15/I2C_Subroutines/I2C_slave_Rx_Tx.c"

long number_from_IO(void);
void Tables_sw1_and_2_isr(void);
void Tables_sw3_isr(void);


#define message_1 \
"\r\n\
TABLES Use sw1 to enter multiplicand\r\n\
Use sw2 to shift display left\r\n\
When number is correct press SW3\r\n\
Press sw1 and sw2 alternatively\r\n\
to generate multiplication table.\r\n"

#define message_2 \
"Hold sw1 down to incement the table\r\n\
and sw2 down to decrement it.\r\n\
press sw3 before sw1 to re-initiate display\r\n"
