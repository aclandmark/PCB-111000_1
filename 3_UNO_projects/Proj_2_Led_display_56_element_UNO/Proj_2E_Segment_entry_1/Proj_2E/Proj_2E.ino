  
/*Proj_2E_Segment_entry_1 and Proj_2F_Segment_entry_2
*******************************************************************************/

/*DISPLAYING NUMBERS USING MANUAL ILLUMINATION OF SEGMENTS


INTRODUCES


1.  Use of "isCharavailable()" and "receiveChar()" in place of "waitforkeypress()" when waiting for 
keyboard entry so that the watch dog timer can be implemented.

A typical implementation is illustrated in the following program segment:  

    while(1){if(isCharavailable(10)){keypress = receiveChar();break;}}  

Inclusion of a "wdr();" statement in "isCharavailable(10)" 
enables the watch dog timer to be used.

Note: 
"waitforkeypress()" is really only suitable to simplify examples.  It pauses program execution 
indefinitely and prevents use of the watch dog timer which would time out and generate an unwanted reset.
The WDT is not implemented in the mini-OS for these projects as it is considered that this would 
introduce unnecessary complexity. 

3.  SW_reset_with_interrupt:  This is a "SW_reset" combined with a WDT ISR.  The ISR writes to an 
EPROM location which the main program reads shortly after every reset. It can there distinguish
betweeen WDTouts and other resets. 
*/


#include "Proj_2E_header_file.h"


int main (void){

char letter = 0, digit_num;           

setup_UNO;
config_sw1_and_sw2_for_PCI;

if (Arduino_non_WDTout)
  {User_prompt;
  String_to_PC("Press key a to g (x for next digit)\r\n\
Press SW1 for WDTout with interrupt");}

else 
  {clear_Arduino_WDT_flag
  String_to_PC("  ?");                                          //Abbreviated user prompt
  I2C_Tx_any_segment('h', 0);}                                  //Re-establish display

sei();
Thirty_mS_watch_dog_with_interrupt;


while(1){digit_num=0;
while(digit_num < 8){
if(isCharavailable(4))                                //Wait up to 60mS for a key press.  In the absence of one
{switch(letter = receiveChar()){                            //the program reverts back to "while(digit_num < 8){"
case 'a': case 'A':                                         //otherwise it continues and updates the display
case 'b': case 'B':                                         //Program execution is not put on hold awaiting a keypress
case 'c': case 'C':
case 'd': case 'D':
case 'e': case 'E':
case 'f': case 'F':
case 'g': case 'G': I2C_Tx_any_segment(letter, digit_num); break;
case 'x': case 'X': digit_num += 1;break;
default: break;}}}}}





/***********************Local version of "isCharavailable()" that includes wdr();**********/


char isCharavailable_Local (char m){int n = 0;
while (!(UCSR0A & (1 << RXC0))){n++;                        //Increments "n" from zero to 8000
if (n>8000) {m--;n = 0;wdr();}if (m == 0)return 0;}         //Checks the receiver at each increment
return 1;}                                                  //Returns a 1 as soon as a character is received
                                                            //If no character is sent it pauses a while (10m mS)
                                                            //and eventually returns zero.
                                                            //Note: this subroutine enables us to reset the WDT (wdr();)
                                                            //in a way that is not possible with "waitforkeypress()"

/***********************************************************************/

ISR(PCINT2_vect) 
{if (switch_1_down) {sei(); while(1);}}



/*****************************************************************/
ISR (WDT_vect){set_Arduino_WDTout;}  



                                     
