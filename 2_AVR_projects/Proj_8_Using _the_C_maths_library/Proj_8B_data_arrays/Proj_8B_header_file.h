

#include "../../Resources_V15/Header_files/Basic_HW_setup.h"
#include "../../Resources_V15/Subroutines/Basic_HW_plus_Timer.c"
#include "../../Resources_V15/Subroutines/Basic_PC_comms.c"
#include "../../Resources_V15/I2C_Subroutines/I2C_subroutines_1.c"
#include "../../Resources_V15/I2C_Subroutines/I2C_slave_Rx_Tx.c"

#include "../../Resources_V15/Subroutines/scanf_and_printf_projects_UNO.c"



/*******Define input streams*****************/
FILE  uart_output = FDEV_SETUP_STREAM (uart_putchar, NULL, _FDEV_SETUP_WRITE);
int uart_getDouble_from_csv(FILE *mystr_input);	
FILE uart_input_any_char = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);
FILE uart_input_Double_from_csv = FDEV_SETUP_STREAM(NULL, uart_getDouble_from_csv, _FDEV_SETUP_READ);