


/********************************************************************************************************************************/
void save_cal_values(unsigned char OSCCAL_user){
eeprom_write_byte((uint8_t*)(EE_size - 1), OSCCAL_user); 
eeprom_write_byte((uint8_t*)(EE_size - 2), OSCCAL_user); 
eeprom_write_byte((uint8_t*)(EE_size - 3), OSCCAL_DV);}




/********************************************************************************************************************************/
void printout_cal_values(void){
sendString("\r\nValues saved to EEPROM  ");	
sendLongNum(10,eeprom_read_byte((uint8_t*)EE_size-1)); sendChar('\t');
sendLongNum(10,eeprom_read_byte((uint8_t*)EE_size-2)); sendString("\tUser value\r\n\t\t\t");
sendLongNum(10,eeprom_read_byte((uint8_t*)EE_size-3)); sendString("\t\tDefault value\r\n");}





/******************************************************************************************************************************/
void initialise_timers_for_cal_error(void){
TCNT1=0;TCCR1B = 0;													//Reset and halt T1
TCCR2B =  0x0;	while(ASSR & (1 << TCR2BUB));						//Halt T2
TCCR2A = 0; while(ASSR & (1 << TCR2AUB));							//Reset T2 
TCNT2=0; while(ASSR & (1 << TCN2UB));	}							//Reset TCNT2




/*********************************************************************************************************************************/
void start_timers_for_cal_error(void)
{TCCR2B = 1; 	
while(ASSR & (1 << TCR2BUB));
TCCR1B = 1;}


/**************************************************************************************************************************************/		
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
if (local_error_mode){}
return error/Num_2;}
