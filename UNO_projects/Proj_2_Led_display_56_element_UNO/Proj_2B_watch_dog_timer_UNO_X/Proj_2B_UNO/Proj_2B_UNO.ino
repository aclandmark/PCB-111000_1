




#include "Proj_2B_header_file.h"



int main (void){  
unsigned int PRN;                             //Memory location used to store "pseudo random numbers"

setup_HW;
wdt_enable(WDTO_250MS);                       //Set up the watchdog timer to generate a reset after 250mS.
config_sw1_and_sw2_for_PCI;                   //SW1 is not used
sei();
while(1){                                     //Infinite while loop 
PRN = PRN_16bit_GEN (0);                      //Generate a new PRN (0) tells subroutine to use the EEPROM
I2C_Tx_2_integers(PRN, (PRN<<1));             //Display two "pseudo random numbers"
Timer_T0_10mS_delay_x_m(10);                  //Pause before repeating
wdr();}}                                      //Reset the watchdog timer which avoids the possibility
                                              //of a reset for another 250mS

ISR(PCINT2_vect) 
{if (switch_2_up)return; else while(1);}       //If switch_3 is pressed put program execution on hold
                                               //The watchdog timer will not be reset and will "time out"
