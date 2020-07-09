
/*Proj_6E_bitwise_ops
******************************************************************/


/*More on bitwise ops


IT INTRODUCES 

1.	The shift operators  << and >>.
These operators will frequently be encountered when setting up the HW (usually in header files).

2.	Project subroutine "binary_char_from_KBD()"

Note press x or X to escape
r or R to start
These options give 4 modes of operation.*/




#include "Proj_6E_header_file.h"

char Op(char, char);


char digits[8];
char BWOp, comp;	

int main (void){
char op_code;
char X;
char User_response, keypress;

setup_UNO_extra;
User_instructions;
User_prompt;




switch (User_response)
{ case 'R':

String_to_PC("\r\nSelect logical operation \r\n\
1 for << X	Shifts a number left\r\n\
2 for >> X	Shifts a number right \r\n");
String_to_PC("\
3 for << X	Rotate a number left\r\n\
4 for << X	Rotate a number right\r\n");
String_to_PC("\
5 for | (1 << X)	Changes just one bit by setting it to 1.\r\n\
6 for &(~(1 << X)	Changes just one bit by setting it to 0.\r\n");
String_to_PC("\
7 for XOR(1 << X)	Changes just one bit by reversing it.\r\n\
8 for &(1 << X)	Tests a single bit\r\n");

waitforchar();
while(1){
op_code = receiveChar();	 	


	switch(op_code){
		case '1':
		case '2':
		case '3':
		case '4': String_to_PC("X? 1-7\t"); X = waitforkeypress()-'0'; break;
		
		
		case '5':
		case '6':
		case '7':
		case '8': X = 1;  break;
		default: wdt_enable(WDTO_30MS); while(1); break;}			//Exit program
		
		String_to_PC("Random number used: AK   "); 
		dig_0 = PRN_8bit_GEN();
		dig_1 = X ;
		//dig_2 = 0;
		dig_2 = dig_0;

		do{
		I2C_Tx_BWops(digits);										//update the display
		keypress=waitforkeypress();
		dig_2 = logical_op(X,dig_0, op_code); 		//perform logical op and update dig_2
		
		switch (op_code){
		case '1':
		case '2':		
		case '3': 
		case '4': X += dig_1; break;								//update X for op_codes 1 and 2
		default: dig_1 = X; X = (X <<1)%255;	break;}				//update X and dig_1 for op-codes 3-6
		}while((keypress != 'x') && (keypress != 'X'));
		
		I2C_Tx_any_segment_clear_all();
		if(keypress == 'X'){wdt_enable(WDTO_30MS); while(1);}
		String_to_PC("New mode?\r\n");
		waitforchar();}break;	
		
		
		
		
		
		
	case 'r':	
		String_to_PC("Select OP: 	| 	^ 	& 	~| 	~^ 	or	~&");
BWOp = waitforkeypress();
if (BWOp == '~') {comp = 1; BWOp = waitforkeypress();}else comp = 0;				//detect complement operator
if ((BWOp != '|') && (BWOp != '^') && (BWOp != '&'))
{wdt_enable(WDTO_30MS); while(1);}													//reset if duff char was sent 
	

String_to_PC("\r\nUse random numbers: Press AK or x to escape\r\n");				//Operates on random data
while (waitforkeypress() !='x'){
digits[0] = PRN_8bit_GEN();
digits[1] = PRN_8bit_GEN();
digits[2] =  Op(digits[0] , digits[1]);
I2C_Tx_BWops(digits);}									

I2C_Tx_any_segment_clear_all();

wdt_enable(WDTO_30MS); while(1);break;}
		
		
		
		} 



/*************************************************************************************/	
char logical_op(char X, char Y, char op_code){
char result = 0, n = 0, num;
switch (op_code){


case '1': result = Y << X;break;
case '2': result = Y >> X;break;

//case '1': {Rotate_Right_cyclical; result = Y;}break;
//case '2': if(mode == 'r'){Rotate_left_cyclical; 
//result = Y;} else {result = Y >> X;}break;



case '3': Rotate_Right_cyclical; result = Y; break;
case '4': Rotate_left_cyclical; result = Y; break;



case '5': result = Y | X; break;
case '6': result = Y &(~(X)); break;
case '7': result = Y ^ (X); break;
case '8': result = Y & (X); break;}
return result;}



char Op(char A, char B)
{char result=0;
switch (BWOp){
case '|': result = A | B; break;
case '^': result = A ^ B; break;
case '&': result = A & B; break;}
if (comp == 1) result = ~result;
return result;}

