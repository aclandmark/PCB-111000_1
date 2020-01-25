


/*Proj_1B and Proj_1B1_LED_display
**********************************************************/



#include "Proj_1B_header_file.h"



int main (void){

unsigned int PORT_1=1;                                //PORT_1 defines the display and initialises it
char m=1;                                             //m  is only active for the first 5 cycles of the "while-loop"
char overflow=0;                                     //overflow is set to 1 when the most left hand leds are illuminated
                
setup_HW;
while(1){ 
    
    
  I2C_Tx_2_integers(PORT_1, ~PORT_1);                 //the ~ symbol inverts the ones and zeros
  Timer_T0_10mS_delay_x_m(6);                         //Can be replaced with waitforkeypress();     
  //waitforkeypress();
  if (m<=5){PORT_1 |= (PORT_1 << 1);m += 1;}          //m += 1; is shorthand for m = m+1;     
  else PORT_1 = PORT_1 << 1;                          //once "m" is 6 simply shift the display left       
  if(overflow)PORT_1 |= 1;                            //if overflow is 1 execute "PORT_1 |= 1;".
  if (PORT_1 & 0x8000) overflow=1;                    //0x8000 = binary 1000000000000000
  else overflow = 0;
  }                                                   //if PORT_1 is greater than or equal to 0x8000 
  }                                                   //place 1 in its least significant bit
                                                      //this effectively cycles the display round and round 
    
