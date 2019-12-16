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

//char  Ten_mS_tick_counter = 0; //clock_flag=0 PIC_cmd stop_watch_mode stop_watch_flag
//char timer_store [95];
signed char digit_num=0;
int  letter=0;//timer_record timer_pointer record_counter
char payload_size, sign;
long L_number;
signed char expnt;
int text_max, eeprom_ptr;
char string_counter, string_no, sign_bit;
char test_num, test_digit;
char OSCCAL_test;

char *SW_Version = "OS: I2C_V16_0_CC\r\n";
char *SW_info = "SW information: Operating system I2C_V16_0_CC\
  Projects V1_4 and Bootloader V4_22_CC. (zero_SUT & auto clock recovery) \
  External programmer V2_30A\r\n";
	


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

if (mode != 'B') display_mask=0;	//dissable flashing digits

if(payload_size){															//payload is zero for mode 'G'

for (int m = 0; m < payload_size; m++){								//payload_size = Num digits to be downloaded
	if (m == (payload_size-1))I2C_data[m] = I2C_master_receive(0);
	else I2C_data[m] = I2C_master_receive(1);}
	TWCR =  (1 << TWINT ) | (1 << TWEN ) | (1 << TWSTO );}				//Send stop bit	

switch(mode){
case 'a': case 'b': case 'B':case 'd': case 'D': case 'g': 
case 'G': case 'j': case 'J': case 'w': case 'W': break;
default: {for(int m = 0; m < 8; m++)display_buf[m] = 0;}break;}			//case B added

