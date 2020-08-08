/*Proj_9C_ATMEGA328_manual_cal
******************************************************************/


/*Like Proj_9A this project generates a print out of calibration error as a function of the calibration
factor OSCCAL and enables the user to choose a value for OSCCAL.


IT INTRODUCES 


Project Subroutine "I2C_Tx(1, 'N', &cal_mode)".
This causes the mini-OS to carry out manual calibration of the ATMEGA 328.

Note:  
Calibration subroutines are not visible to the user but are similar to those used for the ATMEGA 168 
however T2 running under crystal control provides the 7.8125 mS interrupt.

There is no auto_cal_project for the Atmega 328 because this runs automatically under the control 
of the mini-OS following a POR.
*/





char Decimal_from_KBD(void);



#include "Proj_9D_header_file.h"



int main (void){

int error;

setup_UNO;
User_prompt;

String_to_PC("\r\nATMEGA 328 plot cal running \r\b");

I2C_Tx_initiate_mode('M');                                      //Initiate Atmega 328 calibration mode


for(int m = 0x10; m <= 0xF0; m++){
Num_to_PC(10, m ); Char_to_PC('\t');

waiting_for_I2C_master;                                         //Atmega 328 accepts request to calibrate             
error = receive_byte_with_Ack()  << 8;                            //Receive 328 OSCCAL_default Value
error += receive_byte_with_Nack();
clear_I2C_interrupt;                           						//Working value + 20  
Num_to_PC(10, error );
newline();}
//while(1);
//}



SW_reset;}




/*******************************************************************************/

