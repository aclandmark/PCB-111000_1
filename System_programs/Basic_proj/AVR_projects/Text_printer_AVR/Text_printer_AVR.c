


/************************Subroutine provided by .S file*******************/
extern void read_flash (void);



/***************************************************************************/
volatile char loc_in_mem_H, loc_in_mem_L;					//Used to pass addresses in SRAM to assembly
volatile char Prog_mem_address_H, Prog_mem_address_L;		//Used to pass addresses in flash to the assembly subroutines
volatile char Flash_readout;





void I2C_Tx_2_integers(unsigned int, unsigned int);
void I2C_Tx(char, char, char*);
void send_byte_with_Ack(char);
void send_byte_with_Nack(char);
void Timer_T1_sub(char, unsigned int);
unsigned int PRN_16bit_GEN(unsigned int );

void Char_to_PC(char);
void String_to_PC(const char*);
char waitforkeypress(void);
char receiveChar(void);
char isCharavailable(char);

int Num_from_KBD(void);
void Num_to_PC(char, long);
void SBtoAL(char*, long, char);
void NumericString_to_PC(char*);

char decimal_digit (char);
char wait_for_return_key(void);
void I2C_Tx_8_byte_array(char*);
long I2C_displayToNum(void);

char receive_byte_with_Ack(void);
char receive_byte_with_Nack(void);

void USART_init (unsigned char, unsigned char);

#define T1_delay_50ms 5,0xFE78
#define T1_delay_100ms 5,0xFCF2
#define T1_delay_250ms 5,0xF85F
#define T1_delay_500ms 5,0xF0BE


/*****************************************************************************/
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdio.h>


/*****************************************************************************/
#define SW_reset {wdt_enable(WDTO_30MS);while(1);}

/*****************************************************************************/
#define setup_HW \
setup_watchdog;\
Unused_I_O;\
set_up_I2C;\
ADMUX |= (1 << REFS0);\
USART_init(0,16);


/*****************************************************************************/
#define setup_watchdog \
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;

#define wdr()  __asm__ __volatile__("wdr")


/*****************************************************************************/
#define set_up_I2C \
TWAR = 0x02;


/*****************************************************************************/
#define Unused_I_O \
MCUCR &= (~(1 << PUD));\
DDRB = 0;\
DDRC = 0;\
DDRD = 0;\
PORTB = 0xFF;\
PORTC  = 0xFF;\
PORTD  = 0xFF;


/*****************************************************************************/
#define waiting_for_I2C_master \
TWCR = (1 << TWEA) | (1 << TWEN);\
while (!(TWCR & (1 << TWINT)));\
TWDR;

#define clear_I2C_interrupt \
TWCR = (1 << TWINT);



int char_counter;


char string_counter(int);
void print_string_num(int, int);



int main (void){ 

char keypress, Num_strings;															//The number of strings written to flash
int  start_address; 																//Address in flash of first character in a string,
int  text_num;																		//The number of the string to be printed out
char MCUSR_copy;


MCUSR_copy = MCUSR;

setup_HW;

MCUSR = 0;



char_counter = 0;																	//counts the number of characters in the text file (excludes \r & \n)


while(1){																			//User prompt
do{String_to_PC("w/s  ");}while((isCharavailable(255) == 0));
keypress = receiveChar();
if (keypress == 'w') break;															//press "w" to continue with program execution
if(keypress == 's') {wdt_enable(WDTO_60MS); while(1);}}

//Num_to_PC(10,MCUSR_copy);


start_address = 0x5C7F;		////////
Prog_mem_address_H = start_address >> 8;
Prog_mem_address_L = start_address;
read_flash ();
if (Flash_readout == 0xFF){String_to_PC("No text. Reset UNO\r\n");while(1);}	


String_to_PC("\r\n"); 
start_address = 0x5C7F;																//start adddress of text
Num_strings = string_counter(start_address);										//Count the number of strings
String_to_PC("Total numbers of strings & characters are  ");
Num_to_PC(10,Num_strings); Char_to_PC('\t');
Num_to_PC(10,char_counter);

String_to_PC("\r\nEnter string number or zero to exit");							//Request string

while(1){
text_num = Num_from_KBD();
if(!(text_num))break;
if (text_num > Num_strings){String_to_PC("\r\nNo string!"); continue;}
String_to_PC("\r\n");
Num_to_PC(10,text_num);String_to_PC("\r\n");
print_string_num(text_num,start_address);}

wdt_enable(WDTO_60MS); while(1);

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
if(Flash_readout)Char_to_PC(Flash_readout); else break;
line_length += 1;
if ((Flash_readout == ' ') && (line_length > 90))
{String_to_PC("\r\n");line_length = 0;}
start_address -= 1; }}



