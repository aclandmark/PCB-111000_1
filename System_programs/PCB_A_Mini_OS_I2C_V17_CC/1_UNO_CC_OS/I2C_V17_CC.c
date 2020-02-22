/*********************
Display control part of PCB_A_Mini_OS_I2C_V16_1_CC.
Also offers services which the user app calls over the I2C bus
Services include display of binary or decimal numbers, display of individual segments
Clock/stop watch functions and arithmetic.



I2C opperation:	The PCB_A 328 hosts the master I2C block
The UNO 328 hosts the slave I2C block.  The master is permanently active however slave is only
turned on when the user project needs to initiate a transaction.   In this way the user project controls
the I2C bus without hosting the master.  It was done this way because a PIC 16F640 was originally chosen to host
the user projects.  The PIC only offered the slave implementation of the I2C bus.



*EEPROM reservations
0x3FF	user cal if set
0x3FE	user cal if set
0x3FD	Default cal supplied by Atmel
0x3FC	Set to 1 if diagnostic mode required.  Diagnostic mode is now discontinued
0x3FB	Controls multiplexter (T0) on interval:  0xFF for 2ms, 0xFE for 500uS and 0xFD for 125uS
0x3FA	Not used
0x3F9	Reset status_1: Zero Indicates that PCB_A has just been programmed by Project_programmer_AVR
0x3F8	prog_counter_H
0x3F7	prog_counter_L
0x3F6	cmd_counter_H
0x3F5	cmd_counter_L
0x3F4	Reset_status_2: Zero resets UNO immediately after PCB_A has been programmed triggering the h/t/r/D prompt
0x3F3	8 bit PRN gen EEP subroutines



***********************/

# include "I2C_V17_Header_file_CC.h"


