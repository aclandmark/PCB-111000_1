/*
 Proj_1C1_Target_practice.c
 
 An attempt to introduce a bit of fun
 Switch 2 is the gun, the leds are the targets
 
 
 It also introduces two macros
  config_sw1_and_sw2_for_PCI; (Pin Change Interrupt) 
  This is used to detect switch presses
  
  SW_reset
  This pauses program flow for 30mS and then triggers 
  a watch dog time out which resets the program

 Note:
 The logic may look a bit tricky at first, but its not really what's important here.
 */



#include "Proj_1C1_header_file.h"


volatile unsigned int PORT_1, mask;
char switch_control;

int main (void){
  setup_UNO;
    
  config_sw1_and_sw2_for_PCI;
  disable_pci_on_sw1
  
  mask = 0xFFFF;                                  //"mask" keeps a record of which leds have been shot down
  switch_control = 0;                             //This enables the gun when set to zero
  sei();
  while(mask){                                    //Keep looping untill all leds have been shot down
    PORT_1=1;                                     //Start by illuminating most right hand leds

    for(int m = 1; m < 17; m++){                  //Illuminate all 16 sets of leds in turn
      if(m == 1)switch_control = 0;               //Enable the gun
      I2C_Tx_2_integers\
      (PORT_1 & mask, (~mask) ^ PORT_1);          //Illumination of leds depends upon which have been shot down        
      
      Timer_T0_10mS_delay_x_m(10);                //100mS delay
      PORT_1 = (PORT_1 << 1);}}                   //Move on to next target and eventuallly repeat the while loop.
      
    I2C_Tx_2_integers(0, 0xFFFF);                 //Hold display for 1 second before restarting
    Timer_T0_10mS_delay_x_m(100);
  SW_reset;}



  /**************Routine executed every time that switch is  operated**************************************/
                                              
  ISR(PCINT2_vect) {                          //The ISR is called when the trigger is operated
    if(switch_2_up)return;                    //Only switch presses are significant, switch releases are ignored
    if (switch_control) return;               //The shot is ignored if the gun has been dissabled
    mask &= ~PORT_1;                          //Otherwise it writes zero to the "mask" register at the target location
  switch_control = 1;}                        //and temporarily dissables the gun


  
