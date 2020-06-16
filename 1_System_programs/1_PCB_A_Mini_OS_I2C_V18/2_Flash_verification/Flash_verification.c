/*
"Flash_verification" routine designed for use with the "PCB_bootloader" program.
"Flash_verification" starts at address 0x5E00 and is reached via a jump command
located at the end of the PCB_bootloader.  Normal HW setup is not therefore required.
Data passed between the bootloader and read routines is placed in EEPROM 

"Flash_verification" also programs the UNO fuses where possible leaving them as supplied,
however Fuse High is changed from 0xDE to 0xD0 to increase the size of the boot partition to
0x1000 bytes and ensure that the EEPROM survives chip erasure.
Lock bye is changed from 0xFF to 0xEF to protect the bootloader partition from being
accidently overwritten.



*/


#include "Flash_verification.h"
	

int main (void){ 


/*****************Power-up and make contact with target****************************/
Define_programmining_pins; 


/***********************New Config bytes for UNO device*************************************************/
Atmel_config(write_extended_fuse_bits_h,0x5);						//Sets BOD to 2.9V
Atmel_config(write_fuse_bits_h,0xFF);								//Low power crystal and 65mS SUT

if(eeprom_read_byte((uint8_t*)0x3FC) == 1)							//Arduino Config bytes
{Atmel_config(write_fuse_bits_H_h,0xDE);	
Atmel_config(write_lock_bits_h,0xFF);}
else 																//Hex_text boolotloader config bytes
{Atmel_config(write_fuse_bits_H_h,0xD0);							//1kB boot partition and EEPROM survives chip erasure
Atmel_config(write_lock_bits_h,0xEF);}								//Protects boot loader section





prog_counter = ((eeprom_read_byte((uint8_t*)0x3F8)) << 8) +\
eeprom_read_byte((uint8_t*)0x3F7);									//EEPROM used to pass values from bootloader
cmd_counter = ((eeprom_read_byte((uint8_t*)0x3F6)) << 8) +\
eeprom_read_byte((uint8_t*)0x3F5);

initialise_leds;

Verify_Flash();  

sendString (Version);

sendString("UNO Config bytes: Fuses extended, \
high, low and lock\r\n");											//Print out fuse bytes, cal byte and file sizes
sendHex(16, Atmel_config(read_extended_fuse_bits_h, 0));sendString("  ");
sendHex(16, Atmel_config(read_fuse_bits_H_h,0));sendString("  ");	
sendHex(16, Atmel_config(read_fuse_bits_h, 0));sendString("  ");
sendHex(16, Atmel_config(read_lock_bits_h, 0));sendString("  ");

sendString("    on-chip cal bit:  ");

if(eeprom_read_byte((uint8_t*)0x3FD) == OSCCAL)					//Print out OSCCAL
{sendHex(16, OSCCAL); sendString("\t DF value");}
else {sendHex(16, OSCCAL); sendString("\t User value");}


newline();
sendString("Hex_file_size:	");
sendHex(10,cmd_counter); sendString("  d'loaded:  "); 
sendHex(10,prog_counter); sendString(" in:  "); 
sendHex(10,read_ops); sendString(" out\r\n");

Reset_H;															//Exit programming mode
DDRC &= (~(1 << DDC3));
sendString("h/t/r/D      ");											//First user prompt of UNO bootloader
timer_T1_sub(T1_delay_500mS);

Reset_L;												
DDRC |= (1 << DDC3);												//Generate a 2mS UNO reset pulse on PC3
timer_T0_sub(T0_delay_2ms);											//Release UNO from reset (using WPU)
Reset_H;															//Reset line remains high untill SW_reset takes
DDRC &= (~(1 << DDC3));											//UNO enters bootloader program after being programmed
wdt_enable(WDTO_250MS); while(1);
return 1;  }



