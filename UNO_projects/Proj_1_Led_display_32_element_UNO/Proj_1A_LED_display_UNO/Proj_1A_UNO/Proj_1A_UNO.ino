
/*Proj_1A_LED_display
***************************************************/


 

#include "Proj_1A_header_file.h"          //Provides details of resources provided by the complier and project_SW
                                          //including "setup_HW", the I2C subroutine and "SW_reset"


int main (void){                          //The "main" routine lies between a pair of {} brackets.

unsigned int PORT_1=1;                    //PORT_1 is an address in data memory initially set to 1 (binary 0000000000000001)
                                          //Note: that PORT_1 holds 16 bits (binary digits) of data 
setup_HW;         

  for(int m = 1; m < 17; m++){            //Code between the {} brackets is repeated 16 times             
                                          //as m increments from 1 to 16  (m++ is shorthand for increment m)
    I2C_Tx_2_integers(PORT_1, PORT_1);        

    Timer_T0_10mS_delay_x_m(6);            //This subroutine generates a 10mS delay that is repeated 6 times   
    PORT_1 = PORT_1 << 1;                 //The contents of address "PORT_1" are shifted one place left
    }SW_reset;                            //every time that the "for-loop" is executed
}
