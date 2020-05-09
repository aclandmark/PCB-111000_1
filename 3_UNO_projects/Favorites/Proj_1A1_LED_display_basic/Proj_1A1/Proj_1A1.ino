
/*
Proj_1A1_LED_display_basic.c

A basic first program.

It introduces the following project resources:
    
    Subroutines:  
      I2C_Tx_2_integers(A,B)
      This displays two numbers in binary form
   
      Timer_T1_sub() 
      This pauses program execution

    Macro Set_up_HW which initialises
      The watch dog timer
      I2C slave
      Switched inputs
      The UART
      Initialises default programs
      Etc.
    
    Note: there is a simplified version 
      Set_up_HW_basic
      This omits the default programs


Note: Click on Tools, Options, Environment, Fonts and Colours, 
Scroll down Display items to VA brace matching and choose a 
bright colour such as red.  This makes programs much easier to read.
*/







#include "Proj_1A1_header_file.h"

int main (void){

  unsigned long PORT_1, PORT_2;

  setup_UNO;

  while(1)
  {PORT_1=1; PORT_2 = 0x80000000;

    for(int m = 1; m < 33; m++){
      
      I2C_Tx_2_integers(PORT_1, PORT_2);

      Timer_T1_sub(T1_delay_50ms);
      PORT_1 = PORT_1 << 1;
      PORT_2 = PORT_2 >> 1;
    }
  }}
