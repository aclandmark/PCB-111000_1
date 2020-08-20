
#include "UNO_programmer.h"

#define Version "UNO_programmer_V1.2\r\n" 

int main (void){ 

CLKPR = (1 << CLKPCE);
CLKPR = (1 << CLKPS0);                                            //Convert 16MHx xtal to 8MHx clock
setup_328_HW;                                                     //see "Resources\ATMEGA_Programmer.h"

Reset_L;                                                          //Put target in reset state to dissable UART

while(1){
do{sendString("s  ");} 
while((isCharavailable(255) == 0));                               //User prompt 
if(receiveChar() == 's')break;}
Atmel_powerup_and_target_detect;                                  //Leave target in programming mode                              


EE_top = EE_size-0x4;                                             //Last 4 bytes of EEPROM reserved for OSCCAL calibration
text_start = 0x5;                                                 //First 5 bytes reserved for EEPROM programmmer use

sendString(" detected.\r\nPress -p- to program flash, \
-e- for EEPROM, -r- to run target or -x- to escape.");

while(1){
op_code = waitforkeypress();
switch (op_code){

case 'r': UCSR0B &= (~((1 << RXEN0) | (1<< TXEN0)));             //Dissable UART
Reset_H;                                                         //Set target device running 
while(1); break;                                                 //Wait for UNO reset

case 'R': Verify_Flash_Text();  SW_reset; break;
case 'e': Prog_EEPROM(); break;

case 'd':                                                       //Delete contents of the EEPROM
sendString("\r\nReset EEPROM! D or AOK to escape");             //but leave cal data.
newline();
if(waitforkeypress() == 'D'){
sendString("10 sec wait");
for (int m = 0; m < EE_top; m++)  
{Read_write_mem('I', m, 0xFF);}                                 //Write 0xFF to all EEPROM loactions bar the top 3
sendString(" Done\r\n");}
SW_reset;break;

case 'x': SW_reset; break;
default: break;} 

if ((op_code == 'p')||(op_code == 'P')) break;} 
sendString("\r\nSend hex file (or x to escape).\r\n");

Program_Flash_Hex();
Verify_Flash_Hex();

if (op_code == 'P'){Prog_config_bytes;}
Verify_config_bytes;

sendString("\r\nText_file? y or n\r\n");
if (waitforkeypress() == 'y')
{op_code = 't';                                                 //Required by UART ISR

sendString
("\r\nReduce baud rate to 19.2kB and press 'r'.\r\n");          //Sending text requires baud rate reduction
Timer_T0_sub(T0_delay_10ms);
UBRR0L = 51;
while (1){if (waitforkeypress() != 'r'); else break;}


sendString("Send file.");
Program_Flash_Text();

sendString
("\r\nRestore baud rate to 38.4kB and press 'r'.\r\n");
Timer_T0_sub(T0_delay_10ms);
UBRR0L = 25;
while (1){if (waitforkeypress() != 'r'); else break;}
Verify_Flash_Text();}

sendString (Version);
newline();

Read_write_mem('I', EE_size - 4, \
(Atmel_config(signature_bit_2_h, signature_bit_2_l)));          //Define target type on target device

UCSR0B &= (~((1 << RXEN0) | (1<< TXEN0)));                      //Dissable UART

initialise_IO;                                                  //Reset all to WPU
Reset_H;                                                        //Set target device running 
while(1);                                                       //Wait for UNO reset
return 1;}





/***************************************************************************************************************************************************/
ISR(USART_RX_vect){
switch (op_code){
case 't': upload_text();break;
case 'p':
case 'P': upload_hex(); break;}}


/***************************************************************************************************************************************************/
ISR(TIMER2_OVF_vect) {                                          //Timer2 times out and halts at the end of the text file
if(text_started == 3)                                           //Ignore timeouts occurring before start of file download
  {endoftext -= 1; TCCR2B = 0; TIMSK2 &= (~(1 << TOIE2));       //Shut timer down
  inc_w_pointer; store[w_pointer] = 0;                          //Append two '\0' chars to the end of the text
  inc_w_pointer; store[w_pointer] = 0; }}


  
