
/**************Proj_5G_Stand_a_lone_clock********************/


/*Works from a 5V supply independently of the PC,
 Having uploaded the sketch press the PCB_A reset switch 
 to print out instructions on  the serial monitor or
 see Proj_5G_header_file.h
 Note: having entered the time press sw1 to start the clock.*/



#include "Proj_5G_header_file.h"



void time_from_IO(void);
volatile int digit_num = 7;
char start_time[8];
volatile char Data_Entry_complete=0;
char keypress = 0;

  
int main (void){
char display_mode;

setup_UNO;
if (switch_2_down)
{User_prompt; 
User_instructions;
while(switch_2_down);}
time_from_IO();
I2C_Tx_OS_timer(AT_clock_mode, start_time);                                             //Send Start clock command (AT clock mode is 7)



/********************Clock Control loop**************************************************************************/

display_mode = 0;
while(1){                                                                              //Operation continuously cycles around this loop 
Timer_T0_10mS_delay_x_m(10);
switch (display_mode){                                                                

case 0: if(switch_3_down){Timer_T0_10mS_delay_x_m(50); display_mode = 'A';}
    if(switch_2_down){display_mode = 'B';I2C_Tx_Clock_command(hide_clock);while(switch_2_down);}
    if(switch_1_down){display_mode = 'C';I2C_Tx_Clock_command(pause_clock);while(switch_1_down);}break;

case 'A': if(switch_3_up)display_mode = 1; else {while(switch_3_down); display_mode = 2;} break;
case 'B':if(switch_2_down){display_mode = 0;I2C_Tx_Clock_command(display_clock);while(switch_2_down);}break;
case 'C':if(switch_1_down){display_mode = 0; I2C_Tx_Clock_command(display_clock);while(switch_1_down);}break;


case 1: if (switch_2_down){I2C_Tx_Clock_command(increment_seconds);Timer_T0_10mS_delay_x_m(20);}
    if (switch_1_down){I2C_Tx_Clock_command(increment_minutes);Timer_T0_10mS_delay_x_m(20);}
    if(switch_3_down){ while (switch_3_down);display_mode = 0; }break;
    
case 2: if (switch_2_down){I2C_Tx_Clock_command(decrement_seconds);Timer_T0_10mS_delay_x_m(20);}
    if (switch_1_down){I2C_Tx_Clock_command(decrement_minutes);Timer_T0_10mS_delay_x_m(20);}
    if(switch_3_down){while (switch_3_down);display_mode = 0; }break;}}}



/***********************************************************************************************************************/

void time_from_IO(void){
char temp=0;
setup_and_enable_PCI;
disable_pci_on_sw3;
for(int m = 0; m<= 7; m++)start_time[m] = 0;
start_time[7] = '_';I2C_Tx_8_byte_array(start_time);
sei();
while(!(Data_Entry_complete));                                                        //Line A. wait here for pci interrupts used to enter data
Data_Entry_complete = 0;        
for(int m = 0; m<=3; m++)                                                             //Reverse "start_time" array
{temp = start_time[m]; 
start_time[m] = start_time[7-m];
start_time[7-m] = temp;}}


/***********************************************************************************************************************/

ISR(PCINT2_vect) {                                                                     //input number: store keypresses in array -start_time
if((switch_1_up) && (switch_2_up))return;
Timer_T0_10mS_delay_x_m(5);                                                           //Place delay here to trap bounce on sw release
clear_PCI_on_sw1_and_sw2;

if (switch_2_down){                                                                    //Switch_2: used to scroll through the permissible                    
switch(digit_num){                                                                    //characters for each digit on the display
case 7: 
switch (start_time[7]){                                                               //Enter Digit 7 hours 0,1 or 2
case '0': case '1': start_time[7]++; break;
case '2': start_time[7] = '0';break;
default:start_time[7]='0';break; }break;

case 6:
if (start_time[7]== '2'){                                                               //Enter Digit 6 hours 0,1,2 or 3
switch (start_time[6]){
case '0': case '1': case '2': start_time[6]++; break;
case '3': start_time[6] = '0';break;
default:start_time[6]='0';break;}} 

else                                                                                      //Enter Digit 6 hours 0 to 9
switch (start_time[6]){
case '_': case '9': start_time[6] = '0'; break;
default:start_time[6]++;break;}
break;

case 4:                                                                                   //Enter Digit 4 minutes 0 to 5
switch (start_time[4]){
case '0': case '1': case '2':
case '3': case '4': start_time[4]++; break;
case '5': start_time[4] = '0'; break;
default:start_time[4]='0';break;} break;


case 3:                                                                                     //Enter Digit 3 minutes 0 to 9
switch (start_time[3]){
case '_': case '9': start_time[3] = '0'; break;
default:start_time[3]++;break;} break;


case 1:                                                                                     //Enter Digit 1 seconds 0 to 5
switch (start_time[1]){
case '0': case '1': case '2':
case '3': case '4': start_time[1]++; break;
case '5': start_time[1] = '0'; break;
default:start_time[1]='0';break;} break;

case 0:                                                                                     //Enter Digit 0 secondes 0 to 9
switch (start_time[0]){
case '_': case '9': start_time[0] = '0'; break;
default:start_time[0]++;break;} break;}

I2C_Tx_8_byte_array(start_time);}                                                           //Keep display up-to date 
      


if (switch_1_down){                                                                           //Switch_1: Used to select a character and move onto the next digit
switch(digit_num){
case 7: case 4: case 1:
digit_num--;start_time[digit_num] = '_';break;
case 6: case 3: digit_num -=2; start_time[digit_num] = '_'; break;
case 0: digit_num = 7; Data_Entry_complete = 1;
disable_pci_on_sw1_and_sw2;
start_time[2] = ' ';
start_time[5] = ' ';
while(switch_1_down);Timer_T0_10mS_delay_x_m(10); 
break;}
I2C_Tx_8_byte_array(start_time);}}                                                            //return to Line A when sw1 is released
