
/*Proj_8C_fp_arithmetic
*****************************************************/


/*A bit of practice with the maths library.*/


#include "FP_arithmetic_header_file.h"

#include <math.h>



int main (void){
char op;
double result, x1, x2;

setup_HW;

stdout = &uart_output;
stdin = &uart_input_Double;
printf("\r\nEnter fpn (cr) -op- fpn (cr)\r");
scanf("%lf", &x1);
putchar('?');

while(1){
stdin = &mystdin;
op = getchar();
printf("\b %c ", op);

stdin = &uart_input_Double;
scanf("%lf", &x2);

switch(op){
case '+':result = x1+x2; break;
case '-':result = x1-x2; break;
case '*':result = x1*x2; break;
case '/':result = x1/x2; break;
case '^':result = pow(x1,x2); break;
default: result = 0; break;}
printf(" = %g  ?",result);
x1 = result; printf("\r");}
return 0;}




/*************************************************************************************/
int uart_putchar(char c, FILE *mystr_output)
	{if (c == '\n')
	uart_putchar('\r', mystr_output);
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = c;
	return 0;}


	
/*************************************************************************************/
int uart_getchar(FILE *mystr_input)
{while (!(UCSR0A & (1 << RXC0)));
return UDR0;}



/*************************************************************************************/
int uart_getDouble(FILE *mystr_input)
{char keypress;
{while (!(UCSR0A & (1 << RXC0)));
keypress = UDR0;

if((keypress == '\r') || (keypress == '\n'))
{if(isCharavailable(1))receiveChar();}

while(!(decimal_digit (keypress)) && (keypress != '\r')
&& (keypress != '\n')&& (keypress != '-')&& (keypress != '.')
&& (keypress != 'e') && (keypress != 'E'))
{while((isCharavailable(100) == 0));keypress = UDR0;}

if ((keypress != '\r') && (keypress != '\n'))putchar(keypress);

return keypress;}}


