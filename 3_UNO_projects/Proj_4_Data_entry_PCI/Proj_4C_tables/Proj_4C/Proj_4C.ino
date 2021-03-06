
/*Proj_4C_tables
*******************************************/


/*IT INTRODUCES  An aid to learning the multiplication tables

USER INSTRUCTIONS
Use sw1 to enter multiplicand
Use sw2 to shift display left
When number is correct press SW3
Press sw1 and sw2 alternatively to generate multiplication table
hold sw2 down to decrement table
press sw3 before sw1 to re-initiate display

Note:
Switch location SW1(PD2) - SW2(PD7) – SW3(PB2)*/



#include "Proj_4C_header_file.h"


int main (void){
long num;
setup_UNO;
User_prompt;
String_to_PC ( message_1);
String_to_PC ( message_2);

while(1){
num = number_from_IO();                                       //Line A. Wait here until data entry is complete
Num_to_PC(10,num);                                            //Display number on PC
newline();
if(num <0) {num*= -1; if(num < 10)digits[1]=0;}               //Ignore negative numbers
while(num>=100) {num/=10; shift_digits_right;}                //Numbers over 100 not allowed    
while(switch_3_down);                                         //wait for key release (Switch 3)
I2C_Tx_Initiate_tables(digits);                               //update the display

while(1){ 
while(switch_1_up);if(switch_3_down)break;                    //Wait for sw1 key press & exit if sw3 has been pressed
while(switch_1_down){                                         //While sw1 is pressed scroll through table until it is released
I2C_Tx_Inc_tables(digits);
Timer_T0_10mS_delay_x_m(25);}
while(switch_2_up);                                           //wait for sw2 keypress
I2C_Tx_Inc_tables(digits);                                    //Update display
Timer_T0_10mS_delay_x_m(25);

while(switch_2_down){                                         //While sw2 remains pressed scroll back through table until it is released
I2C_Tx_dec_tables(digits);
Timer_T0_10mS_delay_x_m(25);}

while(switch_2_down);}                                        //wait for switch 2 to be released
while(switch_3_down);}}                                       //wait for all switched to be released  




/*****************************************************************************************************************************************/
ISR(PCINT2_vect){Tables_sw1_and_2_isr();}
ISR(PCINT0_vect){Tables_sw3_isr();}




/*****************************************************************************************************************************************/
long number_from_IO(void){
char copy_of_SREG;
config_sw1_and_sw2_for_PCI;                                 //Enable pci on sw1 and sw2
Init_display_for_pci_data_entry;
copy_of_SREG = SREG;                                        //Save interrupt state
sei();                                                      //Enable interrupts
while(!(Return_key));                                       //Line B wait here. Jump to  sw1/2_ISR to enter a number 
Return_key = 0;                                             //Jump to sw3_ISR for the return key then clear return key
SREG = copy_of_SREG;                                        //restore original interrupt state
disable_pci_on_sw3;                                         //disable pci on sw3 (prevents sw bounce)
return I2C_displayToNum();}                                 //get number from display and return




/*****************************************************************************************************************************************/
void Tables_sw1_and_2_isr(void){                            //Jump here from line B
Timer_T0_10mS_delay_x_m(5);                                 //switch bounce delay
while(switch_1_down){                                       //while sw1 is down scroll through the digits
switch(digits[0]){                                          //test digit[0]
case '9': if(!(digits[1]))digits[0] = '-';                  //if it is 9 and this is the first digit scroll on to "-" 
else digits[0] = '0';break;                                 //otherwise scroll onto 0
case '-': digits[0] = '0';break;                            //if it is "-" scroll onto 0
default: digits[0]++; break;}                               //otherwise increment the digit
I2C_Tx_8_byte_array(digits);                                //update the display
Timer_T0_10mS_delay_x_m(25);}                               //return to line B when sw1 is released

if(switch_2_down) {                                         //if sw2 is pressed
shift_digits_left; digits[0] = '0';                         //shift the display left and re-initialise digit[0]
I2C_Tx_8_byte_array(digits);}                               //update display, 
config_sw3_for_PCI;                                         //enable pci on sw3 
clear_PCI_on_sw1_and_sw2;}                                  //clear spurious interrupts due to switch bounce and return to line B



/*****************************************************************************************************************************************/
void Tables_sw3_isr(void){                                  //Jump here from line B
char disp_bkp[8];                                           //sw3_ISR is delayed until sw1 has been released
if(switch_3_down) return;                                   //Sense key release only, ignore key press.

for(int m = 0; m<=7; m++){disp_bkp[m]=digits[m];            //Blink the display
digits[m]=0;}I2C_Tx_8_byte_array(digits);
Timer_T0_10mS_delay_x_m(25);
for(int m = 0; m<=7; m++)
{digits[m]=disp_bkp[m];}I2C_Tx_8_byte_array(digits);
disable_pci_on_sw1_and_sw2;                                 //disable pci on sw1/2
Return_key=1;}                                              //Set the return key and return to line B
