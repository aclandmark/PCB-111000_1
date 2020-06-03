
/*Proj_6E_bitwise_ops
******************************************************************/


/*More on bitwise ops


IT INTRODUCES 

1.  The shift operators  << and >>.
These operators will frequently be encountered when setting up the HW (usually in header files).

Note: For signed chars the >> operator maintains the sign.  For negative numbers bit 7 is set to one
at each shift operation untill the result 0b11111111 (i.e. -1) is obtained.
However if the byte diretive is used (see line 106) bit 7 is set to zero at each shift operation
until the result zero is obtained.

2.  Project subroutine "binary_char_from_KBD()"

Note press x or X to escape
r or R to start
These options give 4 modes of operation.*/




#include "Proj_6E_header_file.h"

char digits[8];


int main (void){
char op_code;
unsigned int X;
char User_response, keypress;

setup_UNO_extra;
User_instructions;
User_prompt;

String_to_PC("\r\nSelect logical operation \r\n\
1 for << X  Shifts a number left\r\n\
2 for >> X  Shifts a number right \r\n");
String_to_PC("\
3 for | (1 << X)  Changes just one bit by setting it to 1.\r\n\
4 for &(~(1 << X) Changes just one bit by setting it to 0.\r\n");
String_to_PC("\
5 for XOR(1 << X) Changes just one bit by reversing it.\r\n\
6 for &(1 << X) Tests a single bit\r\n");

waitforchar();
while(1){
op_code = receiveChar();    


    switch(op_code){
    case '1':
    case '2': String_to_PC("X? 1-7\t"); X = waitforkeypress()-'0'; break;
    case '3':
    case '4':
    case '5':
    case '6': X = 1;  break;
    default: wdt_enable(WDTO_30MS); while(1); break;}                     //Exit program
    
    
    switch(User_response){
    case 'R': 
    String_to_PC("Enter binary number: 1 to 0b11111111\
terminate in cr then AK  "); 
    dig_0 = (binary_char_from_KBD_Local()); break;
    
    case 'r': String_to_PC("Random number used: AK   "); 
    dig_0 = PRN_8bit_GEN_UNO();
    break;}
    
    dig_1 = (X);
    dig_2 = dig_0;   
    
    do{
    I2C_Tx_BWops(digits);                                                   //update the display
    keypress=waitforkeypress();
    dig_2 = logical_op((X),dig_0, op_code, User_response);                  //perform logical op and update dig_2
    switch (op_code){
    case '1': 
    case '2': X = dig_1 + X;break;                                          //update X for op_codes 1 and 2
    default: dig_1 = (X); X = (X <<1)%255;  break;}                         //update X and dig_1 for op-codes 3-6
    }while((keypress != 'x') && (keypress != 'X'));
    I2C_Tx_any_segment_clear_all();
    if(keypress == 'X'){wdt_enable(WDTO_30MS); while(1);}
    String_to_PC("New mode?\r\n");
    waitforchar();}} 



/*************************************************************************************/ 
char logical_op(char X, char Y, char op_code, char mode){
char result = 0, n = 0;
unsigned char num;

switch (op_code){

case '1': if(mode == 'r'){Rotate_Right_cyclical; 
result = Y;}else {result = Y << X;} break;
case '2': if(mode == 'r'){Rotate_left_cyclical; 
result = Y;} else {result = (Y >> X);}break;

case '3': result = Y | X; break;
case '4': result = Y &(~(X)); break;
case '5': result = Y ^ (X); break;
case '6': result = Y & (X); break;}
return result;}



/*************************************************************************************/
char binary_char_from_KBD_Local(void){
char keypress=0;
char LSB;

for(int m = 0; m<=7; m++)digits[m]=0;
do{
LSB = wait_for_return_key(); 
if (LSB == '\r')break;
if((LSB != '0') && (LSB != '1'));

else {keypress = (keypress << 1) + LSB - '0';
digits[0] = keypress;
I2C_Tx_BWops(digits);
}} while(1);

return keypress;}
