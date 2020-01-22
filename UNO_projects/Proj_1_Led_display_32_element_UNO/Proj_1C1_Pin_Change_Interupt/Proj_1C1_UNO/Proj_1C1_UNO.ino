
/*Proj_1C1_Pin_Change_Interupt
**************************************************************

A SLIGHT VARIATION ON PROGRAM 3 THE REACTION TIME TESTER.
INTRODUCES the statement "if(m == 1)" which means execute the next statement if m equals 1, and
if it equals anything else skip the next statement.
*/



#include "Proj_1C1_header_file.h"



volatile unsigned int PORT_1, mask;      
char switch_control;                        //Prevents rapid switch_3 presses from shooting leds down at random

int main (void){
setup_HW;
config_sw3_for_PCI;                  

mask = 0xFFFF;                              //0xFFFF = 0b1111111111111111 indicating that none of the leds have yet been shot down
switch_control = 0;
sei();                            
while(mask){            
PORT_1=1;             

for(int m = 1; m < 17; m++){    
if(m == 1)switch_control = 0;                //Set switch_control to zero at the start of each sweep
I2C_Tx_2_integers
(PORT_1 & mask, (~mask) ^ PORT_1);
 
Timer_T0_10mS_delay_x_m(10);    
PORT_1 = (PORT_1 << 1);           
}}I2C_Tx_2_integers(0, 0xFFFF); 
Timer_T0_10mS_delay_x_m(100);          
SW_reset;}



/**************Routine executed every time that switch is  operated**************************************/
                  
ISR(PCINT0_vect) {          
if(switch_3_up)return;         
if (switch_control) return;               //Return early if "switch_control" is 1
mask &= ~PORT_1;          
switch_control = 1;}                      //Set switch conrol to 1.
