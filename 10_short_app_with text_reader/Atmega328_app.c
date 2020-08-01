




#include "Atmega328_app.h"
char Char_from_flash(int);
void Determine_device_type(void);


int char_counter;
int FlashSZ;
int EEP;

char string_counter(int);
void print_string_num(int, int);



int main (void){ 

char keypress, Num_strings;															//The number of strings written to flash
int  start_address; 																//Address in flash of first character in a string,
int  text_num;																		//The number of the string to be printed out

setup_HW;
cal_device;																			//checks cal status and adopts user cal bytes if present (Optional)


Initiase_device_labels;
Determine_device_type();

char_counter = 0;																	//counts the number of characters in the text file (excludes \r & \n)

while(1){																			//User prompt
do{sendString("w/s  ");}while((isCharavailable(300) == 0));
keypress = receiveChar();
if (keypress == 'w') break;															//press "w" to continue with program execution
if(keypress == 's') {wdt_enable(WDTO_60MS); while(1);}}

sendString("\r\nProgram running on Atmega ");
sendString(Device_ptr[Device]);
newline();


newline(); 
start_address = FlashSZ - 1;														//start adddress of text

if(Char_from_flash(start_address) == 0xFF){
sendString("No text available!\r\n");
wdt_enable(WDTO_60MS); while(1);}


Num_strings = string_counter(start_address);										//Count the number of strings
sendString("Total numbers of strings & characters are  ");
Num_to_PC(10,Num_strings); sendChar('\t');
Num_to_PC(10,char_counter);

sendString("\r\nEnter string number or zero to exit");								//Request string

while(1){
text_num = Num_from_KBD();
if(!(text_num))break;
if (text_num > Num_strings){sendString("\r\nNo string!"); continue;}
sendString("\r\n");
Num_to_PC(10,text_num);sendString("\r\n");
print_string_num(text_num,start_address);}

wdt_enable(WDTO_60MS); while(1);

return 1;}



/***************************************************************************************************************************************************/
char string_counter(int start_address){												//Scroll through text section of flash counting the '\0' chars
char counter = 0, next_char, previous = 0; 												//untill '\0' '\0' is detected to indicate the end of the
while(1){																			//last string

next_char = Char_from_flash(start_address);															//result provided by assembly subroutine

if(next_char == 0){counter += 1; if (!(previous)) return counter-1;}
else char_counter += 1;

previous = next_char;
start_address -= 1;}}



/***************************************************************************************************************************************************/
void print_string_num(int text_num, int start_address){								//scroll through text section of flash counting '\0' chars
int null_counter = 1; 																//untill the start of the required string
char line_length = 0;	
char next_char;														//Print the characters untill '\0' is detected

while(1){	
if(null_counter == text_num)break;
next_char = Char_from_flash(start_address);
if(next_char == 0)null_counter += 1;
start_address -= 1;}

while(1){	
next_char = Char_from_flash(start_address);
if(next_char)sendChar(next_char); else break;
line_length += 1;
if ((next_char == ' ') && (line_length > 90))
{sendString("\r\n");line_length = 0;}
start_address -= 1; }}





char Char_from_flash(int start_address){

Prog_mem_address_H = start_address >> 8;
Prog_mem_address_L = start_address;

asm volatile ("push r0") ;
asm volatile ("push r31") ;
asm volatile ("push r30") ;


asm volatile ("lds r31, Prog_mem_address_H");
asm volatile ("lds r30, Prog_mem_address_L");

asm volatile ("lpm r0, Z");

asm volatile ("sts Flash_readout,  r0");

asm volatile ("pop r30");
asm volatile ("pop r31");
asm volatile ("pop r0");

return Flash_readout;}





void Determine_device_type(void){

switch(SIGNATURE_1){

case 0x95: 
Device = 0;
EEP = 0x400;break;

case 0x94: 
Device = 1;
EEP = 0x200;
FlashSZ =0x4000;break;
}
}
