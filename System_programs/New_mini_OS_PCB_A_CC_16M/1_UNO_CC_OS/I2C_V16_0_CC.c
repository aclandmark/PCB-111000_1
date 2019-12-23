/*********************
Projects version 9
I2C program version 4

V9.9 Prevents users applying impractical user-cal values to the ATMEGA 328
Performs scan of the ATMEGA 328 clock cal factor
Following POR check cal factor and perform auto clock recovery if necassary

*EEPROM reservations
0x3FF	user cal if set
0x3FE	user cal if set
0x3FD	Default cal supplied by Atmel
0x3FC	If 1: press 'x' diagnostic mode else press 'r' normal mode
0x3FB	If 0 use multiplexter (T0) period of 4ms else use period of 2mS (std)


***********************/
void basic_clock (void);
void multi_mode_clock(void);
void stop_watch(void);

# include "I2C_V16_0_Header_file_CC.h"


int main (void){

signed char digit_num=0;
int  letter=0;
char payload_size, sign;
long L_number;
signed char expnt;
//int text_max, eeprom_ptr;
//char string_counter, string_no;
char test_num, test_digit, sign_bit;
//char OSCCAL_test;

char *SW_Version = "\r\nOS: I2C_V16_1_UNO_CC\r\n";
char *SW_info = "SW information: Operating system I2C_V16_1_UNO_CC\
  Projects V1_7 and Bootloader V5_UNO_CC.\
  \r\nProgrammed using Project_programmer_AVR\r\n";
	


/****Watchdog initiated for mode G only (user clock/stop watch with
external 10mS crystal interrupt).*********/

/***********Brown-out:  This is set (using config bits only) for 2.9V*************/

if(MCUSR & (1 << BORF)){								//Detect brown-out
MCUSR &= (~(1 << BORF));}								//Reset brown-out flag 

ADMUX |= (1 << REFS0);									//select internal ADC ref and remove external supply on AREF pin
setup_watchdog;	
initialise_IO;											//Ensures that all IO is initially set to WPU

if(!(eeprom_read_byte((uint8_t*)0x3F4)))				//If PCB_A has just been programmed with I2C_V16_CC using the project programmer 
{eeprom_write_byte((uint8_t*)0x3F4, 0xFF);				//the UNO device is automatically reset so the the project programer can be removed
//PORTB |= (1 << PB2);									//Set UNO signalling line high (WPU)
//DDRB &= (~(1 << DDB2));								//Note: Theses are default states for the CC display driver
PORTC &=(~(1 << DDC3));								//PDC3 is the output used to reset the UNO device
DDRC |= (1 << DDC3);									//Put UNO in reset for 10mS
Timer_T1_sub(T1_delay_10ms);							//After its release from reset the UNO selects its boot loader
PORTC |=(1 << DDC3);									
DDRC &= (~(1 << DDC3));
Timer_T1_sub(T1_delay_125ms);}							//Delay required due to UNO Start Up Time of 65mS

TWBR = 32;												//gives 100KHz I2C clock for TWSR 
ASSR = (1 << AS2); 									//initialise T2 for crystal
timer_2_counter=0;										//Initialsise timer_2_counter to zero

OSCCAL_DV = OSCCAL;										//Save default value of OSCCAL
cal_PCB_A_328;											//Select User value of OSCCAL if one exists
if(MCUSR & (1 << PORF))
{Cal_at_Power_on_Reset();}								//Only run clock calibration test following a POR
OSCCAL_WV = OSCCAL;										//Save working value of OSCCAL

set_digit_drivers;
clear_digits;
clear_display;
initialise_Arithmetic_variables;

clock_flag=0;
Ten_mS_tick_counter = 0;


	
if((eeprom_read_byte((uint8_t*)0x3FB) == 0xFF) ||\
 (eeprom_read_byte((uint8_t*)0x3FB) == 0x01));			//Check multiplexer control has a valid value 
else eeprom_write_byte((uint8_t*)0x3FB,0x01);			//(i.e. for normal or bright display)

sei();
T0_interupt_cnt = 0;	
TIMSK0 |= (1 << TOIE0);								//Start the display multiplexer
MUX_cntl = 0;
if(eeprom_read_byte((uint8_t*)0x3FB) == 0xFF)
{timer_T0_sub_with_interrupt(T0_delay_2ms);}			//Full display brightness
else 
{timer_T0_sub_with_interrupt(T0_delay_500us);}			//Normal display brightness
	
	
	
	
while(1){

while((mode == 'G')||(mode == 'g')\
||(mode == 'I')||(mode == 'i')\
 || (mode == 'P')||(mode == 'p'));	
/********Code parks here following WDT, POR and Brown-out***************/
while(1){
TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);			//send a start condition  (this identical to a "repeated start")
while (!(TWCR & (1 << TWINT)));							//Wait for TWINT flag 
TWDR = 0x03;												//Address of slave (master read operation)  SLA + R
TWCR = (1 << TWINT) | (1 << TWEN);							//Clear TWINT bit to start transmission of address
while (!(TWCR & (1 << TWINT)));							//Wait for TWINT flag 
if (TWSR == 0x40)break;										//SLA + R successfully transmitted ACK recveived 
if(clock_flag==1){refresh_clock_display;}}

clock_flag=0; 

payload_size = I2C_master_receive(1);

if(payload_size)mode = I2C_master_receive(1);
else mode = I2C_master_receive(0);

if (mode != 'B') display_mask=0;							//dissable flashing digits

if(payload_size){															//payload is zero for mode 'G'

for (int m = 0; m < payload_size; m++){									//payload_size = Num digits to be downloaded
	if (m == (payload_size-1))I2C_data[m] = I2C_master_receive(0);
	else I2C_data[m] = I2C_master_receive(1);}
	TWCR =  (1 << TWINT ) | (1 << TWEN ) | (1 << TWSTO );}				//Send stop bit	

switch(mode){
case 'a': case 'b': case 'B':case 'd': case 'D': case 'g': 
case 'G': case 'j': case 'J': case 'w': case 'W': break;
default: {for(int m = 0; m < 8; m++)display_buf[m] = 0;}break;}			//case B added

switch (mode){


case 4:	I2C_Tx_2_integers; break;								//Uses the 32 vertical segments to display two binary numbers

case 'A': case 'a':	I2C_Tx_any_segment;	break;					//Illuminates/clears any of the 56 segments
	
case 1: I2C_Tx_8_byte_array; break;								//Display a string of 8 digits

case 5:	I2C_Tx_display_char; break;								//Displays +/- char as number and binary																												

case 'H': case 'h':	I2C_Tx_long;break;							//Displays 8 digit number

case 'D': case 'd':	I2C_Tx_Compile_tables();break;				//I2C_Tx_Initiate_tables() and I2C_Tx_Inc/dec_tables()

case 'B': case 'b':	I2C_Tx_2URNs_from_IO();break;				//Generates numbers of type 0.1234 With an exponent											
	
case 'C': case 'c': I2C_Tx_Uarithmetic_OP();break;				//Takes numbers entered using mode B and does some arithmetic

case 'E': case 'e':	I2C_Tx_accumulator_1();	break;				//Implements add, subtract and clear modes											

case 'F': case 'f':	I2C_Tx_accumulator_2();break;				//Aquires data for use by accumulator											

case 7: basic_clock(); break;									//I2C_Tx_OS_timer(AT_clock_mode, start_time): Starts the timer/clock

case 8: multi_mode_clock(); break;								//Used with subroutine I2C_Tx_Clock_command(char timer_mode, char command): Controls the timer/clock

case 9: stop_watch(); break;									//Used by I2C_Tx_Clock_command(one100ms_mode) and I2C_Tx_Clock_command(ten_ms_mode)

case 'G': case 'g': I2C_initiate_10mS_ref;break;				//Ten_mS_interrupt for combined clock and stopwatch;	
	
case 3: I2C_Tx_BWops; break;									//Used to illustarte bit wise logic operations

case 'Y': case 'y':	Message_from_the_OS();break;																			

case 'J': case 'j':	I2C_displayToLong;break;					//Converts display to long number and transmits it to the UNO device														

case 'k': case 'K':	I2C_Tx_real_num;break;						//Displays a real number														

case 'I': case 'i':	I2C_Tx_float_num;break;						//Scrolls scientific number accross the display												













case 'M': case'm': I2C_Rx_get_version; break;

case 'N': case'n': manual_cal_PCB_A_device(); break;															

case 'O': case'o': break;										//Obsolete cal routine																					

case 'P': case 'p': start_T2_for_ATMEGA_168_cal(1);	break;		//Obsolete: Generates 7_8125mS TWI slave interrupt  																				

case 'Q': case 'q':	Multiplexer_demo; break;

case 'R': case 'r':	PCB_test; break;							//For manufacturint test

case 'S': case 's':	Plot_cal(); break;							//Scans 328 cal-factor fronmm 0x10 to 0xF0
		
case 'W': case 'w':  I2C_Tx_LED_dimmer;break;

case 'x': case 'X': diagnostic_mode;break;						//User enters 'x' at the p/r prompt and the mini-OS enters a at Eeprom location 0x3FC


case 'Z': case 'z':	Read_AT328_EEPROM;break;					//Used to restore UNO device EEPROM from backup version on pcb_A device




case 6:	unknown; break;											//1 signed integer													


}}}

/**********Modes 'T' and 't' are not available***************************/
/**********Modes 'U' and 'u' are not available***************************/	
	
	
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


