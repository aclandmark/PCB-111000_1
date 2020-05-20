
/*Proj_2H_Numerical_entry with dimmer control
******************************************************************************/


/*AN ALMOST EXACT COPY OF Proj_2G    


IT INTRODUCES 

1.    Subroutine "isCharavailable_Local".
      This can be used in place of "waitforkeypress()" when the watch dog timer is required


2.    The Program Segment

      do{if(isCharavailable_Local (10))digit = receiveChar();       
      else continue;
      .................} while(...);  

Note:
"isCharavailable_Local()" returnes 1 if a keypress is made and zero if it is not
"isCharavailable_Local ()" includes the statement "wdr();"
If no keypress is made the "continue" statement returns program execution back to the top of the "do-loop".

3.    Use of a switch to adjust display brightness

4     Use of a WDTout with interrupt that uses the EEPROM to record the use of thw WDT to reset the device


Note:
Following the WD time out "digit_num" is reset to zero.  Digits already illuminated will be over written 
or blanked by new keypresses. The WDT ISR could be used to save the value of "digit_num" to EEPROM to avoid 
this happening if required.*/




#include "Proj_2H_header_file.h"
 
 
char Dimmer_control_local;                                      //Variable "Dimmer_control" also used by the "setup_UNO" macro

int main (void){
char  digit;                                                    //Saves numerical keypress made by the user
const char* string_ptr = 0;                                     //Points to strings used to define zero, one, two etc..  (see headrer file)
volatile int digit_num=0;                                       //Identifies which digit in the display will be illuminated 


setup_UNO;
config_sw1_and_sw2_for_PCI;
config_sw3_for_PCI;

if(Char_from_EEPROM(0x3F1))                                     //Program start up due to POR or post programming
{User_prompt;
String_to_PC("Send digits:\r\n\
Press sw_3 to change display brightness\r\n\
sw1 to generate 1 second watchdog timeout");}

else                                                            //Reset due to watch dog timeout.
{String_to_PC("\r\nWatch-Dog Timeout");
I2C_Tx_any_segment('h', 7);                                     //Restore display
Char_to_EEPROM( 0x3F1, 0xFF);}                                  //Reset WDTout entry in EEPROM

One_sec_WDT_with_interrupt;                                     //Initialise WDT with interrupt
digit = '0';                                                    //Initialise digit

while(1){
Dimmer_control_local = 0;                                       //Leave display brightness unchanged
digit_num = 0;  
sei();

do{                                                             //"isCharavailable_Local()" continuously resets the WD timer
if(isCharavailable_Local (10))digit = receiveChar();            //Probably all interrupts will occur when isCharavailable(10) is executing      
else continue;                                                  //Program flow jumps from here back to the start of the "do-loop"                     
                                                                //until a character is received from the keyboard
switch(digit){                        
case '0': string_ptr = zero; break;                             //"string_ptr" takes the address of the first letter of string "zero"    
case '1': string_ptr = one; break;
case '2': string_ptr = two; break;
case '3': string_ptr = three; break;
case '4': string_ptr = four; break;                             //"string_ptr" takes the address of the first letter of string "four"
case '5': string_ptr = five; break;
case '6': string_ptr = six; break;
case '7': string_ptr = seven; break;
case '8': string_ptr = eight; break;
case '9': string_ptr = nine; break;
default: continue; break;}                                       //Any character other than 0-9 sends program flow back to the start of the do-loop                    

display_num_string(string_ptr, digit_num);                       //Illuminate the digit
digit_num++;}                                                    //Get ready to illuminate next digit
while (digit_num < 8);                                           //Exit "do-loop" as soon as display is completely illuminated

while(1){

if(isCharavailable_Local (10))break;}                           //continuously resets WD timer.
digit = receiveChar();
I2C_Tx_any_segment_clear_all();}}                               //clear display and wait for new keypresses



/****************************************************************************************************/
void display_num_string (const char* s, int digit_num){         //"s" takes address of first letter of number string   
int seg_ptr=0;                                                  //Increments "s" so that subsequent letters can be read          
char letter;
while(1){
letter = (s[seg_ptr]);                                          //"letter" takes contents of location "s + seg_ptr"
switch(letter){
case 'a': case 'b': case 'c': 
case 'd': case 'e': case 'f': 
case 'g': I2C_Tx_any_segment(letter, digit_num);break;          //Illuminate segments one at a time
case 0:  return; break;
default: break;}seg_ptr++;}}         


/****************************************************************************************************/
char isCharavailable_Local (char m){int n = 0;                    //Contains a "wdr()" command
while (!(UCSR0A & (1 << RXC0))){n++;                              //Increments "n" from zero to 8000
if (n>8000) {m--;n = 0;wdr();}if (m == 0)return 0;}               //Checks the receiver at each increment
return 1;}                                                        //Returns a 1 as soon as a character is received
                                                                  //If no character is sent it pauses a while (15m mS)
                                                                  //and eventually returns zero.
                                                                  //Note: this subroutine enables us to reset the WDT (wdr();)
                                                                  //in a way that is not possible with "waitforkeypress()"


/****************************************************************************************************/
ISR(PCINT0_vect) {                                                //This ISR momentarily interrupts the main routine
if(switch_3_up)return;

while(switch_3_down)
{Dimmer_control_local = (Dimmer_control_local + 1)%4;             //Set variable "Dimmer_control_local2 to 0, 1, 2 or 3
if(!(Dimmer_control_local))continue;                              //skip the zero value
I2C_Tx(1, 'Q', &Dimmer_control_local);                            //Adjust led brightness
I2C_Tx_any_segment('h', 7);                                       //Restore display
Timer_T0_10mS_delay_x_m(70); wdr();}}                             //Reset watch dog timer 


/*********************************************************************************************************/
ISR(PCINT2_vect){
if(switch_1_down){sei(); while(1); }}                              //Generate WD time out


/*********************************************************************************************************/
ISR (WDT_vect){                                         
Char_to_EEPROM( 0x3F1, 0);}                                  