/*************************************************************************************************************************/
void Verify_Flash (void){

int  print_line = 0;												//Controls printing of hex file													
int line_no;														//Refers to the .hex file
signed int phys_address;											//Address in flash memory
signed int prog_counter_mem;										//Initialised with size of .hex file used for programming
char print_out_mode = 0;											//Print out flash contents as hex or askii characters
char skip_lines[4];													//Enter number to limit the print out	

FlashSZ=0x4000;														
read_ops = 0;

if((eeprom_read_byte((uint8_t*)0x3FC)  == 1) ||\
(eeprom_read_byte((uint8_t*)0x3FC)  == 2)){						//Read Arduino code from new UNO pcb
phys_address = 0x3F00;

while(1){
while(1) {															//Start reading the flash memory searching for the next hex command
Hex_cmd = Read_write_mem('L',phys_address, 0x0);							
Hex_cmd = (Hex_cmd<<8) + (Read_write_mem('H',phys_address, 0x0)); 
phys_address++;        
if (phys_address == FlashSZ)break;									//No more memory? Quit if yes
if (Hex_cmd != 0xFFFF) break;}										//If the hex command is 0xFFFF remain in this loop otherwise exit.

if (phys_address == FlashSZ)break;

sendString("\r\n:20");sendHex (16, (phys_address-1)*2);
sendString("00");
sendHex (16, Hex_cmd);read_ops++;

for(int m=0; m<15; m++){    										//Read the next seven locations in the flash memory   
Hex_cmd = Read_write_mem('L',phys_address, 0x0);				
Hex_cmd = (Hex_cmd<<8) + (Read_write_mem('H',phys_address, 0x0)); 
phys_address++; 
sendHex (16, Hex_cmd);read_ops++;
if(phys_address==FlashSZ)break;}

if(phys_address==FlashSZ)break;}
sendString("\r\n:00000001FF\r\n");

if(eeprom_read_byte((uint8_t*)0x3FC)  == 2)
{cmd_counter = 0;
prog_counter = 0;}
eeprom_write_byte((uint8_t*)0x3FC, 0xFF);

return;}





phys_address = 0x2E08;	//0x2E30;//0x2E48;  									//Verify Hex_text_bootloader code.
read_ops=0; 
line_no = 0; prog_counter_mem = prog_counter; 

sendString("\r\nInteger(0-FF)?  ");									//0 prints no lines -1-, every line, -8- prints every eighth line etc... 
skip_lines[0] = '0';												//Acquire integer between 0 and FF
skip_lines[1] = waitforkeypress();
skip_lines[2] = '\0';
timer_T1_sub(T1_delay_500mS);	
if (isCharavailable(1)){skip_lines[0] = skip_lines[1]; 
skip_lines[1] = receiveChar();}	
binUnwantedChars();	
print_line = askiX2_to_hex(skip_lines);
sendHex (16,print_line); sendString("   ");


if (print_line == 0); 												//hex file print out not required
else {sendString("1/2?\r\n");										//else -1- sends file as askii, -2- sends it as hex
print_out_mode =  waitforkeypress(); binUnwantedChars();			
newline();}



while(1){ if(!(prog_counter_mem))break;							//print out loop starts here, exit when finished
		
while(1) {															//Start reading the flash memory searching for the next hex command

Hex_cmd = Read_write_mem('L',phys_address, 0x0);							
Hex_cmd = (Hex_cmd<<8) + (Read_write_mem('H',phys_address, 0x0)); 
phys_address++;        
if (phys_address == FlashSZ)break;									//No more memory? Quit if yes
if (Hex_cmd != 0xFFFF) break;										//If the hex command is 0xFFFF remain in this loop otherwise exit.
led_on;}

led_off;
if (phys_address == FlashSZ)break;									//Exit when there is no more flash to read

if ((print_line == 0)  && (!(line_no%10)))sendChar('*');			//Print out of hex file not required
																	//Print a -*- every tenth line of the file
if(print_line && (!(line_no%print_line)))							//Print out required: Print all lines or just a selection			
{newline(); sendHex (16, (phys_address-1)*2);		
sendString("   "); if(print_out_mode == '1'){send_as_askii;} 		//Start with the address of the first command in the line
else {sendHex (16, Hex_cmd);sendString(" ");}}										//Print first command in askii or hex
read_ops++;															//Value to be sent to PC for comparison with the hex filer size
prog_counter_mem--;													//"prog_counter_mem" decrements to zero when the end of the file is reached


for(int m=0; m<7; m++){    										//Read the next seven locations in the flash memory   

Hex_cmd = Read_write_mem('L',phys_address, 0x0);				
Hex_cmd = (Hex_cmd<<8) + (Read_write_mem('H',phys_address, 0x0)); 
phys_address++; 
if(Hex_cmd == 0xFFFF)break;											//Read 0xFFFF: return to start of print out loop
prog_counter_mem--;

if(phys_address & 0x0040) {led_on;	} else {led_off;} 			       

if(print_line && (!(line_no%print_line)))
{timer_T0_sub(T0_delay_5ms);										//5ms delay prevents PC from getting overwhelmed with characters	
if(print_out_mode == '1'){send_as_askii;} 
else {sendHex (16, Hex_cmd);sendString(" ");}}
read_ops++;

if(phys_address==FlashSZ)break;}

line_no++;
if (phys_address == FlashSZ)break;}

led_off;

newline();}





