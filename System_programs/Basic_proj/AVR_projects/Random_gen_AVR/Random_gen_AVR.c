
/*
Random display based on Pseudo Random Number PRN generator.
Default user project loaded onto UNO at address zero to ensure that there is always something to run.
To rienstate the default project press D at the h/r/t prompt.

Note: EEPROM locations 0x3F0 to 0x3FF are reserved.
Therefore the PRN generator uses locations 0x3EF and 0x3EE to store the next number


*/




/*****************************************************************************/

#include "Random_gen_header_file.h"
#include "Random_gen_subroutines.c"



int main (void){	
unsigned int PRN;



CLKPR = (1 << CLKPCE);
CLKPR = (1 << CLKPS0);

		
setup_HW;


while(1){	
PRN = PRN_16bit_GEN (0);									//Generate a new PRN (0) tells subroutine to use the EEPROM
I2C_Tx_2_integers(PRN, (PRN<<1));							//Display two "pseudo random numbers"
Timer_T1_sub(T1_delay_100ms);	
}}													



/************************************************************************************/
void Timer_T1_sub(char Counter_speed, unsigned int Start_point){ 
TCNT1H = (Start_point >> 8);
TCNT1L = Start_point;
TCCR1B = Counter_speed;
while(!(TIFR1 & (1<<TOV1)));
TIFR1 |= (1<<TOV1); 
TCCR1B = 0;}




/************************************************************************************/
unsigned int PRN_16bit_GEN(unsigned int start){
unsigned int bit, lfsr;

if(!(start)) lfsr = (eeprom_read_byte((uint8_t*)(0x3EF)) << 8) + eeprom_read_byte((uint8_t*)(0x3EE));
else lfsr = start;
bit = (( lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
lfsr = (lfsr >> 1) | (bit << 15);
if(!(start)){
eeprom_write_byte((uint8_t*)(0x3EF),(lfsr>>8));
eeprom_write_byte((uint8_t*)(0x3EE),lfsr);}

return lfsr;}
	
