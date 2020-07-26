
#include "UNO_programmer.h"

#define Version "UNO_programmer_V1.1\r\n" 



int main (void){ 

CLKPR = (1 << CLKPCE);
CLKPR = (1 << CLKPS0);                                            //Convert 16MHx xtal to 8MHx clock
setup_328_HW;                                                     //see "Resources\ATMEGA_Programmer.h"
Reset_L;                                                          //Put target in reset state

while(1){
do{sendString("s  ");} 
while((isCharavailable(255) == 0));                               //User prompt 
if(receiveChar() == 's')break;}
Atmel_powerup_and_target_detect;                                  //Leave target in programming mode                              

sendString ("\r\n\r\nATMEGA ");
sendString(Device);

EE_top = EE_size-0x3;                                             //Last 3 bytes reserved for OSCCAL calibration
text_start = 0x5;                                                 //First 5 bytes reserved for programmmer use

sendString(" detected\r\nPress -p- to program flash, \
-e- for EEPROM or -x- to escape.");

while(1){
op_code = waitforkeypress();
switch (op_code){

case 'r': Verify_Flash_Text();  SW_reset; break;
case 'e': Prog_EEPROM(); break;
case 'd':                                                       //Delete contents of the EEPROM
sendString("\r\nReset EEPROM! D or AOK to escape");             //but leave cal data.
newline();
if(waitforkeypress() == 'D'){
sendString("10 sec wait");
for (int m = 0; m < EE_top; m++)  
{Read_write_mem('I', m, 0xFF);}
sendString(" Done\r\n");}
SW_reset;break;
case 'p': break;
case 'x': SW_reset; break;
default: break;} 
if ((op_code == 'p')||(op_code == 'P')) break;} 


sendString("\r\nSend hex file (or x to escape).\r\n");

(Atmel_config(Chip_erase_h, 0));

Program_Flash_Hex();
Verify_Flash_Hex();
if (op_code == 'P'){Prog_config_bytes;}
Verify_config_bytes;
sendString("\r\nTest_file? y or n\r\n");
if (waitforkeypress() == 'y')
{op_code = 't';
sendString("Send file.");
Program_Flash_Text();
Verify_Flash_Text();}

sendString (Version);

newline();
           
SW_reset;       //TO BE REMOVED

UCSR0B &= (~((1 << RXEN0) | (1<< TXEN0)));                        //Dissable UART
Reset_H;                                                          //Set target device running 
while(1);                                                         //Wait for UNO reset
return 1;}


/***************************************************************************************************************************************************/
ISR(USART_RX_vect){
switch (op_code){
case 't': upload_text();break;
case 'p':
case 'P': upload_hex(); break;}}



/***************************************************************************************************************************************************/
/***************************************************************************************************************************************************/
ISR(TIMER2_OVF_vect) {                                                //Timer0 times out and halts at the end of the text file
if(text_started == 3)                                                 //Ignore timeouts occurring before start of file download
  {endoftext -= 1; TCCR2B = 0; TIMSK2 &= (~(1 << TOIE2));             //Shut timer down
  inc_w_pointer; store[w_pointer] = 0;                                //Append two '\0' chars to the end of the text
  inc_w_pointer; store[w_pointer] = 0; }}



/***************************************************************************************************************************************************/




/***************************************************************************************************************************************************/
