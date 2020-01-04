/*
This program is used to verify that a text file has been successully loaded into flash.
It is loaded onto the UNO device at address 0x6C70 and works alongside the "Hex_text_programmer".
It is accessed by the "Hex_text_programmer" using an assembly jump command and a WDTout returns 
controll the the "Hex_text_programmer"
The EEPROM is used to share variables between the two programs.

Compile it using optimisation level s ONLY
Rx/Tx work at 57.6k


Note:

This program contains two subroutines, one to count the number of text strings and one to print out
any given string.  The first string starts at location 0x5C7F, and occupies addresses below 0x5C7F.
The application and its text strings share the flash from addresses zero to 0x5C7F (>23kB).
Each string is termintaed with the '\0' char and the final string is ternimated wth two '\0' chars.
It is anticipated that these subroutines will be used in any user aplication putting strings in flash.
*/




#include "Text_verification.h"


int char_counter;


char string_counter(int);
void print_string_num(int, int);



int main (void){ 

char Num_strings;																	//The number of strings written to flash
int  start_address; 																//Address in flash of first character in a string,

eeprom_write_byte((uint8_t*)0x3FC,	(eeprom_read_byte((uint8_t*)0x3FC) | 0x80));

char_counter = 0;																	//counts the number of characters in the text file (excludes \r & \n)

newline(); 
start_address = 0x5C7F;																//start adddress of text
Num_strings = string_counter(start_address);										//Count the number of strings

for(int text_num = 1; text_num <= Num_strings; text_num++)
{print_string_num(text_num,start_address);
if(text_num == 1){sendString("\tAK to continue");}
sendString("\r\n"); waitforkeypress();

sendString("\r\n");}

wdt_enable(WDTO_15MS);
while(1);

return 1;}



/***************************************************************************************************************************************************/
char string_counter(int start_address){												//Scroll through text section of flash counting the '\0' chars
char counter = 0, next, previous = 0; 												//untill '\0' '\0' is detected to indicate the end of the
while(1){																			//last string

Prog_mem_address_H = start_address >> 8;
Prog_mem_address_L = start_address;
read_flash ();																		//assembly subroutine
next = Flash_readout;																//result provided by assembly subroutine

if(next == 0){counter += 1; if (!(previous)) return counter-1;}
else char_counter += 1;

previous = next;
start_address -= 1;}}



/***************************************************************************************************************************************************/
void print_string_num(int text_num, int start_address){								//scroll through text section of flash counting '\0' chars
int null_counter = 1; 																//untill the start of the required string
char next,line_length = 0;															//Print the characters untill '\0' is detected

while(1){	
if(null_counter == text_num)break;
Prog_mem_address_H = start_address >> 8;
Prog_mem_address_L = start_address;
read_flash ();	
next = Flash_readout;
if(next == 0)null_counter += 1;
start_address -= 1;}

while(1){	
Prog_mem_address_H = start_address >> 8;
Prog_mem_address_L = start_address;
read_flash ();
if(Flash_readout)sendChar(Flash_readout); else break;
line_length += 1;
if ((Flash_readout == ' ') && (line_length > 90))
{sendString("\r\n");line_length = 0;}
start_address -= 1; }}


