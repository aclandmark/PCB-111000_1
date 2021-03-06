

#include "../../Resources_V15/Header_files/Basic_HW_setup.h"
#include "../../Resources_V15/Subroutines/Basic_HW_plus_Timer.c"
#include "../../Resources_V15/I2C_Subroutines/I2C_subroutines_1.c"
#include "../../Resources_V15/Subroutines/Basic_PC_comms.c"
#include "../../Resources_V15/Header_files/clock_stop_watch.h"
#include "../../Resources_V15/I2C_Subroutines/I2C_subroutines_2.c"
#include "../../Resources_V15/I2C_Subroutines/I2C_slave_Rx_Tx.c"
#include "../../Resources_V15/Header_files/I_O_from_switches_UNO.h"		

void Inc_display(void);
void set_time(void);

#define message_1 \
"\r\nSTAND-a-LONE CLOCK\r\n\
Use sw1 and 2 to enter time (24Hr clock).\r\n\
\r\n\
Press sw2 to toggle the display on and off\r\n"

#define message_2 \
"Press sw1 to pause or resume the clock\r\n\
To adjust the clock:\r\n"

#define message_3 \
"pulse sw3 then press sw1 and sw2 to advance the time\r\n\
or press and hold sw3 (for 500ms)\r\n\
then press sw1 and sw2 to retard the time\r\n\
Always pulse sw3 when the time is correct\r\n"

#define User_instructions \
String_to_PC(message_1);\
String_to_PC(message_2);\
String_to_PC(message_3);