int main (void){

signed char digit_num=0;
int  letter=0;
char payload_size, sign;
long L_number;
signed char expnt;
char test_num, test_digit, sign_bit;


char *SW_Version = "\r\nPCB_A Mini-OS: I2C_V17_CC\r\n";
char *SW_info = "SW information:\
  Compatible with User Projects V1_7, PCB_A_Bootloader V5_CC.\r\n\
  and UNO_bootloader_for_hex&text_V5\
  \r\nProgrammed using Project_programmer_AVR_V1\r\n";
	


/****Watchdog initiated for mode F only (user clock/stop watch with
external 10mS crystal interrupt).*********/

/***********Brown-out:  This is set (using config bits only) for 2.9V*************/

if(MCUSR & (1 << BORF)){							//Detect brown-out
MCUSR &= (~(1 << BORF));}							//Reset brown-out flag 

ADMUX |= (1 << REFS0);								//select internal ADC ref and remove external supply on AREF pin
setup_watchdog;	
initialise_IO;										//Ensures that all IO is initially set to WPU

if(!(eeprom_read_byte((uint8_t*)0x3F4)))			//If PCB_A has just been programmed with I2C_V16_CC using the project programmer 
{eeprom_write_byte((uint8_t*)0x3F4, 0xFF);			//the UNO device is automatically reset so the the project programer can be removed
//PORTB |= (1 << PB2);								//Set UNO signalling line high (WPU)
//DDRB &= (~(1 << DDB2));							//Note: Theses are default states for the CC display driver
PORTC &=(~(1 << DDC3));							//PDC3 is the output used to reset the UNO device
DDRC |= (1 << DDC3);								//Put UNO in reset for 10mS
Timer_T1_sub(T1_delay_10ms);						//After its release from reset the UNO selects its boot loader
PORTC |=(1 << DDC3);									
DDRC &= (~(1 << DDC3));
Timer_T1_sub(T1_delay_125ms);}						//Delay required due to UNO Start Up Time of 65mS

TWBR = 32;											//gives 100KHz I2C clock for TWSR 
ASSR = (1 << AS2); 								//initialise T2 for crystal
timer_2_counter=0;									//Initialsise timer_2_counter to zero

OSCCAL_DV = OSCCAL;									//Save default value of OSCCAL
cal_PCB_A_328;										//Select User value of OSCCAL if one exists
if(MCUSR & (1 << PORF))
{Cal_at_Power_on_Reset();}							//Only run clock calibration test following a POR
OSCCAL_WV = OSCCAL;									//Save working value of OSCCAL

Initialise_dislay_brightness;
set_digit_drivers;
clear_digits;
clear_display;
initialise_Arithmetic_variables;

clock_flag=0;
Ten_mS_tick_counter = 0;


/******************************Start multiplexer******************************/		
sei();
T0_interupt_cnt = 0;	
TIMSK0 |= (1 << TOIE0);										
switch(eeprom_read_byte((uint8_t*)0x3FB)){
case 0xFF: timer_T0_sub_with_interrupt(T0_delay_2ms); break;			//High intensity: Each of the 8 digits is active for 2mS in 16ms
case 0xFE: timer_T0_sub_with_interrupt(T0_delay_500us); break;			//Normal intensity:  Each digit is refreshed every 16mS but only for 500uS
case 0xFD: timer_T0_sub_with_interrupt(T0_delay_125us); break;			//Low intensity: Each digit is refreshed for 125uS every 16mS
default: 	eeprom_write_byte((uint8_t*)(0x3FB), 0xFE);				//BUG Extra line added post publication
			timer_T0_sub_with_interrupt(T0_delay_2ms); break;}

	
while(1){

while((mode == 'F') || (mode == 'K'));				//Multiplexer continues to work, otherwise requires reset to escape.

	
/********Code parks here following WDT, POR and Brown-out***************/
while(1){
TWCR = (1 << TWINT)|(1 << TWSTA)|(1 << TWEN);		//send a start condition  (this identical to a "repeated start")
while (!(TWCR & (1 << TWINT)));					//Wait for TWINT flag 
TWDR = 0x03;										//Address of slave (master read operation)  SLA + R
TWCR = (1 << TWINT) | (1 << TWEN);					//Clear TWINT bit to start transmission of address
while (!(TWCR & (1 << TWINT)));					//Wait for TWINT flag 
if (TWSR == 0x40)break;								//SLA + R successfully transmitted ACK recveived 
if(clock_flag==1){refresh_clock_display;}}

clock_flag=0; 
payload_size = I2C_master_receive(1);
if(payload_size)mode = I2C_master_receive(1);
else mode = I2C_master_receive(0);

if (mode != 8) display_mask=0;						//dissable flashing digits

if(payload_size){									//payload is zero for mode 'F'

for (int m = 0; m < payload_size; m++){			//payload_size = Num digits to be downloaded
if (m == (payload_size-1))
I2C_data[m] = I2C_master_receive(0);
else I2C_data[m] = I2C_master_receive(1);}
TWCR = (1 << TWINT )|(1 << TWEN )|(1 << TWSTO );}	//Send stop bit	

switch(mode){

case 3:  case 8: case 7:  
case 'F': case 'I':  case 'Q': break;
default: 
{for(int m = 0; m < 8; m++)display_buf[m] = 0;}
break;}	

switch (mode){

case 1:	I2C_Tx_2_integers; break;					//Uses the 32 vertical segments to display two binary numbers

case 2: 
case 3: I2C_Tx_any_segment;	break;					//Illuminates/clears any of the 56 segments
	
case 4: I2C_Tx_8_byte_array; break;					//Display a string of 8 digits

case 5:	I2C_Tx_display_char; break;					//Displays +/- char as number and binary																												

case 6: I2C_Tx_long;break;							//Displays 8 digit number

case 7: I2C_Tx_Compile_tables();break;				//I2C_Tx_Initiate_tables() and I2C_Tx_Inc/dec_tables()

case 8: I2C_Tx_2URNs_from_IO();break;				//Generates numbers of type 0.1234 With an exponent											
	
case 9: I2C_Tx_Uarithmetic_OP();break;				//Takes numbers entered using mode B and does some arithmetic

case 'A': I2C_Tx_accumulator_1();	break;			//Implements add, subtract and clear modes											

case 'B': I2C_Tx_accumulator_2();break;				//Aquires data for use by accumulator											

case 'C': basic_clock(); break;						//I2C_Tx_OS_timer(AT_clock_mode, start_time): Starts the timer/clock

case 'D': multi_mode_clock(); break;				//Used with subroutine I2C_Tx_Clock_command(char timer_mode, char command): Controls the timer/clock

case 'E': stop_watch(); break;						//Used by I2C_Tx_Clock_command(one100ms_mode) and I2C_Tx_Clock_command(ten_ms_mode)

case 'F': I2C_initiate_10mS_ref;break;				//Ten_mS_interrupt for combined clock and stopwatch;	
	
case 'G': I2C_Tx_BWops; break;						//Used to illustarte bit wise logic operations

case 'H': Message_from_the_OS();break;				//Messages compiled by user and saved to pcb_A Atmrga 328 EEPROM																		

case 'I': I2C_displayToLong;break;					//Converts display to long number and transmits it to the UNO device														

case 'J': I2C_Tx_real_num;break;					//Displays a real number														

case 'K': I2C_Tx_float_num;break;					//Scrolls scientific number accross the display												

case 'L': Multiplexer_demo; break;

case 'M': Plot_cal(); break;						//Scans 328 cal-factor fronmm 0x10 to 0xF0

case 'N': manual_cal_PCB_A_device(); break;															

case 'O': PCB_test; break;							//For manufacturing test: Dissables the multiplexer

case 'P': I2C_Rx_get_version; break;

case 'Q': I2C_Tx_LED_dimmer;break;

default: break;}}}

/******************Modes 'R', 'S' and 'T'are reserved***************************/

	
	
	void I2C_master_transmit(char data){
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));}		

	char I2C_master_receive(char mode){
	char data;
	if(!(mode))TWCR = (1 << TWINT) | (1 << TWEN);				
	else TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);					
	while (!(TWCR & (1 << TWINT)));
	data =  TWDR;
	return data;}