switch (mode){
case 'A':														//Called by subroutines I2C_Tx_any_segment(char segment, char digit_num)
case 'a':														//and I2C_Tx_any_segment_clear_all(void)
	letter = I2C_data[0];
	digit_num = I2C_data[1];
	switch (letter){
	case 'A': case 'a': letter = 0; break;
	case 'B': case 'b': letter = 3; break;
	case 'C': case 'c': letter = 4; break;
	case 'D': case 'd': letter = 2; break;
	case 'E': case 'e': letter = 5; break;
	case 'F': case 'f': letter = 6; break;
	case 'G': case 'g': letter = 1; break;
	default: letter = 8; break;	}
	if(letter == 8)break;
	if((digit_num >= 8) || (digit_num < 0))break;
	
	if (mode == 'A'){clear_digits;clear_display;}
	if (mode == 'a'){if(display_buf[letter] & (1 << digit_num))display_buf[letter] &= (~(1 << digit_num));
						else display_buf[letter] |= (1 << digit_num);} //toggles a segment
	break;
	
		
case 'B':												//Called by subroutine I2C_Tx_2URNs_from_IO();  (2 Unsigned Real numbers)
case 'b':	I2C_Tx_2URNs_from_IO ();break;				//Generates numbers of type 0.1234 With an exponent
	
case 'C':												//Called by subroutine I2C_Tx_Uarithmetic_OP()  														
case 'c': 	I2C_Tx_Uarithmetic_OP();break;				//Takes numbers entered using mode B and does some arithmetic

case 'D':												//Called by subroutines I2C_Tx_Initiate_tables()									
case 'd':	I2C_Tx_Compile_tables(); break;				//I2C_Tx_Inc_tables(),and I2C_Tx_dec_tables();		
	
case 'E':												//Called by subroutine I2C_Tx_accumulator_op()
case 'e':	I2C_Tx_accumulator_1();	break;				//Implements add, subtract and clear modes

case 'F':												//Called by subroutine I2C_Tx_accumulator_op()
case 'f':	I2C_Tx_accumulator_2();break;				//Aquires data for use by accumulator
	


case 'G':												//Called by subroutine I2C_initiate_10mS_ref(void)
case 'g':
	TCCR2B = 2;//= 6;									//Set clock to 4096Hz and start it
	TIMSK2 |= ((1 << OCIE2A) | (1 << TOV2));			//Enable timer clock interrupt
	wdt_enable(WDTO_120MS); break;						//Start WDT on 15mS
	
case 'H':												//Called by subroutine I2C_Tx_long(long)
case 'h':												//Receives and displays a long number
L_number = 0;
for (int m = 0; m < 4; m++){
L_number += I2C_data[m];
if(m<3) L_number = L_number << 8;}
Display_num(L_number);break;

case 'I':															//Called by subroutine I2C_Tx_float_num(long, char)
case 'i':															//Recieves scientific number and then scrolls it across the display
L_number = 0;
for (int m = 0; m < 4; m++){
L_number += I2C_data[m];
if(m<3) L_number = L_number << 8;}
expnt = I2C_data[4];							
float_to_askii(L_number, expnt,Sc_Num_string);
TCCR2B = 2;//= 6;												//Set clock to 4096Hz and start it
TIMSK2 |= (1 << TOIE2);	
break;

case 'J':															//Called by subroutine I2C_displayToLong(void)
case 'j':															//Converts display to long and transmits it to the slave
L_number=0;
sign = '+';
{int m = 7; while (!(display_buf[m]))m--;
if (display_buf[m] == '-') {sign = '-'; m--;}
while(m>=0){L_number = L_number*10 + display_buf[m] - '0'; m--;}}
if (sign == '-')L_number = L_number * (-1);
for(int m = 0; m <= 3; m++){result[3-m] = L_number; L_number = L_number >> 8;}
long_to_slave_I2C;
break;

case 'k':															//Called by subroutine I2C_Tx_real_num(long L_number)
case 'K':															//Displays a real number
L_number = 0;
for (int m = 0; m < 4; m++){
L_number += I2C_data[m];
if(m<3) L_number = L_number << 8;}
Display_real_num(L_number);break;

case'M':															//Called by subroutine I2C_Rx_get_version(char str_type)
case'm':
switch (I2C_data[0]){
case '0': string_to_slave_I2C(SW_Version); break;
case '1': string_to_slave_I2C(SW_info); break;
default: break;}
break;

case'N':													//caled by I2C_Tx(1, 'N', &cal_mode);													
case'n': manual_cal_PCB_A_device();

break;
	
case'O':								//called by I2C_Tx(1, 'O', &cal_mode)															
case'o':								//performs a quick cal. Note: cal_mode is not used
	
	OSCCAL_test = OSCCAL;
	//ASSR = (1 << AS2);					//Set T2 for use with crystal
	TIMSK2 |= (1 << TOIE2);				//Set Timer 2: interrupt on overflow
	TIMSK1 |= (1 << TOIE1);
	initialise_timers_for_cal_error();
	start_timers_for_cal_error();
	
		cal_error = compute_error(0);						//Repeat 3 times to ovecome
		cal_error = compute_error(0);						//warm up error
		cal_error = compute_error(0);						
	
	cal_error = calibrate_quick_cal(OSCCAL_test);				//Adjusts OSCCAL
	OSCCAL_WV = OSCCAL;
	initialise_timers_for_cal_error();  				//dissable timers
	TIMSK2 &= (~(1 << TOIE2));							//Dissable Timer 2: interrupt on overflow
	TIMSK1 &= (~(1 << TOIE1));
	Initialise_I2C_master_write;
	I2C_master_transmit(OSCCAL_WV);
	I2C_master_transmit(cal_error >> 8);
	I2C_master_transmit(cal_error);
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);	
	break;

case'P':																							
case'p':	start_T2_for_ATMEGA_168_cal(1);	break;			//Generates 7_8125mS TWI slave interrupt  

