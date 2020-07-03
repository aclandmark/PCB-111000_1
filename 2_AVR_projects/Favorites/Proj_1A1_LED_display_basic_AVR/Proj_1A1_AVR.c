
#include "Proj_1A1_header_file.h"




int main (void){								//The "main" routine lies between a pair of {} brackets.

unsigned long PORT_1, PORT_2;
 

//CLKPR = (1 << CLKPCE);
//CLKPR = (1 << CLKPS0);

setup_UNO_extra;					

while(1)
{PORT_1=1; PORT_2 = 0x80000000;

	for(int m = 1; m < 33; m++){				//Code between the {} brackets is repeated 16 times 						
												//as m increments from 1 to 16	(m++ is shorthand for increment m)
		I2C_Tx_2_integers(PORT_1, PORT_2);				

		Timer_T1_sub(T1_delay_50ms);			//This subroutine generates a 10mS delay that is repeated 6 times		
		PORT_1 = PORT_1 << 1;					//The contents of address "PORT_1" are shifted one place left
		PORT_2 = PORT_2 >> 1;
		}										//every time that the "for-loop" is executed
}}