/************************************************************************************/
void Timer_T1_sub(char Counter_speed, unsigned int Start_point){ 
TCNT1H = (Start_point >> 8);
TCNT1L = Start_point;
TCCR1B = Counter_speed;
while(!(TIFR1 & (1<<TOV1)));
TIFR1 |= (1<<TOV1); 
TCCR1B = 0;}



/************************************************************************************/
void I2C_Tx_2_integers(unsigned int s1, unsigned int s2){			
char num_bytes=4; char mode=1; char s[4];
for (int m = 0;  m < 4; m++){
switch (m){
case 0: s[m] = s1; break; 											//Send S1 lower byte
case 1: s[m] = s1 >> 8; break; 									//Send S1 higher byte									
case 2: s[m] = s2; 	break;											//Send S1 lower byte
case 3: s[m] = s2 >> 8; break;}}									//Send S1 higher byte
I2C_Tx(num_bytes,mode, s);}



/************************************************************************************/
void I2C_Tx(char num_bytes, char mode, char s[]){
waiting_for_I2C_master;
send_byte_with_Ack(num_bytes);
send_byte_with_Ack(mode);
for (int m = 0;  m < num_bytes; m++){
if (m==num_bytes-1){send_byte_with_Nack(s[m]);}
else {send_byte_with_Ack(s[m]);}}
TWCR = (1 << TWINT);}



/************************************************************************************/
void send_byte_with_Ack(char byte){
TWDR = byte;													//Send payload size: Zero in this case
TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);				//clear interrupt and set Enable Acknowledge
while (!(TWCR & (1 << TWINT)));}



/************************************************************************************/
void send_byte_with_Nack(char byte){
TWDR = byte;													//Send payload size: Zero in this case
TWCR = (1 << TWINT) | (1 << TWEN);								//clear interrupt and set Enable Acknowledge
while (!(TWCR & (1 << TWINT)));}



