
/**************Proj_5B_Demo_Clock********************/

/*At the user prompt press 'r' to start a timer or
R to enter the time and then start the clock*/



#include "Proj_5B_header_file.h"


void set_time(void);
void timer_utoa(unsigned char);
void Display_time(void);

char digits[8];                                                               //Global variables
unsigned char charH, charL, Hours, Minutes, Seconds;
unsigned int sec_counter;


int main (void){
char User_response;
setup_UNO_extra;
User_prompt;

if(User_response == 'R'){set_time();}
  
else {reset_clock_1; msecsH = 0; msecsL = 0;  }
I2C_Tx_8_byte_array(digits);

String_to_PC("AK to start\r\n");
waitforkeypress();binUnwantedChars ();

while(1){Timer_T0_10mS_delay_x_m(100);sec_counter++; Display_time();}}		//1 second tick



/**********************************************************************************************************************/
void Display_time(void){
sec_counter = sec_counter%43200;                                            //Reset sec_couter every 43200 seconds at 11:59:59
Hours = sec_counter/3600;
Minutes = (sec_counter%3600)/60;
Seconds = (sec_counter%3600)%60;
timer_utoa(Hours); HoursH = charH; HoursL = charL;                          //HoursH & L are defined as digits[7] and [6] 
timer_utoa(Minutes); MinsH = charH; MinsL = charL;                          //MinsH & L are defined as digits[5] and [4]  
timer_utoa(Seconds); SecsH = charH; SecsL = charL;                          //SecsH & L are defined as digits[3] and [2]
I2C_Tx_8_byte_array(digits);}


/**********************************************************************************************************************/
void set_time(void){
String_to_PC("\r\nEnter start time Hours, Minutes\
and Seconds (12 hour clock, no spaces)\r\n");                               //Requests user to enter start time
while(isCharavailable(50) == 0){String_to_PC("T?  ");}    
    HoursH = receiveChar();
    while(isCharavailable(5) == 0); HoursL = receiveChar();
    while(isCharavailable(5) == 0); MinsH = receiveChar();
    while(isCharavailable(5) == 0); MinsL = receiveChar();
    while(isCharavailable(5) == 0); SecsH = receiveChar();
    while(isCharavailable(5) == 0); SecsL= receiveChar(); 
    msecsH = 0; msecsL = 0; 
    sec_counter = 
    ((((HoursH -'0') * 10) + HoursL-'0') * 3600) +    
    ((((MinsH-'0') * 10) + MinsL-'0') * 60) +(SecsH-'0') * 10 +     
    SecsL - '0'; }                                                         //Start time is converted to seconds
    

/**********************************************************************************************************************/
void timer_utoa(unsigned char n){                                         //Converts an unsigned number to either one ot two askii characters
if (n>=10)                                                                //The number is between 10 and 59) 
{charL = n%10;
charH = ((n-charL)/10) + '0';
charL = charL + '0';}
else
{charH = '0'; charL = n + '0';}}                                          //The number is between 0 and 9