case 'Q':
case 'q':	MUX_cntl = I2C_data[0];							//Explore operation of multiplexer
			
			if(eeprom_read_byte((uint8_t*)0x3FB) == 0xFF){
			switch (MUX_cntl){
			case 0:	timer_T0_sub_with_interrupt(T0_delay_2ms);break;
			case 1: timer_T0_sub_with_interrupt(T0_delay_4ms);break;
			case 2:	timer_T0_sub_with_interrupt(T0_delay_6ms);break;
			case 3: timer_T0_sub_with_interrupt(T0_delay_8ms);break;
			case 4:	timer_T0_sub_with_interrupt(T0_delay_10ms);break;
			case 5: timer_T0_sub_with_interrupt(T0_delay_12ms);break;
			case 6: timer_T0_sub_with_interrupt(T0_delay_20ms);break;
			case 7: timer_T0_sub_with_interrupt(T0_delay_33ms);break;
			default: timer_T0_sub_with_interrupt(T0_delay_2ms);break;}}
			
			else{T0_interupt_cnt = 0;
			switch (MUX_cntl){
			case 0:	timer_T0_sub_with_interrupt(T0_delay_500us);break;
			case 1: timer_T0_sub_with_interrupt(T0_delay_1ms);break;
			case 2:	timer_T0_sub_with_interrupt(T0_delay_1500us);break;
			case 3: timer_T0_sub_with_interrupt(T0_delay_2ms);break;
			case 4:	timer_T0_sub_with_interrupt(T0_delay_2500us);break;
			case 5: timer_T0_sub_with_interrupt(T0_delay_3ms);break;
			case 6: timer_T0_sub_with_interrupt(T0_delay_5ms);break;
			case 7: timer_T0_sub_with_interrupt(T0_delay_8ms);break;
			default: timer_T0_sub_with_interrupt(T0_delay_500us);break;}}

			for(int p = 0; p <= 7; p++)display_buf[p] = p + '0';
			break;

case 'R':												//For manufacturint test
case 'r':
		TIMSK0 &= (~(1 << TOIE0));						//Disable Multiplexer
		clear_digits; clear_display;
		Initialise_I2C_master_read;
		test_num = I2C_master_receive(1);
		switch(test_num){
		case '0': zero; break;
		case '1': one; break;
		case '2': two; break;
		case '3': three; break;
		case '4': four; break;
		case '5': five; break;
		case '6': six; break;
		case '7': seven; break;
		case '8': eight; break;
		case '9': nine; break;}
		test_digit = I2C_master_receive(0);
		switch(test_digit){
		case '0': digit_0; break;
		case '1': digit_1; break;
		case '2': digit_2; break;
		case '3': digit_3; break;
		case '4': digit_4; break;
		case '5': digit_5; break;
		case '6': digit_6; break;
		case '7': digit_7; break;}
		TWCR =  (1 << TWINT ) | (1 << TWEN ) | (1 << TWSTO );		//Send stop bit	
		break;


case 'S': case 's':											//Scans 328 cal-factor fronmm 0x10 to 0xF0
		OSCCAL_DV = eeprom_read_byte((uint8_t*)0x3FD);
		for(int m = 0x10; m <= 0xF0; m++){
		TIMSK2 |= (1 << TOIE2);								//Set Timer 2: interrupt on overflow
		TIMSK1 |= (1 << TOIE1);
		initialise_timers_for_cal_error();
		start_timers_for_cal_error();
		OSCCAL = m;
		cal_error = compute_error(0);
		OSCCAL = OSCCAL_DV;
		initialise_timers_for_cal_error();  				//dissable timers
		TIMSK2 &= (~(1 << TOIE2));							//Dissable Timer 2: interrupt on overflow
		TIMSK1 &= (~(1 << TOIE1));
			 	
		Initialise_I2C_master_write;
		I2C_master_transmit(m);
		if(cal_error <= 1500){
		I2C_master_transmit(cal_error >> 8);
		I2C_master_transmit(cal_error);}
		else{
		I2C_master_transmit(10000 >> 8);
		I2C_master_transmit(0);	}
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);}
		
		Initialise_I2C_master_write;
		I2C_master_transmit(0xFE);
		I2C_master_transmit(0xFF);
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);break;

/**********Modes 'T' and 't' are not available***************************/
/**********Modes 'U' and 'u' are not available***************************/

case 'W':																	
case 'w':

if(I2C_data[0] == 1)
	{if(eeprom_read_byte((uint8_t*)0x3FB) != 0x01)
	{eeprom_write_byte((uint8_t*)0x3FB, 0x01);}							//Reduces LED intensity
	else {eeprom_write_byte((uint8_t*)0x3FB, 0xFF);}}break;				//Full brightness


