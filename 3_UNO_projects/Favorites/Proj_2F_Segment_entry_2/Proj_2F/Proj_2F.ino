
  
  /*Proj_2F_Segment_entry_2
  *******************************************************************************/

  /*DISPLAYING NUMBERS USING MANUAL ILLUMINATION OF SEGMENTS

Demonstrates one step in the process of displaying numbers 
    
  It Introduces 
  
  1. Subroutine isCharavailable()
  This waits several mS and returns 1 if a character has been received.
  Subroutine "receiveChar()" can then be used to acquire it
  
  2. Macro "User_prompt":
  This sends the following character string to the PC "R     R      R      R........"
  The user responds with keypress "r" or "R".

  3.  Macro "SW_reset_with_interrupt".  
      When the watch dog times out after 30mS the program is not immediatey reset.  
      Instead an interrupt is generated. The ISR writes to EEPROM.  The reset is 
      generated as the program exits from the ISR.

  4.  Project macros 
      "set_Arduino_WDTout"        This writes to the EEPROM when the WDT ISR is called
      "Arduino_non_WDTout"        This reads the EEPROM to determine whether or not the reset is due to a WDTimeout.      
      "clear_Arduino_WDT_flag"    This clears the EEPROM
      
  
  Having downloaded the program start up a terminal program and pres 'r' at the user prompt
  
  */


  #include "Proj_2F_header_file.h"

  #define message_1 "For each digit press 0 to 7 then a to g for segments."
  #define message_2 "Again!"


  int main (void){

  char keypress = 0, digit_num=0;

  setup_UNO;

  if (Arduino_non_WDTout)                                 //Reset due to POR or immediately after being programmed
  {User_prompt;                                           //Display the user prompt and
   String_to_PC(message_1);}                              //full start-up message
  
  else {clear_Arduino_WDT_flag;                          //Reset due to watch dog time out 
  newline();String_to_PC(message_2);                      //Send abbreviated message and no user prompt
  
  I2C_Tx_any_segment('h', 0);}                          //Restore display after reset

  do{                                                   //Keep inputting segment/digit data until x is received
  while(1){if(isCharavailable(10))
  {keypress = receiveChar();break;}}                   //"isCharavailable" is repeatedly executed until a character is received
                                                        //when the "break" command allows program flow to escape from the "while(1)" loop

  switch(keypress){                                     //Test keypress
  case '0': case '1': case '2':
  case '3': case '4': case '5':
  case '6': case '7': 
  digit_num = keypress - '0'; break;                    //If numeric save it as "digit_num".

  case 'a': case 'b': case 'c':                         //If a letter illuminate the display
  case 'd': case 'e': case 'f':
  case 'g': 
  I2C_Tx_any_segment(keypress, digit_num); break;

  case 'x': break;                                      //Use "x" to start again
  default: {SW_reset_with_interrupt;} break;}           //Any other key (AOK) press is ignored.
                                                        //Bottom of switch block return to top of "do-loop"
  }while(keypress != 'x');                              //Bottom of "do-loop" with exit condition

  I2C_Tx_any_segment_clear_all();                       //Clear display and
  SW_reset_with_interrupt;}                             //repeat program



  ISR (WDT_vect){set_Arduino_WDTout; }
