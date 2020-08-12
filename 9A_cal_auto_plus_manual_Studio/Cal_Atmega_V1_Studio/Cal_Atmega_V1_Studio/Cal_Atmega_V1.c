

#include "Cal_Atmega_V1_header_file.h"


int main (void)

{setup_HW_basic;

	ASSR = (1 << AS2);                         //initialise T2 for crystal
	OSCCAL_DV = OSCCAL;                         //Save default value off OSCCAL

	Auto_cal ();

	sendString("\r\nAuto_cal running:\r\n\
	Device being calibrated? Press 0, 1, 2, 3 \
	for Atmega 328/P, 168/P, 88/P or 48/P\r\n\r\n");

	set_device_type_and_memory_size;
	save_cal_values(OSCCAL_WV);                     //Save result of the auto calibration

	sendString("\r\nNew OSCCAL value ");
	sendLongNum(10,OSCCAL_WV);

	sendString("\r\nPress 'x' to finnish or AOK for manal cal\r\n");
	if (waitforkeypress() == 'x'){
		save_cal_values(OSCCAL_WV);
	printout_cal_values();}

	else Manual_cal();

	sendString("\r\nTest");
	sendString("\r\nValues saved to EEPROM  ");
	sendLongNum(10,eeprom_read_byte((uint8_t*)(0x3FF))); sendChar('\t');
	sendLongNum(10,eeprom_read_byte((uint8_t*)(0x3FE))); sendString("\tUser value\r\n\t\t\t");
	sendLongNum(10,eeprom_read_byte((uint8_t*)(0x3FD))); sendString("\t\tDefault value\r\n");

	sendString("\r\nAll done: AK to repeat.\r\n");

	waitforkeypress();

SW_reset;}




/************************************************************************************************/
void Auto_cal (void){
	unsigned char counter_1, counter_2;
	unsigned char OSCCAL_mem = 0;
	long  error_mag;
	int limit;

	UCSR0B &= (~(1 << RXEN0));                        //avoid unwanted keypresses

	Timer_T1_sub(T1_delay_1sec);                      //Crystal warm up time

	sei();
	cal_mode = 1;
	Get_ready_to_calibrate;
	
	counter_1 = 0xF1;
	while(1){sendChar('.');
		counter_1 -= 1;
		OSCCAL = counter_1; error_mag = compute_error(0,cal_mode,0);
		if(counter_1 > 0xE0)continue;
	if(error_mag < 1000)break;}

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

	UCSR0B |= (1 << RXEN0);
cli();}





/**********************************************************************************************************************/
void Manual_cal(void){
	long cal_error;
	unsigned char osccal_MIN;
	long percentage_error;

	UCSR0B &= (~(1 << RXEN0));                      //avoid unwanted keypresses
	sendString("\r\nManual cal: please wait 5 seconds\r\n");
	Timer_T0_sub(T0_delay_5ms);                     //gives printer time to finnish

	sei();

	EA_buff_ptr = 0;
	cal_mode = 5;
	Get_ready_to_calibrate;
	OSCCAL -=20;
	osccal_MIN = OSCCAL;                        //Compute cal error for 41 values of OSCCAL
	for(int m = 0; m <= 40; m++)
	{cal_error = compute_error(1,cal_mode,1);OSCCAL++;}
	OSCCAL = OSCCAL_WV;                         //Restore working value of OSCCAL
	close_calibration;

	newline();
	sendString("OSCCAL\t Error\r\n");


	for(int m = 0; m <= 40; m++)                    //Print table of values
	{sendLongNum(10,osccal_MIN); osccal_MIN++;
		sendChar('\t');sendLongNum(10,buffer[m]);
		sendChar('\t');
		percentage_error = buffer[m];
		sendLongNum(10,percentage_error*100/62500);sendChar('%');
		newline();
	Timer_T0_sub(T0_delay_20ms);}                   //stops PC being overwhelmed with data

	sendString("\r\nEnter new user cal value or x to escape");


	UCSR0B |= (1 << RXEN0);                     //keypresses required again

	if ((OSCCAL_UV = UC_from_KBD()) == 'x')
	OSCCAL_UV = OSCCAL_WV;                        //stick with result of autocal

	
	/*********************************************/
	else
	{Get_ready_to_calibrate;                      //Test value of OSCCAL entered by user
		OSCCAL = OSCCAL_UV;                         //Test new OSCCAL value
		calibrate_without_sign_plus_warm_up_time;
		close_calibration;

		if(cal_error > 1750)                        //Error resulting from User OSCCAL exceeds 1750
		{OSCCAL = OSCCAL_WV;                        //Reinstate working value
			OSCCAL_UV = OSCCAL_WV;
		sendString("\tChange rejected: error too great");}}


		cli();
		save_cal_values(OSCCAL_UV);
	printout_cal_values();}







	/*************************************************************************************************************************************/
	ISR(TIMER2_OVF_vect) {
		long TCNT1_BKP, overflow = 0, target_res;

		target_res = 62500;
		TCCR1B = 0;                               //Halt T1
		TCNT1_BKP = TCNT1;                            //Copy the value of TCNT1
		TCNT1 = 0;                                //Clear TCNT1
		TCCR1B = 1;                               //Get T1 running again ASAP (Note T2 has not stopped running)
		//Get T1 running again ASAP (Note T2 has not stopped running)
		if(EA_counter < cal_mode)T1_OVF = 0;                  //Ignore first few results
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