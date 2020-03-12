
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdio.h>


/*****************************************************************************/




#define zero "abcdef"										//chars a,b,c,d,e and f are stored in an array named "zero"
#define one "bc"											//chars b and c are stored in an array named "one"
#define two "abged"											//Note: the compiler terminates each string in zero
#define three "abcdg"
#define four "fgbc"
#define five "afgcd"
#define six "fegdc"
#define seven "abc"
#define eight "abcdefg"
#define nine "abcfg"


#include "Numerical_entry_header_file.h"
#include "Numerical_entry_subroutines.c"



/*****************************************************************************/
int main (void){

char   digit;
int digit_num=0;											//defines number of next digit on display						
const char* string_ptr = 0;										//pointer: will be loaded with the address of a segment string 


CLKPR = (1 << CLKPCE);
CLKPR = (1 << CLKPS0);

setup_HW;													//(i.e. the address of string "zero", "one", "two" etc....) 

while(1){
do{String_to_PC("S  ");} 
while((isCharavailable(255) == 0));            //User prompt 
if(receiveChar() == 'S')break;}

String_to_PC("Send digits?");
while(1){

digit_num = 0;												//First digit on display

do{															//start of "do{}while();" loop
digit = waitforkeypress();									//user enters digit (0 to 9) at the PC keyboard
switch(digit){												//The appropriate address is loaded into location "string_pointer"
case '0': string_ptr = zero; break;							//The address of array zero is loaded into location "string_ptr"
case '1': string_ptr = one; break;
case '2': string_ptr = two; break;
case '3': string_ptr = three; break;
case '4': string_ptr = four; break;
case '5': string_ptr = five; break;
case '6': string_ptr = six; break;
case '7': string_ptr = seven; break;
case '8': string_ptr = eight; break;
case '9': string_ptr = nine; break;
default: continue; break;}									//Illegal key press: Go immediately to the start of the do loop

															//Send the address of the required string to subroutine "display_num_string();"
display_num_string(string_ptr, digit_num);digit_num++;} 
while (digit_num <= 7);										//return to the top of the "do" loop until all digits have been illuminated

waitforkeypress(); I2C_Tx_any_segment_clear_all();}}		//clear display and repeat





/*****************************************************************************/
void display_num_string (const char* s, int digit_num){			//Subroutine requires a pointer to the string		
int char_ptr=0;												//containing segments used to define a digit
char letter;

while(1){
letter = *(s + char_ptr);// (s[char_ptr]);					//Note these two expressions are equivalent
switch(letter){												//Work through the segments contained in the 
case 'a':													//string until '\0' is encountered
case 'b': 
case 'c': 
case 'd': 
case 'e': 
case 'f': 
case 'g': I2C_Tx_any_segment(letter, digit_num);break;		//update display one segment at a time
case 0:  return; break;										//zero indicates the end of the string
default: break;}char_ptr++;}}								//incrementing "char_ptr" steps through the string
															//Selecting segment letters in turn
