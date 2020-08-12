

char waitforkeypress(void);
char receiveChar(void);
char isCharavailable(char);
char wait_for_return_key(void);
void sendChar(char);
void sendString(const char*);
void newline(void);
void sendLongNum(char, long);
void SBtoAL(char*, long, char);
void sendNumericString(char*);
char decimal_digit (char);
char decimal_digit_or_x (char);
char UC_from_KBD(void);



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



/*********************************************************************/
char wait_for_return_key(void){	
char keypress;		// temp = 0;
keypress = waitforkeypress();
if((keypress == '\r') || (keypress == '\n')){
if (isCharavailable(1)){receiveChar();}keypress = '\r';}
return keypress;}




/*********************************************************************/
void sendChar(char data){
while (!(UCSR0A & (1 << UDRE0)));
UDR0 = data;}



/*********************************************************************/
void sendString(const char s[]){
int i = 0;
while(i < 200){
if(s[i] == '\0') break;
sendChar(s[i++]);} }



/*********************************************************************/
void newline(void){sendString ("\r\n");}




/*********************************************************************/
void sendLongNum(char radix, long long_num){
char array[12];														//Long has 10 chars + sign + null terminator	
SBtoAL(array, long_num, radix);										//calls the Binary to askii subroutine
sendNumericString(array);}											//Prints characters in reverse order




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
void sendNumericString(char* s){					
int n=0;
while (s[n] != '\0')n++;											//scroll to end of string counting the number of characters
for(int m = n; m; m--)sendChar(*(s + m-1));}						//print last character first




/*********************************************************************/
char decimal_digit (char data){
if (((data > '9') || (data < '0')) )return 0;
else return 1;}



/*********************************************************************/
char decimal_digit_or_x (char data){
if (((data <= '9') && (data >= '0')) || (data == 'x') )return 1;
else return 0;}




/*********************************************************************/
char UC_from_KBD(void){
char keypress;
long number;

do
{keypress =  waitforkeypress();} 
while (!(decimal_digit_or_x(keypress)));

if (keypress == 'x')return 'x';

newline(); sendChar(keypress);
keypress -= '0';
number =  keypress;

while(1){
if ((keypress = wait_for_return_key())  =='\r')break;
if (decimal_digit (keypress))	
{sendChar(keypress);
keypress -= '0';
number = number * 10 + keypress;}}
return number;}

