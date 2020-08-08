

#include "Plot_Cal_Atmega_header_file.h"
void Plot_cal (void);
void set_up_target_parameters(void);



int main (void)

{
char User_response;

setup_HW_basic;

set_up_target_parameters();										//Reads header file to determine device type

cal_device;														//checks cal status and adopts user cal bytes if present

OSCCAL_WV = OSCCAL;


User_prompt;

ASSR = (1 << AS2); 												//initialise T2 for crystal

sendString("Plot of OSCCAL v calibration error\r\n");

Plot_cal();

SW_reset;}




/************************************************************************************************/

void Plot_cal (void){
char counter_1;
long  error_mag;
Timer_T1_sub(T1_delay_1sec);											//Crystal warm up time

sei();
cal_mode = 5;
Get_ready_to_calibrate;
		
counter_1 = 0x0E;
while(counter_1 < 0xF0){
counter_1 += 1;
OSCCAL = counter_1; error_mag = compute_error(0,cal_mode,1); 

OSCCAL = OSCCAL_WV;
sendLongNum(10,counter_1); 
sendChar('\t');sendLongNum(10,error_mag);
newline();
Timer_T0_sub(delay_2ms);
}}








/**********************************************************************************************************************/
ISR(TIMER2_OVF_vect) {			
long TCNT1_BKP, overflow = 0, target_res;

target_res = 62500;
TCCR1B = 0;																//Halt T1
TCNT1_BKP = TCNT1;														//Copy the value of TCNT1
TCNT1 = 0;																//Clear TCNT1
TCCR1B = 1;																//Get T1 running again ASAP (Note T2 has not stopped running)
																		//Get T1 running again ASAP (Note T2 has not stopped running)
if(EA_counter < cal_mode)T1_OVF = 0;									//Ignore first few results
else
{
switch(T1_OVF){
case 0: overflow = 0; break;
case 1: overflow = 65536; break;
case 2: overflow = 131072; break;}
error_SUM = error_SUM + (TCNT1_BKP - target_res + overflow);
T1_OVF = 0;}
EA_counter++;}




/************************************************************************************************************************************/
ISR(TIMER1_OVF_vect) {T1_OVF += 1;}	


/************************************************************************************************************************************/
void set_up_target_parameters(void){
if((Device[0] == '3') && (Device[1] == '2')
&& (Device[2] == '8'))
{FlashSZ = 0x4000;
EE_size = 0x400;}


if((Device[0] == '1') && (Device[1] == '6')
&& (Device[2] == '8'))
{FlashSZ = 0x2000;		
EE_size = 0x200;}




}
