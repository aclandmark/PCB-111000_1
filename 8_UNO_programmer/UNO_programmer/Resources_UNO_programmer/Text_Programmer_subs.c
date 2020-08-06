



void Program_Flash_Text (void){
w_pointer = 0; r_pointer = 0;
text_started =0; endoftext = 3; counter = 0;
Rx_askii_char_old = '0';

UCSR0B |= (1<<RXCIE0);                                                  //Activate UART interrupt
sei();                                                                  //Set global interrupt

Timer_T2_sub_with_interrupt(7,0);                                       //Start Timer2 with interrupt

address_in_flash = FlashSZ;//0x4000; //0                                   //First character will be storrred at 0x6FFF not 0x6FFE
write_address = 0x40;                                                     //Address on page buffer
while (1){
while (r_pointer == w_pointer);                                           //wait for w_pointer to be incremented
if(endoftext != 3)endoftext -= 1;                                         //Indicates that the timer has been shut down
write_address -= 1;
address_in_flash -= 1;
Load_page('L', write_address, store[r_pointer]); 
Load_page('H', write_address, store[r_pointer] >> 8);
store[r_pointer] = 0;                                                     //clear the contents of the location in array store
inc_r_pointer;                                                            //restore the value of "r_pointer" to that of "w_pointer"
if (write_address == 0){
sendChar('*');
write_page_SUB(address_in_flash);
write_address = 0x40;}                                                    //Restore address_in_flash
if(!(endoftext)) break;}                                                  //Break when two '\0' chars have been appended to text stored in the array
address_in_flash -= write_address;
write_page_SUB(address_in_flash);
sendChar('*');
//sendHex(10,address_in_flash);
cli();  
UCSR0B &= (~(1<<RXCIE0));}





void upload_text(void){
unsigned char Rx_askii_char;
  
Rx_askii_char = receiveChar();
if((Rx_askii_char) == '\0')return;                                   //Ignore polling from Bootloader
if((Rx_askii_char != '*') && (!(text_started)))return;                //Ignore header text occuring before a line of * characters
if((Rx_askii_char == '*')&&(!(text_started)))                         //The number of * characters is not critical only one is required
{ text_started = 1;return;}
if((Rx_askii_char == '*')&&(text_started == 1))return;                //Ignore carriage returns occurring before the first text string
else {if (text_started == 1)text_started = 2;}
if((text_started == 2) && ((Rx_askii_char == '\r')
 || (Rx_askii_char == '\n')))return; else text_started = 3;           //Enter main area of text file containing strings to be written to flash

if ((Rx_askii_char == '\r') || (Rx_askii_char == '\n')){              //Check for '\r' and or '\n' and convert to a single '\0' character
if (Rx_askii_char_old == '\0') return;
else Rx_askii_char = '\0';}

TCNT2 = 0;                                                            //Reset the timer. An interrrupt is only required at the end of the file

if(!(counter%2)){                                                     //Each location in array "store" holds two askii characters
store[w_pointer] = Rx_askii_char;
store[w_pointer] = store[w_pointer] << 8;}                            //"w_pointer" gives address in array of the next free location
else{store[w_pointer] += Rx_askii_char;
inc_w_pointer;}
counter = (counter + 1);                                             //"counter" gives the number of characers downloaded
Rx_askii_char_old = Rx_askii_char;} 






void Verify_Flash_Text (void){

unsigned char  high_char, low_char, print_line = 0;                         //Controls printing of hex file                         
int line_no;                                                                //Refers to the .hex file
int phys_address;                                                          //Address in flash memory
signed int prog_counter_mem;                                                //Initialised with size of .hex file used for programming
unsigned char string_counter = 1;

phys_address = FlashSZ - 1;   

sendString("\r\nEnd of flash ");sendHex(16,phys_address);newline();

sendHex(16,string_counter++);sendChar('\t');
 while ((high_char = Read_write_mem('H',phys_address, 0x0)) != 0xFF)
 {timer_T0_sub(T0_delay_10ms);
 low_char = Read_write_mem('L',phys_address, 0x0);
if(high_char)
sendChar(high_char);              
else {newline();sendHex(10,string_counter++);sendChar('\t');}
if(low_char)
sendChar(low_char);              
else {newline();sendHex(10,string_counter++);sendChar('\t');}
phys_address -= 1;}

sendString("\r\nLast address available for hex ");sendHex(16,address_in_flash-1);
newline();newline();

//if((op_code == 'p') || (op_code == 'P'))
//{sendString("\r\nHex ends at "); sendHex(10, read_ops);sendChar('\t');}

}
  
