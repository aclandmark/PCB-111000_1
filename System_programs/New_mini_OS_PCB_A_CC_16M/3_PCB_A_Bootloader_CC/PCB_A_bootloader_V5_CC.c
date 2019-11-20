




/********  ATMEGA pcb bootloader runs in the boot space of the ATMEGA328**********/

/*Developed for use with CC displays
Compile it using optimisation level s ONLY
It shares the ATMEGA 328 with the display control program which runs in the application code space.
Rx/Tx work at 57.6k
*/

/*EEPROM reservations
0x3FF	user cal if set
0x3FE	user cal if set
0x3FD	Default cal supplied by Atmel
0x3FC	If 1: press 'x' diagnostic mode else press 'r' normal mode
0x3FB	If 0 use multiplexter (T0) period of 4ms else use period of 2mS (std)
0x3FA	No longer used.
0x3F9	Reset status byte: set to zero when the mini-OS is programmed to flash.
0x3F8	Unused
*/

#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>

#include "PCB_A_bootloader_V5_CC.h"
#include "PCB_A_bootloader_V5_CC_SW_subs.c"
#include "PCB_A_bootloader_V5_CC_HW_subs.c"

#define wdr()  __asm__ __volatile__("wdr")

signed int  read_ops=0;	
char keypress;

int main (void){ 
char target_detected = 0;

if ((!(MCUSR & (1 << EXTRF)))||\
(!(eeprom_read_byte((uint8_t*)0x3F9))))				//External programmer sets EEPROM location 0x3F9 to zero 			 
{eeprom_write_byte((uint8_t*)0x3F9,1);					//FOR POR, BOR or WDTime out or immediately after 
asm("jmp 0x0000");}										//programming jump to location zero

initialise_IO;											//Only jump here following an external reset switch press
thirty_msec_delay;										//Pause device so that it can be put into programming mode 
set_up_WDT;												//Watch Dog Timer
cal_device;												//Check for user calibration byte and apply if present


if ((eeprom_read_byte((uint8_t*)0x3F9)) == 1)			//First click of pcb reset switch
{eeprom_write_byte((uint8_t*)0x3F9, 2);				//Update and save Reset status
MCUSR &= (~(1 << EXTRF));								//Clear external reset flag
three_hundred_msec_delay;								//Start 300ms double click timer
eeprom_write_byte((uint8_t*)0x3F9, 1);					//No second reset: Clear Reset status
PORTB &= (~(1 << PB2));
DDRB |= (1 << DDB2);									//Set UNO signalling line low
Reset_L;												
DDRC |= (1 << DDC3);									//Generate a 2mS UNO reset pulse on PC3
two_msec_delay;						
Reset_H;												//Release UNO from reset (using WPU)
DDRC &= (~(1 << DDC3));								//Reset line remains high untill SW_reset takes
wdt_enable(WDTO_120MS); while(1);}		//250MS			//effect and control jumps to location zero, use 250ms if cal routine is included



if ((eeprom_read_byte((uint8_t*)0x3F9)) == 2)			//Second click of pcb reset switch
{eeprom_write_byte((uint8_t*)0x3F9, 1);				//Clear Reset status
MCUSR &= (~(1 << EXTRF));								//Clear external reset flag
//PORTB |= (1 << PB2);									//Set UNO signalling line high (WPU)
//DDRB &= (~(1 << DDB2));								//Note: Theses are default states for the CC display driver
Reset_L;
DDRC |= (1 << DDC3);									//Put UNO in reset for 2mS
two_msec_delay;											//After its release from reset
Reset_H;												//the UNO selects its boot loader
DDRC &= (~(1 << DDC3));								//Leave PC3 as week pull up
USART_Rx_init(0,16);									//Activate UART receiver
if ((waitforkeypress()) != 'p')						//For keypres h,t or r
{wdt_enable(WDTO_30MS); while(1);}	//60MS						//return control to line zero
else {													//For keypress p: Run pcb_bootloader

MCUCR = (1<<IVCE);  									//use interrupt vector table starting at start of boot section
MCUCR = (1<<IVSEL);
initialise_leds; 
ADMUX |= (1 << REFS0);									//select internal ADC ref and remove external supply on AREF pin

boot_target; 
Atmel_powerup_and_target_detect;

	
/*****************Power-up and make contact with target****************************/

if(!(target_detected))sendChar('!');

if(target_detected == 1){
UCSR0B |= (1<< TXEN0);									//It is now OK to activate the UART TX

while(1){
do{sendString("?  ");one_second_delay;} 				//User prompt
while((isCharavailable() == 0));
keypress = receiveChar();	
 if (keypress == 'p')break;
if((keypress == 'r') || (keypress == 'x')){
if (keypress == 'x') 
{eeprom_write_byte((uint8_t*)0x3FC, 0x1);}
Reset_H;
sendString("h/t/r_1  ");								//Send the first UNO user prompt
wdt_enable(WDTO_60MS); while(1);}}  					//Exit bootloader early

PageSZ = 0x40; PAmask = 0x3FC0;							//Define flash parameters

sendString ("\n\rUNO_Hex_file?"); 		
while ((keypress = waitforkeypress()) != ':')			//Ignore characters before the first ':'


Initialise_variables_for_programming_flash;

UCSR0B |= (1<<RXCIE0); sei();							//Set UART Rx interrupt
(Atmel_config(Chip_erase_h, 0));
led_on;
Program_Flash();

led_off;

eeprom_write_byte((uint8_t*)0x3F8, prog_counter >> 8);	
eeprom_write_byte((uint8_t*)0x3F7, prog_counter);	
eeprom_write_byte((uint8_t*)0x3F6, cmd_counter >> 8);	
eeprom_write_byte((uint8_t*)0x3F5, cmd_counter);	


asm("jmp 0x5D00");}}									//Jump to flash verification routine

}return 1;}



