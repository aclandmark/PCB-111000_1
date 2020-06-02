

/**************Proj_2A_LED_display***************************
Reaction time tester*****************************************/

/*Press the middle switch to start the display and release
it as soon as the LEDS line up to make them flash*/




#include "Proj_2A_header_file.h"



int main (void){
unsigned int rand;                      //random number
unsigned int PORT_1, PORT_2;

setup_UNO_extra;
while(switch_2_up);                     //wait for switch 3 to be pressed before starting


PORT_1=0b0000000000000001; 
PORT_2=0b1000000000000000;                                    
while (1){                              //Infinite loop that could be replaced by a "SW_reset"                                      

while(1){                                //Infinite loop. 
Timer_T0_10mS_delay_x_m(1);              //The "break" statement is used to exit this loop
while(switch_2_up);                       //halt if switch 3 is released
rand = (PRN_16bit_GEN(0))%3 + 1;          //generate a random number (1,2 or 3)
for (int m = 0; m < rand; m++)            //set up a loop   
{if (PORT_1 == 0b1000000000000000)        //Either reinitialise PORT_1 
PORT_1 = 0b0000000000000001;              //or
else PORT_1 = PORT_1 << 1; }              //prepare to shift the top row of the display left    
if (PORT_2 == 0b1000000000000000)         //by 1, 2 or 3 places
PORT_2 = 0b0000000000000001;              //prepare to shift the lower row of the   
else PORT_2 = PORT_2 << 1;                //display left but only by one place

I2C_Tx_2_integers(PORT_1, PORT_2);         //update the display

if(PORT_1 == PORT_2)                      //If a single vertical line in illuminated
{Timer_T0_10mS_delay_x_m(35);              //start a timer and pause
if(switch_2_up)break;                     //If switch 3 has been released "break" out of the while loop
else while(switch_2_down);}               //else wait for it to be released

if(PORT_1 != PORT_2)                      //If disjointed segments are illuminated
{Timer_T0_10mS_delay_x_m(8);}}            //momentarily pause before returning  
                                          //to the top of the while loop
while(switch_2_up){
I2C_Tx_2_integers(PORT_1, 0);             //flash the display until the switch is up
Timer_T0_10mS_delay_x_m(16);
I2C_Tx_2_integers(0, PORT_2);
Timer_T0_10mS_delay_x_m(16);}}}           //When it is pressed again return to the outer while loop