case 'x':																	//0x3FC is set to 1 when the user enters 'x' at the p/r prompt
case 'X':	Initialise_I2C_master_write;									//The means that diagnostic tests are to be carried out
			I2C_master_transmit(eeprom_read_byte((uint8_t*)(0x3FC)));		//Including version info and calibration checks
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
			eeprom_write_byte((uint8_t*)0x3FC, 0x0);break;
case 'y':																				//Called by "Proj_6H_message_from_the_OS"
case 'Y':																				//Reads messages stored in AT328 EEPROM up to address 0x200
text_max = (eeprom_read_byte((uint8_t*)(0)) <<8) + eeprom_read_byte((uint8_t*)(1));
string_counter = 0;
eeprom_ptr = 5;

while(eeprom_ptr <= text_max){

if (!(eeprom_read_byte((uint8_t*)(eeprom_ptr))))string_counter++;
eeprom_ptr++;}
	
	string_counter -=1;
	Initialise_I2C_master_write;
	I2C_master_transmit(string_counter);
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);			//Transmit stop NOT actually needed
	Initialise_I2C_master_read;
	
	string_no = I2C_master_receive(0);					//One data item only										//Receive data from slave
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);			//Transmit stop NOT actually needed
	
if(!(string_no))string_no++;
string_counter=1;
eeprom_ptr = 5;

while(1){if(string_counter==string_no)break;
		else if (!(eeprom_read_byte((uint8_t*)(eeprom_ptr))))string_counter++;
		eeprom_ptr++;}

	Initialise_I2C_master_write;

	do{
	I2C_master_transmit(eeprom_read_byte((uint8_t*)eeprom_ptr));
	eeprom_ptr++;}while(eeprom_read_byte((uint8_t*)eeprom_ptr));
	I2C_master_transmit('\0');
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);			//Transmit stop NOT actually needed
break;

case 'z':														//Called by Read_AT328_EEPROM(char Op) starting at address 0x200
case 'Z':														//Used to restore AT168 EEPROM from backup version on AT328
																//So that it can be used as a programmer
	Initialise_I2C_master_write;
	
	{int m = 0x200;
	while (m <= 0x3F6)	
	{I2C_master_transmit(eeprom_read_byte((uint8_t*)m));
	m++;}	
	I2C_master_transmit(eeprom_read_byte((uint8_t*)0x3FE));
	I2C_master_transmit(eeprom_read_byte((uint8_t*)0x3FF));
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);}				//Transmit stop NOT actually needed
break;


case 1:																//Called by subroutine I2C_Tx_8_byte_array(char s[])
case 3:																//Called by subroutine I2C_Tx_BWops(char*)
for (int m = 0; m < 8; m++){	
	display_buf[m] = I2C_data[m];}										//Store digits for display
	break;
case 4:																//Called by subroutine I2C_Tx_2_integers((unsigned int s1, unsigned int s2))
for (int m = 0; m < 4; m++){										//Drives the vertical display segments
	display_buf[m] = I2C_data[m];}							
	for (int m = 4; m < 8; m++)display_buf[m] = 0;
	break;
case 5:																//Called by subroutine I2C_Tx_display_char (char, char)															
	output_2 =  I2C_data[0];										//Demonstrates +/- char as symbol and binary
	sign_bit = I2C_data[1];
	for (int m = 0; m < 8; m++)display_buf[m] = 0;
	if (sign_bit == 's') {Disp_CharS (output_2);}
	if (sign_bit == 'u') {Disp_CharU (output_2);}
	break;
case 6:																		//1 signed integer													
	for (int m = 0; m < 2; m++){								
	display_buf[m] = I2C_data[m];}										//Store digits for display	
	display_buf[2] = display_buf[0];display_buf[3] = display_buf[1];
	for (int m = 4; m < 8; m++)display_buf[m] = 0;
	break;


case 7: basic_clock(); break;											//I2C_Tx_OS_timer(AT_clock_mode, start_time)

case 8: 	multi_mode_clock(); break;									//Used with subroutine I2C_Tx_Clock_command(char timer_mode, char command)

case 9:		stop_watch(); break;										//Used by I2C_Tx_Clock_command(one100ms_mode) and I2C_Tx_Clock_command(ten_ms_mode)


	}}}

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


