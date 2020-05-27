#include "Proj_9B_header_file.h"
int main (void){                         
unsigned char OSCCAL_mini_OS;
int error_mag;

setup_UNO;
User_prompt;
I2C_Tx_initiate_mode('R');
String_to_PC("\r\nPCB_A (mini_OS) device calibrating");
waiting_for_I2C_master;  
OSCCAL_mini_OS = receive_byte_with_Ack();
error_mag = receive_byte_with_Ack() << 8;
error_mag += receive_byte_with_Nack();
clear_I2C_interrupt;
String_to_PC("\r\nOSCCAL user value   "); Num_to_PC(10,OSCCAL_mini_OS);
String_to_PC("\r\ncalibration error  "); Num_to_PC(10,error_mag);
if (error_mag < 750) String_to_PC("  OK\r\n");
/*********************************/

}
