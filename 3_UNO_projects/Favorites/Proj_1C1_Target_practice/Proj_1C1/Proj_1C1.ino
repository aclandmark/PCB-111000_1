/*
 Proj_1C1_Target_practice.c
 
 An attempt to introduce a bit of fun
 
 It introduces two macros
  config_sw2_for_PCI; (Pin Change Interrupt) used to detect switch presses
  SW_reset; This triggers a watch dog time out
 */



#include "Proj_1C1_header_file.h"



volatile unsigned int PORT_1, mask;
char switch_control;

int main (void){
  setup_UNO;
  
  
  config_sw1_and_sw2_for_PCI;
  disable_pci_on_sw1
  
  
  
  
  mask = 0xFFFF;
  switch_control = 0;
  sei();
  while(mask){
    PORT_1=1;

    for(int m = 1; m < 17; m++){
      if(m == 1)switch_control = 0;
      I2C_Tx_2_integers(PORT_1 & mask, (~mask) ^ PORT_1);
      
      Timer_T0_10mS_delay_x_m(10);
      PORT_1 = (PORT_1 << 1);
    }}I2C_Tx_2_integers(0, 0xFFFF);
    Timer_T0_10mS_delay_x_m(100);
  SW_reset;}



  /**************Routine executed every time that switch is  operated**************************************/
  
  ISR(PCINT2_vect) {                          //This ISR momentarily interrupts the main routine
    if(switch_2_up)return;                    //It notes which LED has just been shot down and
    if (switch_control) return;               //Return if "switch_control" is not zero
    mask &= ~PORT_1;                          //writes zero at its location in the "mask" register
  switch_control = 1;}



  