/************************************************************************************/
unsigned int PRN_16bit_GEN(unsigned int start){
unsigned int bit, lfsr;

if(!(start)) lfsr = (eeprom_read_byte((uint8_t*)(0x3EF)) << 8) + eeprom_read_byte((uint8_t*)(0x3EE));
else lfsr = start;
bit = (( lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
lfsr = (lfsr >> 1) | (bit << 15);
if(!(start)){
eeprom_write_byte((uint8_t*)(0x3EF),(lfsr>>8));
eeprom_write_byte((uint8_t*)(0x3EE),lfsr);}

return lfsr;}
	


/*********************************************************************/
void Char_to_PC(char data){
while (!(UCSR0A & (1 << UDRE0)));
UDR0 = data;}

/*********************************************************************/
void String_to_PC(const char s[]){
int i = 0;
while(i < 200){
if(s[i] == '\0') break;
Char_to_PC(s[i++]);} }



/*********************************************************************/
char waitforkeypress(void){
while (!(UCSR0A & (1 << RXC0)));
return UDR0;}


/*********************************************************************/
char receiveChar(void)
{return UDR0;}


/*********************************************************************/
char isCharavailable (char m){int n = 0;
while (!(UCSR0A & (1 << RXC0))){n++;
if (n>8000) {m--;n = 0;}if (m == 0)return 0;}
return 1;}


/*******************************************************************************/
int Num_from_KBD(void){
char keypress;
int number;

do
{keypress =  waitforkeypress();} while(!(decimal_digit(keypress)));

keypress -= '0';
number =  keypress;

while(1){
if ((keypress = wait_for_return_key())  =='\r')break;
if (decimal_digit (keypress))		
{
keypress -= '0';
number = number * 10 + keypress;}}
return number;}




/*********************************************************************/
void Num_to_PC(char radix, long long_num){
char array[12];														//Long has 10 chars + sign + null terminator	
SBtoAL(array, long_num, radix);										//calls the Binary to askii subroutine
NumericString_to_PC(array);}										//Prints characters in reverse order

/*********************************************************************/
void SBtoAL(char array[], long num, char radix){					//Signed Binary to Askii
int m=0;
long sign;

if (num == 0x80000000){
switch(radix){
case 10: array[0] = '8';array[1] = '4'; array[2] = '6';			//0x80000000 * -1 = 0
array[3] = '3';array[4] = '8';array[5] = '4'; array[6] = '7';
array[7] = '4';array[8] = '1';array[9] = '2';
array[10] = '-'; array[11] = '\0'; break;

case 16: array[0] = '0';array[1] = '0'; array[2] = '0';			//0x80000000 * -1 = 0
array[3] = '0';array[4] = '0';array[5] = '0'; array[6] = '0';
array[7] = '8';array[8] = '-';array[9] = '\0';
array[10] = '\0'; array[11] = '\0'; break; } return;}

for (int n = 0; n <=11; n++)array[n] = 0;	
if ((sign = num) < 0) num = num * (-1);

do {array[m] = num % radix;
if(array[m] < 10)array[m] += '0'; else array[m] += '7';
m++;} while ((num = num/radix) > 0);
if (sign < 0) {array[m] = '-';m++;}}


/*********************************************************************/
void NumericString_to_PC(char* s){					
int n=0;
while (s[n] != '\0')n++;											//scroll to end of string counting the number of characters
for(int m = n; m; m--)Char_to_PC(*(s + m-1));}					//print last character first



char decimal_digit (char data){
if (((data > '9') || (data < '0')) )return 0;
else return 1;}

char wait_for_return_key(void){	
char keypress,temp;
keypress = waitforkeypress();
if((keypress == '\r') || (keypress == '\n')){
if (isCharavailable(1)){temp = receiveChar();}keypress = '\r';}
return keypress;}

/************************************************************************/
void I2C_Tx_8_byte_array(char s[]){				
char num_bytes=8; char mode=1;
I2C_Tx(num_bytes,mode, s);}





long I2C_displayToNum(void){		
long L_number = 0;
char receive_byte;
char num_bytes=0;
char mode = 'I';

waiting_for_I2C_master;		
send_byte_with_Ack(num_bytes);
send_byte_with_Nack(mode);
TWCR = (1 << TWINT) | (1 << TWEN);						//clear interrupt and leave I2C active
waiting_for_I2C_master;

for (int m = 0; m<=3; m++){							//Receive 4 chars and assemble into unsigned long result
if (m ==3){receive_byte = receive_byte_with_Nack();}
else {receive_byte = receive_byte_with_Ack();}
switch(m){
case 0: case 1: case 2:L_number =  L_number + receive_byte; 
L_number = L_number << 8; break;
case 3: L_number =  L_number + receive_byte; break;}}
clear_I2C_interrupt;
return L_number;}


/***********************************************************/
char receive_byte_with_Ack(void){
char byte;
TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWINT);		//Set Ack enable and clear interrupt
while (!(TWCR & (1 << TWINT)));						//Wait for interrupt
byte = TWDR;
return byte;}

/***********************************************************/
char receive_byte_with_Nack(void){
char byte;
TWCR = (1 << TWEN) | (1 << TWINT);						//Set Ack enable and clear interrupt
while (!(TWCR & (1 << TWINT)));						//Wait for interrupt
byte = TWDR;
return byte;}

/************************************************************************/
void USART_init (unsigned char UBRROH_N, unsigned char UBRR0L_N ){
UCSR0B = 0;
UBRR0H = UBRROH_N;  									
UBRR0L = UBRR0L_N;  								
UCSR0A = (1 << U2X0);
UCSR0B = (1 << RXEN0) | (1<< TXEN0);
UCSR0C =  (1 << UCSZ00)| (1 << UCSZ01);}
