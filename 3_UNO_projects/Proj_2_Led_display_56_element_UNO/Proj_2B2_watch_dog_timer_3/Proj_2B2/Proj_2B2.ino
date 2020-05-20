
/*
*Proj_2B2_watch_dog_timer_3
*********************************************************************/

/*SNOW STORM DISPLAY VERSION 3


IT INTRODUCES:


1.  Interrupt Service Routine "ISR (WDT_vect){}":
This ISR is called by the watch dog timer when it is operating in its Interrupt and System Reset Mode.

When the WDT times out it initially issues an interrupt and program execution jumps to "ISR (WDT_vect){}".
This is used here to save the PRN value to the EEPROM. The WDT then times out for a second time and resets 
the Atmega 168.  

2.  Project macros "set_Arduino_WDTout", "Arduino_non_WDTout" and "clear_Arduino_WDT_flag".  The first is 
called by the WDT ISR and writes to an EEPROM location. The second reads the same EEPROM location shortly
after reset to determine whether or not the reset was due to a WDTout.

3.  Project subroutines "Char_to_EEPROM()" used to save data to the EEPROM and "Char_from_EEPROM()" used to 
read the EEPROM.


Note:

For programs loaded using the Arduino bootloader MCUSR always reads as zero and therefore the reset mechanism
cannot be ddetermined in the usual way.
The display always starts with all ones when it is switched on or immediately after programming but 
continues from where it was when interrupted by the WDT.

Press sw_2 to operate the WDT.*/




#include "Proj_2B2_header_file.h"



unsigned int PRN;                                         //Global memory location used to store "pseudo random numbers"

int main (void){  
setup_UNO;                                                


One_25ms_WDT_with_interrupt;                              //Reconfigures the WDT for Interrupt and System Reset Mode

if(Arduino_non_WDTout) PRN = 0xFFFF;                     //Reset due to POR or post programming
else   PRN=read_display();                               //Reset due to WDTout: Read PRN from EEPROM

clear_Arduino_WDT_flag;

config_sw1_and_sw2_for_PCI;
sei();
while(1){                                                 //Infinite while loop
I2C_Tx_2_integers(PRN, (PRN<<1));                         //Display two "pseudo random numbers"
PRN = PRN_16bit_GEN_UNO (PRN);                            //Generate a new PRN
Timer_T0_10mS_delay_x_m(10);                              //Pause before repeating
wdr();}}                                                  //Reset the watchdog timer


  

/****************************************************************************************/
ISR(PCINT2_vect) 
{if (switch_2_up)return; else {sei();while(1);}}        //If switch_2 is pressed put program execution on hold
                                                        //The watchdog timer will not be reset and will "time out"
                                                        //Note: Global interrupts are automatically disabled when 
                                                        //an ISR runs.  Hence the "sei()" command so that the 
                                                       //WDT interrupt can be triggered.

/****************************************************************************************/
unsigned int read_display(void){                    
return((Char_from_EEPROM(0x3F3) << 8) +                 //Read the upper 8 bits shift it 8 bits left
Char_from_EEPROM(0x3F2));}                              //then add the lower 8 bits
                                                        //return the result to PRN        

/****************************************************************************************/
ISR (WDT_vect){

set_Arduino_WDTout;
Char_to_EEPROM(0x3F3, PRN >> 8);                        //Write the upper 8 bits of the PRN to EEPROM address 0x1FF 
Char_to_EEPROM(0x3F2, PRN); }                           //Write the lower 8 bits to address 0x1FE
