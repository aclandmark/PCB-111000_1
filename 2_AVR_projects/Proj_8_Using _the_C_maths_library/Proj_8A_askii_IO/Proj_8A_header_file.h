

#include "../../Resources_V15/Header_files/Basic_HW_setup.h"
#include "../../Resources_V15/Subroutines/Basic_HW_plus_Timer.c"
#include "../../Resources_V15/Subroutines/Basic_PC_comms.c"
#include "../../Resources_V15/I2C_Subroutines/I2C_subroutines_1.c"
#include "../../Resources_V15/I2C_Subroutines/I2C_slave_Rx_Tx.c"
#include "../../Resources_V15/Subroutines/scanf_and_printf_projects_UNO.c"

int uart_getchar_local(FILE *mystr_input);					//accepts any char
int uart_getDecimal_local(FILE *mystr_input);				//Ignores non-Decimal chars
int uart_putchar_local(char c, FILE *mystr_output);