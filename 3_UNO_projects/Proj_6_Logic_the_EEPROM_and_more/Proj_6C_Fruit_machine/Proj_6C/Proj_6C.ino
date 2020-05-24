
/*Proj_6C_Fruit_machine
********************************************************************/


/*Just press sw2

Switch location SW1(PD2) - SW2(PD7) – SW3(PB2).*/


#include "Proj_6C_header_file.h"


volatile char mode;
volatile int l;
volatile int PRN2=0x0F;
char digit_array[8];

int main (void){  
int m=0;  
unsigned int lfsr;

setup_UNO;
User_prompt;
setup_and_enable_PCI;

String_to_PC("\r\n Just press sw3\r\n");

sei();
while(1){
if(mode) {                                                        //mode of zero results in a stationary display
lfsr = PRN_16bit_GEN_UNO(0);                                          //If mode is 3: get 16 bit random number
digit_array[m+2]=((lfsr)%5+'0');                                  //convert to number between 0 & 4
I2C_Tx_8_byte_array(digit_array );                                 //Illuminate digits 2 to 5 only
Timer_T0_10mS_delay_x_m(2);
m++;if(m==4)m=0;}}}                                               //cycle continuously round the display




/*************************************************************************************/
ISR (PCINT0_vect){                                                  //Uses interrupts generated by pressing and releasing sw3.
char Counter_speed=4;
unsigned int Start_point=0;

if(switch_3_down){mode = 3; 
for(int m = 0; m<8; m++)digit_array[m]=0;                           //If sw3 pressed clear display
I2C_Tx_8_byte_array(digit_array);return;}

PRN2 = PRN_4_bit_generator(PRN2);                                   //If sw3 is released use random number to initialise Timer 1
switch (PRN2){
case 1:           Start_point = 0xFFFF -  0xFFFF; break;
case 2:           Start_point = 0xFFFF -  0x8000; break;
case 3: case 4:   Start_point = 0xFFFF -  0x4000; break;
case 5: case 6:   Start_point = 0xFFFF -  0x2000; break;
case 7: case 8:   Start_point = 0xFFFF -  0x1000; break;
case 9: case 10:  Start_point = 0xFFFF -  0x0800; break;
case 11: case 12: Start_point = 0xFFFF -  0x0400; break;
case 13: case 14: Start_point = 0xFFFF -  0x0200; break;
case 15:          Start_point = 0xFFFF -  0x0100; break;}
Timer_T1_sub_with_interrupt(Counter_speed, Start_point);}



/*************************************************************************************/
char PRN_4_bit_generator(char lfsr){                                 //Generates 16 random numbers 0-15.
char bit;                                                           //Google "lfsr" for details of the random number generator
bit = ((lfsr >> 0)^(lfsr >> 1)) & 1;
lfsr = (lfsr >> 1) | (bit << 3);
return lfsr;}



/*************************************************************************************/
ISR(TIMER1_OVF_vect) { mode=0; TCCR1B=0;}                           //When Timer 1 overflows pause display and halt timer
