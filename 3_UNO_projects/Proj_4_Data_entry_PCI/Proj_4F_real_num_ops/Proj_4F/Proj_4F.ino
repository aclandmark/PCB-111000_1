
/*Proj_4F_real_num_ops
******************************************************************/


/*IT INTRODUCES

A program that receives real numbers entered using the switches and either multiplies or divides them.



USER INSTRUCTIONS

Press SW1 to select divide or SW2 to select multiply and initialise display
Hold SW1 down to scroll through the digits: 
Pulse SW3 down to select 'P' the symbol what is used to select a decimal point
Press SW2 to select the digit
Repeat for second digit (only select P once per number)
Decimal digits flash
Press sw3 to enter the real number
Enter a second number
Pulse SW3 to perform Op_1 or restore the input data

To reset the device
when an answer is being displayed press sw1 or sw3 before sw2 then 
release sw3 after sw1 or 2 
Return to start


Switch location SW3(PD2) - SW2(PB6) - SW1(PD7)*/


#include "Proj_4F_header_file.h"



char  Op = 'M';

int main (void){
setup_UNO;
User_prompt;
User_instructions;

while(1){
  
while(1){if(switch_2_down) {Op = 'M'; break;}               //Press sw2 to select multiplication
    if(switch_1_down) {Op = 'D'; break;}}                   //Press sw1 to select division
while (switch_1_down); while (switch_2_down);               //wait for sw to be released


Two_URNs_from_IO();                                         //acquire to positive real numbers
I2C_Tx_Uarithmetic_OP(Op);                                  //carry out either multiplication or division
while(1){
while(switch_3_up);                                         //wait for sw3 keypress 
if((switch_1_down) || (switch_2_down))break;                //if sw1 or 2 down break
I2C_Tx_Uarithmetic_OP('A');                                 //initialise OS SW 
while(switch_3_down);                                       //wait for sw3 to be released
I2C_Tx_Uarithmetic_OP(Op);}                                 //repeat the arithmetic 

while (switch_1_down); 
while (switch_3_down);
I2C_Tx_any_segment_clear_all();}                                //Clear display
SW_reset;}

/***********************************************************************************************************/
void Two_URNs_from_IO(void){
char copy_of_SREG;            
config_sw1_and_sw2_for_PCI;                                 //Enable pci on sw1 and sw2
config_sw3_for_PCI;                                         //Enable pci on sw3
expnt=0;                                                    //Initialise exponent to zero
initialise_OS_chip_for_arithmetic;                          //set digit[0] to '0'
copy_of_SREG = SREG;                                        //Save the interrupt state
sei();                                                      //Enable interrupts
while (!(Return_key_press));                                //Line B wait here. Jump to sw1/2_isr and sw3_isr to enter a number
expnt=0;                                                    //after receiving first number re-initialise the exponent to zero
while((Return_key_press != 2));                             //wait for second number
Return_key_press = 0;                                       //reset Return_key_press
SREG = copy_of_SREG;}                                       //Restore interrupt state



/***********************************************************************************************************/
ISR(PCINT2_vect) {RN_arithmetic_sw1_and_sw2_isr();}           //input number: store keypresses in array -digits
ISR(PCINT0_vect) {RN_arithmetic_sw3_isr();}



/***********************************************************************************************************/
void RN_arithmetic_sw1_and_sw2_isr(void){
Timer_T0_10mS_delay_x_m(5);                                  //switch bounce delay
clear_PCI_on_sw1_and_sw2;                                    //clear spurious interrupts due to sw bounce
if ((digits[0] == '0') && (digits[1] == '\0')\
&&(switch_2_down) && (!(expnt)))return;                      //Ignore leading zeros
while(switch_1_down) {                                       //While sw1 down 1 to scroll through digits 0...9
switch(digits[0]){                                          //Test digit[0]
case '9': digits[0] = '0';break;                            //If digit[0] is 9 scroll to zero
case 'P': digits[0] = '0';break;                            //If digit [0] is P scroll to zero
default: digits[0]++; break;}                               //Otherwise increment it

if (switch_3_down){digits[0] = 'P';                          //Pulse sw3 to set digit[0] to P (decimal point)
I2C_Tx_2URNs_from_IO(digits);                               //Update display        
disable_pci_on_sw1;                                         //prevent illegal sw1 switch press
return;}                                                     //and return to Line B

I2C_Tx_2URNs_from_IO(digits);                               //update display (delay sets the update rate
Timer_T0_10mS_delay_x_m(27);}                               //return to Line B when sw1 is released 
if(switch_2_down) {                                         //Use switch 3 to shift display left
if(digits[0] != 'P')                                        //if digit 0-9 entered          

/***Prevent overflow on first number*****/
{if ((digits[5])&&(!(Return_key_press))){                   //if first number has 6 digits terminate entry
I2C_Tx_2URNs_from_IO(digits);                               //No change indicates keypress on sw3.
shift_digits_left; digits[0] = '\0';                        //shift display left and blank digit 0
shift_digits_left; digits[0] = '0';                         //shift display left and initialise digit 0
Return_key_press++; return;}                                //Set Return_key_press to 1 and return to Line B
/****************************************/
  
shift_digits_left; digits[0] = '0';                         //shift the display and initialise digit[0] (make it display '0')
if(expnt)                                                   //if the exponent has already been initialised
{expnt--; }}                                                //decrement it and 
                                                            //if digit[0] = P was entered set the exponent to -1
else {expnt = -1;                                           //initiate flashing 'P' symbol
enable_pci_on_sw1;}                                         //restore sw1 switch press                
I2C_Tx_2URNs_from_IO(digits);}}                            //update the display and return to ready state Line B
        


/***********************************************************************************************************/
void RN_arithmetic_sw3_isr(void){
if (switch_3_up) return;                                   //Ignore key releases
if (digits[0] == 'P') return;                               //Do not process -P- entry
if((digits[0] == '0') && (digits[1] == 0))return;           //Ignore double press on switch 3

I2C_Tx_2URNs_from_IO(digits);                               //No change indicates keypress on sw3, Return key press.

shift_digits_left; digits[0] = '\0';                        //shift display left and blank digit 0
shift_digits_left; digits[0] = '0';                         //shift display left and initialise digit 0

Return_key_press++;                                         //Increment return key to indicate entry of second number
if(Return_key_press== 2)                                    //If second number has been entered
{disable_pci_on_sw3;                                        //Clear all pci interrupts and return to line B
disable_pci_on_sw1_and_sw2;}}
