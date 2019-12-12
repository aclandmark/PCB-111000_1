
void initialise_timers_for_cal_error(void);
void start_timers_for_cal_error(void);
long compute_error(char);	
long calibrate_quick_cal(char);
void start_T2_for_ATMEGA_168_cal(char);
long emergency_restore_operation(void);	
void Cal_at_Power_on_Reset (void);
void I2C_master_transmit(char);


#define Get_ready_to_calibrate; \
TIMSK2 |= (1 << TOIE2);\
TIMSK1 |= (1 << TOIE1);\
initialise_timers_for_cal_error();\
start_timers_for_cal_error();

#define close_calibration; \
initialise_timers_for_cal_error();\
TIMSK2 &= (~(1 << TOIE2));\
TIMSK1 &= (~(1 << TOIE1));

#define calibrate_plus_warm_up_time; \
cal_error = compute_error(0);\
cal_error = compute_error(0);\
cal_error = compute_error(0);

		/************************************************************************************/
		void initialise_timers_for_cal_error(void){
		TCNT1=0;TCCR1B = 0;									//Reset and halt T1
		TCCR2B =  0x0;	while(ASSR & (1 << TCR2BUB));		//Halt T2
		TCCR2A = 0; while(ASSR & (1 << TCR2AUB));			//Reset T2 
		TCNT2=0; while(ASSR & (1 << TCN2UB));	}			//Reset TCNT2



		/************************************************************************************/
		void start_timers_for_cal_error(void)
		{TCCR2B = 1; 	
		while(ASSR & (1 << TCR2BUB));
		TCCR1B = 1;}
		


		/************************************************************************************/
		long compute_error(char error_mode)		
		{long error;
		EA_counter = 0;										//Compute error for each value of OSCCAL 10 times
		error_SUM = 0;
		while(EA_counter < 15);EA_counter = 0;
		error = error_SUM;
		if (error < 0) error *= (-1);
		if (error_mode)
		{buffer[EA_buff_ptr] = error/10;EA_buff_ptr++;}
		return error/10;}
		
	

	/************************************************************************************/
		long calibrate_quick_cal(char OSCCAL_test)
		{long cal_error, error_mem;	
		T1_OVF=0;
		OSCCAL = OSCCAL_test;		
		cal_error = compute_error(0);						//compute error for OSCCAL_test
		error_mem = cal_error;
		OSCCAL++;											//increment OSCALL
		while(1){
		cal_error = compute_error(0);						//Calculate new error
		if (cal_error > error_mem) { 										//If it gets worse
		OSCCAL--; break;}									//decrement OSCCAL and exit							
		error_mem = cal_error;							
		OSCCAL++;}
		if (OSCCAL != OSCCAL_test)	return error_mem;		//REPLACE with OSCCAL_DV when OSCCAL_test is discarded
		else{												//OSCCAL is still the default value
		OSCCAL--;
		while(1){
		cal_error = compute_error(0);
		if (cal_error > error_mem){ 						//If it gets worse
		OSCCAL++;
		break;}												//increment OSCCAL and exit
		error_mem = cal_error;							
		OSCCAL--;}}
		return error_mem;}
		
	

	/************************************************************************************/
		void start_T2_for_ATMEGA_168_cal(char cal_mode){
		TCCR2B =  0x0;	while(ASSR & (1 << TCR2BUB));		//Halt T2
		TCCR2A = 0; while(ASSR & (1 << TCR2AUB));			//Reset T2 
		TCNT2=0; while(ASSR & (1 << TCN2UB));
		if(cal_mode){
		TIMSK2 |= (1 << TOIE2);							//Set Timer 2: interrupt on overflow
		TCCR2B = 1; 										//Start T2 with a clock of 32.768Hz
		while(ASSR & (1 << TCR2BUB));}
		else{TIMSK2 &= (~(1 << TOIE2));mode = 0;}}
		
	

	/************************************************************************************/
		long emergency_restore_operation(void)	
		{long error;
		EA_counter = 0;										
		error_SUM = 0;
		while(EA_counter < 2);EA_counter = 0;
		error = error_SUM;
		return error;}
		
	

	/************************************************************************************/
		void Cal_at_Power_on_Reset (void){
		long error_mag;
		long cal_error;
		char OSCCAL_init;
		
		sei();
		Timer_T1_sub(T1_delay_500ms);
		MCUSR &= (~(1 << PORF));
		
		mode = 'T';											//Required by ISR(TIMER1_OVF_vect) & (TIMER2_OVF_vect)
		Get_ready_to_calibrate;
		calibrate_plus_warm_up_time;
		
		if(cal_error > 1250){								//Rough clock calibration for error in excess of 1250
		mode = 'U';											//UC with error > 1250 ensures that calibration 
		initialise_timers_for_cal_error();					//centres on the lower of the two possible values for OSCCAL
		start_timers_for_cal_error();
		for(int m = 0x10; m <= 0xF0; m++)					//sample OSCCAL values from 0x10 upwards utill error < 1250
		{OSCCAL = m ;
		cal_error = emergency_restore_operation();			//low accurracy but quick to scan up to 224 cal factors
		if (cal_error >= 0) error_mag = cal_error; 
		else error_mag = cal_error * (-1);
		if ((error_mag < 1000) && (m > 0x14))break;}}		//Warmup time for first 4 vaues of m 
		
		mode = 'O';											//Fine clock calibration
		OSCCAL_init = OSCCAL;
		initialise_timers_for_cal_error();  				
		start_timers_for_cal_error();
		cal_error = calibrate_quick_cal(OSCCAL);			//Full accuracy but only tests several cal factors
		if(cal_error > 500)
		cal_error = calibrate_quick_cal(OSCCAL + 4);
		if(cal_error > 500)
		cal_error = calibrate_quick_cal(OSCCAL - 2);	
		if(cal_error > 500) OSCCAL = OSCCAL_init;		
		
		eeprom_write_byte((uint8_t*)0x3FE, OSCCAL); 		//Save final result to EEPROM
		eeprom_write_byte((uint8_t*)0x3FF, OSCCAL);
		
		close_calibration;
		cli();	}
		
		
	/************************************************************************************/	
	void manual_cal_PCB_A_device(void){
		if (I2C_data[0] == 1){								//Manual calibration if cal_mode is 1
		EA_buff_ptr = 0;
		
		Get_ready_to_calibrate;
		OSCCAL -=20;										//Compute cal error for 41 values of OSCCAL
		for(int m = 0; m <= 40; m++)
		{cal_error = compute_error(1);OSCCAL++;}
		OSCCAL = OSCCAL_WV;
		close_calibration;
		
Initialise_I2C_master_write;								//Transmit error values to user
	I2C_master_transmit(OSCCAL_DV);
	I2C_master_transmit(OSCCAL_WV - 20);						
	for(int m = 0; m <= 40; m++){							
	I2C_master_transmit(buffer[m] >> 8);
	I2C_master_transmit(buffer[m]);}
	I2C_master_transmit	(OSCCAL_WV);
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);	
	
	Initialise_I2C_master_read;								//Read cal byte entered by user
	OSCCAL_UC = I2C_master_receive(0);							
	TWCR = (1 << TWINT ) | (1 << TWEN ) | (1 << TWSTO );
	
	/*********************************************/
	Get_ready_to_calibrate;									//Test value of OSCCAL entered by user
	if(OSCCAL_UC == 0xFF)OSCCAL = OSCCAL_DV;
	else OSCCAL = OSCCAL_UC;								//OSCCAL test value
	calibrate_plus_warm_up_time;								
	close_calibration;
	
	
	Initialise_I2C_master_write;							//Report results to user
	if(cal_error > 1750)									//Error resulting from User OSCCAL exceeds 1750
	{I2C_master_transmit('X');								//Reject result
	I2C_master_transmit(cal_error >> 8);						
	I2C_master_transmit(cal_error);
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	eeprom_write_byte((uint8_t*)0x3FE, 0xFF); 				//Reset OSCCAL values storred in EEPROM
	eeprom_write_byte((uint8_t*)0x3FF, 0xFF);
	OSCCAL = OSCCAL_DV;
	OSCCAL_WV = OSCCAL_DV;
	return;}												//Exit from calibration mode
	
	/*************************************************/
	else{I2C_master_transmit('Y');							//Error resulting from User OSCCAL is less than 1750
	
	eeprom_write_byte((uint8_t*)0x3FE, OSCCAL_UC); 		//save user OSCCAL to EEPROM
	eeprom_write_byte((uint8_t*)0x3FF, OSCCAL_UC); 
	if(OSCCAL_UC == 0xFF) OSCCAL_WV = OSCCAL_DV;
	else {OSCCAL = OSCCAL_UC;  OSCCAL_WV = OSCCAL; }	
	
	TWDR = eeprom_read_byte((uint8_t*)0x3FE);				//Echo values saved to EEPROM to user
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	TWDR = eeprom_read_byte((uint8_t*)0x3FF);
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);}}


/*************************************************/	
		if (I2C_data[0] == 0){								//Check calibration for working value of OSCCAL: (OSCCAL_WV)									
		Get_ready_to_calibrate;
		calibrate_plus_warm_up_time;							
		close_calibration;

Initialise_I2C_master_write;
	I2C_master_transmit(OSCCAL);
	I2C_master_transmit(cal_error >> 8);
	I2C_master_transmit(cal_error);
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	}}

		
		
		
		