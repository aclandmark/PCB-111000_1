

#include "Proj_1A_header_file.h" 
#include "Proj_1A_subroutines.c" 



int main (void){	

unsigned int PORT_1=1;
		
setup_HW;


for(int m = 1; m < 17; m++){										

		I2C_Tx_2_integers(PORT_1, PORT_1);				

		Timer_T1_sub(T1_delay_100ms);			
		PORT_1 = PORT_1 << 1;		
		}SW_reset;

	
}													

