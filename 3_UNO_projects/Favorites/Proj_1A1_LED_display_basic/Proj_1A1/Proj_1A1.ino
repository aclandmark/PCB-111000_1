
/*
Proj_1A1_LED_display_basic.c

A basic first program.

It introduces the following project resources:
    
    Subroutines:  
      I2C_Tx_2_integers(A,B)
      This displays two numbers in binary form
   
      Timer_T1_sub() 
      This pauses program execution

    The macro "setup_UNO" which initialises
      The watch dog timer
      I2C slave
      Switched inputs
      Display brightness
      The UART
      Etc.
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
