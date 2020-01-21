
/*Proj_1B1_LED_display
**********************************************************

As Proj_1B: But uses project subroutine "waitforkeypress();" to increment the display.*/




#include "Proj_1B1_header_file.h"

int main (void){

unsigned int PORT_1=1;								
char m=1;											
char overflow=0;		
								
setup_HW;
while(1){	
		
		
	I2C_Tx_2_integers(PORT_1, ~PORT_1);		
	waitforkeypress();									
	if (m<=5){PORT_1 |= (PORT_1 << 1);m += 1;}			
	else PORT_1 = PORT_1 << 1;										
	if(overflow)PORT_1 |= 1;						
	if (PORT_1 & 0x8000) overflow=1;				
	else overflow = 0;
	}}		 
	
	



