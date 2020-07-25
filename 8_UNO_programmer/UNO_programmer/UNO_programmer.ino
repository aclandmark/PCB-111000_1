
#include "UNO_programmer.h"




int main (void){ 

unsigned int target_type = 0,  target_type_M;               //Device signature bytes      
unsigned char keypress;
unsigned char OSCCAL_mini_OS;
int error_mag;

CLKPR = (1 << CLKPCE);
CLKPR = (1 << CLKPS0);                                      //Convert 16MHx xtal to 8MHx clock
setup_328_HW;                                               //see "Resources\ATMEGA_Programmer.h"

while(1){
do{sendString("s  ");} 
while((isCharavailable(255) == 0));                           //User prompt 
if(receiveChar() == 's')break;}
Atmel_powerup_and_target_detect;                              //Leave target in programming mode                              
sendString ("\r\n\r\nATMEGA");

switch (target){
case 3281: sendString ("328"); break;
case 3282: sendString ("328P");break;
default: wdt_enable(WDTO_1S); while(1);break;}

PageSZ = 0x40; PAmask = 0x3FC0; FlashSZ=0x4000;                 //flash page size, gives address on page, flash size words
EE_top = 0x400-0x10;                                            //Last 16 bytes reseerved for system use
text_start = 0x5;                                               //First 5 bytes reserved for programmmer use

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
Exit_Programmer;break;
case 'p': break;
case 'x': Exit_Programmer;break;
default: break;} 
if ((op_code == 't') || (op_code == 'p')) break;} 

if (op_code == 'p')sendString("\r\nSend hex file (or x to escape).\r\n");



(Atmel_config(Chip_erase_h, 0));

Program_Flash_Hex();
Verify_Flash_Hex();
Prog_config_bytes;
sendString("\r\nTest_file? y or n\r\n");
if (waitforkeypress() == 'y')
{op_code = 't';
sendString("Send file.");
Program_Flash_Text();
Verify_Flash_Text();}

sendString (Version);


newline();
Reset_H;                                                            //Set target device running          
SW_reset;

UCSR0B &= (~((1 << RXEN0) | (1<< TXEN0)));                        //Dissable UART
while(1);
return 1;}


/***************************************************************************************************************************************************/
ISR(USART_RX_vect){
switch (op_code){
case 't': upload_text();break;
case 'p': upload_hex(); break;}}



/***************************************************************************************************************************************************/
/***************************************************************************************************************************************************/
ISR(TIMER2_OVF_vect) {                                                //Timer0 times out and halts at the end of the text file
if(text_started == 3)                                                 //Ignore timeouts occurring before start of file download
  {endoftext -= 1; TCCR2B = 0; TIMSK2 &= (~(1 << TOIE2));             //Shut timer down
  inc_w_pointer; store[w_pointer] = 0;                                //Append two '\0' chars to the end of the text
  inc_w_pointer; store[w_pointer] = 0; }}



/***************************************************************************************************************************************************/




/***************************************************************************************************************************************************/
