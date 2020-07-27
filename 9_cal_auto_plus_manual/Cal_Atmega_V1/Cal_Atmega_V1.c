

void initialise_timers_for_cal_error(void);
void start_timers_for_cal_error(void);
long compute_error(char, char, char);
void Minimise_error(int, char*, char*, long*, char*, char);
void Auto_cal (void);
char Manual_cal(void);



#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>


#include "Cal_Atmega_V1_header_file.h"





int main (void)
{
setup_HW_special;
OSCCAL_DV = OSCCAL;
ASSR = (1 << AS2); 												//initialise T2 for crystal


Auto_cal ();Timer_T1_sub(T1_delay_1sec);


while(1){
if (Manual_cal() != 'x')break;}

sendString("\r\nAll done\r\n");
return 1;}



char Manual_cal(void){
long cal_error;
char OSCCAL_UV, osccal_MIN;		
long percentage_error;

UCSR0B &= (~(1 << RXEN0));											//avoid unwanted keypresses

sendString("\r\nManual cal: please wait 5 seconds\r\n");
Timer_T0_sub(T0_delay_5ms);


sei();

EA_buff_ptr = 0;
cal_mode = 5;		
Get_ready_to_calibrate;
OSCCAL -=20;
osccal_MIN = OSCCAL;												//Compute cal error for 41 values of OSCCAL
for(int m = 0; m <= 40; m++)
{cal_error = compute_error(1,cal_mode,1);OSCCAL++;}
OSCCAL = OSCCAL_WV;
close_calibration;

newline();
sendString("OSCCAL\t Error\r\n");


for(int m = 0; m <= 40; m++)										//Print table of values
{sendLongNum(10,osccal_MIN); osccal_MIN++;
sendChar('\t');sendLongNum(10,buffer[m]); 
sendChar('\t');
percentage_error = buffer[m];
sendLongNum(10,percentage_error*100/62500);sendChar('%'); 
newline();}

sendString("\r\nEnter new user cal value");	
UCSR0B |= (1 << RXEN0);
OSCCAL_UV = UC_from_KBD();


	
/*********************************************/
Get_ready_to_calibrate;												//Test value of OSCCAL entered by user
if(OSCCAL_UV == 0xFF)OSCCAL = OSCCAL_DV;							//If 0xFF reinstate working value
else OSCCAL = OSCCAL_UV;											//OSCCAL test value
calibrate_without_sign_plus_warm_up_time;								
close_calibration;

cli();

if(cal_error > 1750)												//Error resulting from User OSCCAL exceeds 1750
{OSCCAL = OSCCAL_WV;												//Reinstate default value
OSCCAL_UV = OSCCAL;
sendString("\tChange rejected: error too great");}			


/*************************************************/
eeprom_write_byte((uint8_t*)0x3FE, OSCCAL_UV); 					//save user OSCCAL to EEPROM
eeprom_write_byte((uint8_t*)0x3FF, OSCCAL_UV); 

sendString("\r\nValues saved to EEPROM  ");	
sendLongNum(10,eeprom_read_byte((uint8_t*)0x3FE)); sendChar('\t');
sendLongNum(10,eeprom_read_byte((uint8_t*)0x3FF)); sendString("\tUser value\r\n\t\t\t");
sendLongNum(10,eeprom_read_byte((uint8_t*)0x3FD)); sendString("\t\tDefault value\r\n");

sendString("Press x to repeat or AOK to terminate");
return waitforkeypress();}










/************************************************************************************************/
void Auto_cal (void){				
char counter_1, counter_2;		
char OSCCAL_mem = 0;		
long  error_mag; 
int limit;

UCSR0B &= (~(1 << RXEN0));												//avoid unwanted keypresses

sendString("Auto cal running\r\n");

Timer_T1_sub(T1_delay_1sec);											//Crystal warm up time

sei();
cal_mode = 1;
Get_ready_to_calibrate;
		
counter_1 = 0xF1;
while(1){sendChar('.');
counter_1 -= 1;
OSCCAL = counter_1; error_mag = compute_error(0,cal_mode,0); 
if(counter_1 > 0xE0)continue; 
if(error_mag < 1000)break;}

sendString("\r\nAlmost done");
OSCCAL_mem = OSCCAL;
counter_2 = 0;

cal_mode = 5;		
limit = 1000;
for(int m = 1; m <= 9; m++){
limit -= 100;
Minimise_error(limit, &counter_1, &counter_2, &error_mag, &OSCCAL_mem, cal_mode);}
		
error_mag = compute_error(0,cal_mode,0);
OSCCAL_WV = OSCCAL;	
close_calibration;
eeprom_write_byte((uint8_t*)0x3FE, OSCCAL_WV); 
eeprom_write_byte((uint8_t*)0x3FF, OSCCAL_WV);

sendString("\r\nNew OSCCAL value ");

sendLongNum(10,OSCCAL_WV); 
UCSR0B |= (1 << RXEN0);
cli();}




/************************************************************************************************/
void initialise_timers_for_cal_error(void){
TCNT1=0;TCCR1B = 0;													//Reset and halt T1
TCCR2B =  0x0;	while(ASSR & (1 << TCR2BUB));						//Halt T2
TCCR2A = 0; while(ASSR & (1 << TCR2AUB));							//Reset T2 
TCNT2=0; while(ASSR & (1 << TCN2UB));	}							//Reset TCNT2




/************************************************************************************************/
void start_timers_for_cal_error(void)
{TCCR2B = 1; 	
while(ASSR & (1 << TCR2BUB));
TCCR1B = 1;}




/************************************************************************************************/
void Minimise_error(int limit, char *counter_1, char *counter_2, long *error_mag, char *OSCCAL_mem, char local_cal_mode )
{while(*counter_2 < 20){ OSCCAL = *counter_1; *error_mag = compute_error(0,local_cal_mode,0); 
if(*error_mag < limit)break;
*counter_1 -= 1;
*counter_2 +=1;}
if (*counter_2 < 20)*OSCCAL_mem = OSCCAL;else OSCCAL = *OSCCAL_mem;}




/************************************************************************************************/		
long compute_error(char local_error_mode, char local_cal_mode, char sign)					
{long error;
char Num_1, Num_2;
	
if (local_cal_mode == 1){Num_1 = 2; Num_2 = 1;}
if (local_cal_mode == 2){Num_1 = 6; Num_2 = 4;}
if (local_cal_mode == 5){Num_1 = 15; Num_2 = 10;}
		
EA_counter = 0;															//Compute error for each value of OSCCAL 10 times
error_SUM = 0;
while(EA_counter < Num_1);EA_counter = 0;
error = error_SUM;
if ((!sign) && (error < 0)) error *= (-1);
if (local_error_mode)
{buffer[EA_buff_ptr] = error/Num_2; EA_buff_ptr++;}
return error/Num_2;}




/************************************************************************************************/
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




/************************************************************************************************/
ISR(TIMER1_OVF_vect) {T1_OVF += 1;}	