ISR(USART_RX_vect){
	unsigned char Rx_Hex_char=0;
	unsigned char Rx_askii_char;
	int local_pointer;

Rx_askii_char = receiveChar();
if (Rx_askii_char == ':')counter = 0;
else {if (Rx_askii_char <= '9') Rx_Hex_char = Rx_askii_char - '0'; 
else Rx_Hex_char = Rx_askii_char - '7';}


switch (counter){
case 0x0:  	break;															//Detect -:- at start of new line
case 0x1: 	tempInt1 = Rx_Hex_char<<4;  break;								//Acquire first digit 
case 0x2: 	tempInt1 += Rx_Hex_char;  										//Acquire second digit and combine with first to obtain number of commands in line
			char_count = 9 + ((tempInt1) *2); 								//Calculate line length in terms of individual characters
			local_pointer = w_pointer++; 									//Update pointer to array "store"
			store[local_pointer] = tempInt1; break;							//Save the number of commands in the line to the array  
case 0x3: 	tempInt1 = Rx_Hex_char<<4;  break;								//Next 4 digits give the address of the first command in the line
case 0x4:	tempInt1 += Rx_Hex_char; tempInt1=tempInt1<<8; break;			//Acquire second digit and combine it with first 
case 0x5:	tempInt1 += Rx_Hex_char<<4;  break;							//Continue for third digit
case 0x6: 	tempInt1 += Rx_Hex_char; 										//Acquire final digit and caculate address of next command 
			local_pointer = w_pointer++; 									//Update pointers to array "store"
			store[local_pointer] = tempInt1; break;							//Save address of next command to array "store"
case 0x7: 	break;															//chars 7 and 8 are not used
case 0x8: 	break;
default: 	break;}

if ((counter > 8)&&(counter < char_count)){								//Continue to acquire, decode and store commands
if ((counter & 0x03) == 0x01){tempInt1 = Rx_Hex_char<<4;}					//Note: Final two chars at the end of every line are ignored
if ((counter & 0x03) == 0x02)  {tempInt1 += Rx_Hex_char;}
if ((counter & 0x03) == 0x03)  {tempInt2 = Rx_Hex_char<<4;}
if ((counter & 0x03) == 0x0)  	{tempInt2+= Rx_Hex_char; 
								tempInt2=tempInt2<<8;tempInt1+=tempInt2;
local_pointer = w_pointer++; 	
store[local_pointer] = tempInt1;cmd_counter++;}}

counter++;
w_pointer = w_pointer & 0b00011111;	} 


/***************************************************************************************************************************************************/
void Program_Flash (void){

new_record();  														//Start reading first record which is being downloaded to array "store" 
start_new_code_block(); 											//Initialise new programming block (usually starts at address zero but not exclusivle so)
Program_record();													//Coppy commands from array "store" to the page_buffer														
			
		
while(1){		
new_record();														//Continue reading subsequent records
if (record_length==0)break; 										//Escape when end of hex file is reached


if (Hex_address == HW_address){										//Normal code: Address read from hex file equals HW address and lines contains 8 commands
switch(short_record){
case 0: if (space_on_page == (PageSZ - line_offset))				//If starting new page
			{page_address = (Hex_address & PAmask);}break;			//get new page address
			

case 1:	start_new_code_block();										//Short line with no break in file (often found in WinAVR hex files).
		short_record=0;break;}}
		
		
if(Hex_address != HW_address){										//Break in file
	if (section_break){												//Section break: always found when two hex files are combined into one 										
		if((Flash_flag) && (!(orphan)))
		{write_page_SUB(page_address);}								//Burn contents of the partially full page buffer to flash
		if(orphan) write_page_SUB(page_address + PageSZ);} 		//Burn outstanding commands to the next page in flash			
		
	if(page_break)													//In practice page breaks and short jumps are rarely if ever found											
		{if((Flash_flag) && (!(orphan)))							//Burn contents of the partially filled page buffer to flash
		{write_page_SUB(page_address);}														
		orphan = 0;}
		
	start_new_code_block();											//A new code block is always required where there is a break in the hex file.
	short_record=0;}
		
Program_record();}													//Continue filling page_buffer
		

cli();	
UCSR0B &= (~(1<<RXCIE0));											//download complete, disable UART Rx interrupt
PORTD &= (~(1 << PD7));				
while(1){if (isCharavailable()==1)receiveChar();else break;}		//Clear last few characters of hex file
	
if((Flash_flag) && (!(orphan))){write_page_SUB(page_address);}	//Burn final contents of page_buffer to flash
if(orphan) {write_page_SUB(page_address + PageSZ);}}

