
/*Proj_5F_Combined_clock_SW
*********************************************************/



/*IT INTRODUCES


1.  A crystal controlled clock/Stop Watch application.

Timing is provided using a 10mS I2C interrupt provided by the mini-OS that puts all clock/SW features under the 
control of the "Project_SW".


2.  Two wire serial interface TWI.
  
This is the term Atmel use for the Inter IC bus also known as the I-squared C or I2C bus.  Within the limits 
of these projects TWI and I2C hardware and protocols appear to be identical.


3.  An "atomic process".

Elapsed time is recorded in seconds in a variable known as "sec_counter".  It is updated by a TWI ISR and processed 
by "main" and other subroutines. If any of these processes were interrupted by the TWI ISR then "sec_counter" would 
get corrupted and the process would fail.
A different variable "sec_counter_V" is therefore defined.   This is the variable that the ISR updates directly.
Its value is copied to "sec_counter" at regular intervals. Global interrupts are disabled while this process is 
in progress, using the following code

  " cli(); sec_counter = sec_counter_V; sei();"

4.  Project subroutine "I2C_initiate_10mS_ref();" 

This is used to initiate the 10mS TWI interrupt
Note:
The firmware can crash during a TWI data transfer and to guard against this happening the Watch dog timer has been 
implemented in Interrupt and System Reset Mode on both Atmega devices. The 7 digit clock can be changed to 8 digits 
(10ms tick) see optional line in subroutine “update_7_seg_display()”.



EEPROM USAGE

address 2 Initial mode: clock or SW
addresses 3-6 Time input by user
addresses 7-10 "sec_counter"
addresses 11-14 "clock time" in seconds: When a new SW is selected save the "sec_counter" in clock time and 
"reset sec_counter"
address 15 mode
address 16 The number of WD time outs.



USER INSTRUCTIONS

Connect to pc
press sw1 get message enter time (if not reset and repeat)
Enter time 24 hour clock
Power cycle to 5V supply if wanted 
press sw2 to start
press sw2 to toggle between clock and new SW
press sw3 to restore old SW
press sw1 to pause, blank and restore display
At initial POR press sw3 (rather that sw1) to get two SW's

To adjust time: Press SW2 to restore clock time
Then immediately press SW1
This will cause a WD time out
Release sw1 to increment time
sw3 to decrement time
Both switches to restore normal operation. 


Switch location SW1(PD2) - SW2(PD7) - SW3(PB2)
PD2&7 call ISR(PCINT2_vect)
PB2 calls ISR(PCINT0_vect)
ISR on SW2 is never used.*/




#include "Proj_5F_header_file.h"



long clock_time_secs=0;                                                         //Value is seconds: Backed up in EEPROM when SW is selected


int main (void){

long SW_resume_time = 0;                                                          //Value in seconds: Used when switching from clock to SW
long clock_resume_time=0;                                                         //Value is seconds: Used when switching from SW to clock time


/*****************Normal program entry mode: Everything is OK********************************/
if(Arduino_non_WDTout){
clear_Arduino_WDT_flag;

setup_UNO;

if (eeprom_read_byte((uint8_t*)(2)))
{User_prompt;
User_instructions;}

for(int m = 0; m<=11; m++)
{eeprom_write_byte((uint8_t*)(m+7), 0);}                                         //clear BKP space
//eeprom_write_byte((uint8_t*)(19), 0xFF);

while(1){if(switch_3_down) {mode = 2;break;}                                     //Select timer or clock
    if(switch_1_down) {mode = 1;break;}}
while((switch_1_down) || (switch_3_down));

clock_time_secs=0;
old_mode = 0;
payload=2;}

 

/*****************WDT revovery mode (usually caused by failure of the I2C)**********************/
if(!(Char_from_EEPROM(0x3F1))){
clear_Arduino_WDT_flag;
      
setup_UNO;                                                                        //This macro clears the WD flag

sec_counter=eeprom_read_byte((uint8_t*)(10));                                     //Read saved value of sec_counter from EEPROM   
for(int m = 0; m<3; m++){
sec_counter = (sec_counter<< 8)
+ eeprom_read_byte((uint8_t*)(9-m));}       

if((switch_1_down) && (switch_2_down)){                                            //User induced watch dog timeout:
while((switch_1_down) || (switch_2_down)){                                        //Use sw_1 and sw_3 to adjust time
if((switch_1_up) && (switch_2_down)){sec_counter += 1;}
if((switch_2_up) && (switch_1_down)){sec_counter -= 1;}
Timer_T0_10mS_delay_x_m(10);
Calculate_time();
digits[0]='0'; digits[1] = '0';                                                   //Sets mS*10 to zero  masks mS*10     
I2C_Tx_8_byte_array(digits);}}                                                    //Displays time as adjusted by user

mode = eeprom_read_byte((uint8_t*)15);                                            //Obtain "mode" and "clock_time_secs" from EEPROM
clock_time_secs=eeprom_read_byte((uint8_t*)(14));   
for(int m = 0; m<3; m++){
clock_time_secs = (clock_time_secs<< 8) + eeprom_read_byte((uint8_t*)(13-m));}
    
    sec_counter +=1;                                                             //Fudge factor??
    clock_resume_time = sec_counter;
    sec_counter_V = sec_counter;                                                 //Volatile version of "sec_counter"
    ms_counter=0;                                                               //mSecs are not restored
    clock_time_long = clock_time_secs * 100;
    Calculate_time();
    payload=8;                                                                  //After WDT out update display
    setup_64ms_WDT_with_interrupt;                                              //Re-establish WDT
    I2C_initiate_10mS_ref();sei();}                                             //Reinitialise 10mS tick


/**************************Normal program continues********************************/
setup_and_enable_PCI;
disable_pci_on_sw2;
disable_pci_on_sw3;
display_clear = 0;

while(1){                                                                       //Main loop: Continuously cycles through this loop (every 188uS??).  

switch(mode){
case 0: break;
case 1: set_time();                                                             //Clock mode: "set_time()" requests start time  & saves it in "sec_counter"
mode = 'A';break;   
case 2: sec_counter = 0;                                                        //SW mode: start at time zero                             
    mode = 'A'; 
    eeprom_write_byte((uint8_t*)(0x02),0);                                      //set clock mode
    for(int m = 0; m<=3; m++)                                                   //Clear EEPROM sec_counter value
    {eeprom_write_byte((uint8_t*)(m+3),0);}
    break;
case 'A': 
  ms_counter = 0;
  sec_counter_V = sec_counter;                                                  //Initialise sec_counter_V
  Calculate_time();                                                             //Converts time in seconds to string (Hr,Min and Sec)
  digits[0]='0'; digits[1] = '0';                                               //Sets mS*10 to zero  masks mS*10     
  I2C_Tx_8_byte_array(digits);                                                  //Displays time 
  clock_resume_time = sec_counter;
  mode = 'E'; 
  break;
  
case 'E':                                                                       //Clock ready to start: Awaiting sw3 press
  if(switch_2_up); else{
  I2C_initiate_10mS_ref();                                                      //Initiate TWI interrupt (every 10mS)
  sei();
  mode = 'B';
  setup_64ms_WDT_with_interrupt;}                                               //User input finished: Start WDT
  break;                      

case 'B': if(!(TWI_flag)); 
else {TWI_flag = 0; Update_sec_counter;                                        //Clock running mode
old_mode = 'B'; 
if(((86400 + sec_counter - clock_resume_time)%86400>1) && ((switch_3_down) || (switch_2_down)))
{if(switch_2_down){clock_time_secs = clockTOstop_watch();mode = 'F';}
if(switch_3_down){SW_resume_time = restore_stop_watch(); mode = 'H';}}
else Timer();} 
break;                                                                          //Convert clock to stop watch

case 'F': if(!(TWI_flag)); 
else {TWI_flag = 0; Update_sec_counter;                                         //Stop watch mode     
old_mode = 'F'; 
if(((sec_counter)>1) && ((switch_2_down)))
{clock_resume_time = restore_Clock();mode = 'B'; }
else Timer();}
break;                                                                          //Restore clock

case 'H': if(!(TWI_flag)); 
else {TWI_flag = 0;   Update_sec_counter;                                       //Stop watch restored mode      
old_mode = 'H'; 
if(((86400 + sec_counter-SW_resume_time)%86400 > 1) &&  (switch_2_down))
{clock_resume_time = restore_Clock();mode = 'B'; }
else Timer();}
break;  

case 'C':  
payload = 0; TWI_flag = 0; 
Update_sec_counter; break;                                                     //Stop watch/clock display paused: Tick continues

case 'G': 
payload = 0; TWI_flag = 0; Update_sec_counter; 
if (!(display_clear)) clear_display();break;                                    //blank display

case 'D': 
TWI_flag = 0;
restore_display();  mode = old_mode;                                           //Stop watch/clock display re-activated
break;}

Timer_T2_sub(T2_delay_2ms);
wdr();}}                                                                        //reset the watchdog


/*****************************************************************************************************************/
void Timer(){                                                                   //Calls "update_7_seg_display", to close one I2C transaction and sets "payload" to initiate the next one    
  digits[1] = ms_counter/10 + '0';                                              //Calculate mS display MSB
  digits[0] = ms_counter%10 + '0';                                              //Calculate mS display LSB
  if (!((digits[0] == '0')&&(digits[1] == '0')))                                //For mS tick 0 to 99 only update mS display                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
  {update_7_seg_display();}                                                     //Update mS display
  if(!(ms_counter%100)){                                                        //When 1 second tick occurs
  Calculate_time();                                                             //Calculate time: Hr:Min:Sec
  update_7_seg_display();}                                                      //Update display and complete the current I2C transaction
  if(ms_counter == 99)payload=8; else payload=2;}                               //Set payload for next TWI_interrupt 8 for 1sec tick, 2 for 10mS tick


/*****************************************************************************************************************/
ISR(TWI_vect){                                                                  //Detects 10mS interrupt. Initiates I2C_transaction and responds by sending payload size
unsigned char Null_byte;
TWI_flag=1;                                                                     //Informs main loop that a 10mS tick has occurred
ms_counter = (ms_counter+1)%100;                                                //mS counter continuously cycles through 0 to 100
if(!(ms_counter)) {sec_counter_V++;                                             //Volatile version of "sec_counter" increments every second
if(sec_counter_V == 86400)sec_counter_V = 0;                                    //Roll-over for 24 hour clock
sec_counter_save=1;}                                                            //Enables "sec_counter" to be updated from "sec_counter_V"
Null_byte =  TWDR;                                                              //Detect slave address + slave transmit data command
TWDR = payload;                                                                 //Transmit payload value
TWCR = (1 << TWEN) | (1 << TWINT);                                              //clear interrupt but no ack required Next byte will be the last
while (!(TWCR & (1 << TWINT)));                                                 //Wait for TWINT flag 
if (!(payload))
TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);}                 //No payload? Prepare for next 10ms interrupt


/*****************************************************************************************************************/
void update_7_seg_display(void){
unsigned char Null_byte;
cli();                                                                        //Pause twi interrupt
hold_PCI_on_sw1_and_sw2;
{digits[0] = digits[1]; digits[1] = 0;}                                       //Optional: masks 10mS digit
TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);                              //Enable slave mode and set Enable Acknowledge
while (!(TWCR & (1 << TWINT)));                                               //No interrupt: wait for comms from master
Null_byte =  TWDR;                                                            //Read slave adress
for (int m = 0;  m < payload; m++){
TWDR = digits[m];                                                             //Transmit data for display
if (m==(payload-1))TWCR = (1 << TWINT) | (1 << TWEN);                         //last byte being transmitted now expect NACK in response
else{TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);}                        //ACK pulse normally required
while (!(TWCR & (1 << TWINT)));}                                              //Clear interrupt
TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);                //Prepare for next 10ms interrupt
restore_PCI_on_sw1_and_sw2;
sei();}                                                                       //re-enable twi interrupt


/*****************************************************************************************************************/
void Calculate_time(void){
Hours = sec_counter/3600;                                                   //Obtain hours 0 to 23
Minutes = (sec_counter%3600)/60;                                            //Obtain minutes
Seconds = (sec_counter%3600)%60;                                            //Obtain seconds      
timer_utoa(Hours); HoursH = charH; HoursL = charL;                          //Convert hours to string 
timer_utoa(Minutes); MinsH = charH; MinsL = charL;                          //Convert minutes to string
timer_utoa(Seconds); SecsH = charH; SecsL = charL; }                        //Convert seconds to string                   


/*****************************************************************************************************************/
void set_time(void){
unsigned int Two_sec_counter;                                               //Required because of difficulties combining long and char variables            
unsigned char clock_mode;

clock_mode = eeprom_read_byte((uint8_t*)0x02);                              //Read mode. Default value is 255
switch(clock_mode){

case 255:                                                                   //Default mode: User enters time
String_to_PC("\r\nEnter start time Hours, Minutes and \
Seconds (24 hour clock, no spaces)\r\n");
while(isCharavailable(50) == 0){String_to_PC("T?  ");}    
    HoursH = receiveChar();     
    while(isCharavailable(5) == 0); HoursL = receiveChar();
    while(isCharavailable(5) == 0); MinsH = receiveChar();
    while(isCharavailable(5) == 0); MinsL = receiveChar();
    while(isCharavailable(5) == 0); SecsH = receiveChar();
    while(isCharavailable(5) == 0); SecsL= receiveChar();
    msecsH = 0; msecsL = 0; 
    Two_sec_counter = ((((HoursH-'0') * 10) + HoursL-'0') * 1800)\
    + ((((MinsH-'0') * 10) + MinsL-'0') * 30) + ((SecsH-'0') * 10 + SecsL-'0')/2; 
    sec_counter = Two_sec_counter;
    sec_counter *= 2;             
    if (((SecsH-'0') * 10 + SecsL-'0')%2)sec_counter++;
    
    
    for(int m = 0; m<=3; m++)
    {eeprom_write_byte((uint8_t*)(m+3),sec_counter >> (m*8));
    Timer_T0_sub(T0_delay_5ms);}            
    eeprom_write_byte((uint8_t*)(0x02),0);Timer_T0_sub(T0_delay_5ms);   
    //eeprom_write_byte((uint8_t*)(19), 0);                                     //Ensures that User_prompt is skippped when device is switched to 5V supply
    break;

case 0: 
    sec_counter=eeprom_read_byte((uint8_t*)(6));                              //Read start time and place in sec_counter
    for(int m = 0; m<3; m++){
    sec_counter = (sec_counter<< 8) + eeprom_read_byte((uint8_t*)(5-m));}
    eeprom_write_byte((uint8_t*)(0x02),255);break;    
default: eeprom_write_byte((uint8_t*)(0x02),255);
wdt_enable(WDTO_30MS); 
while(1);break; }}                                                            //Corrupt clock_mode variable: set to default value and restart


/*****************************************************************************************************************/
void timer_utoa(char n){                                                       //converts an unsigned number to a string of digits
if (n>=10)
{charL = n%10;
charH = (n-charL)/10;
charL = charL + '0';
charH = charH + '0';}
else
{charH = '0'; charL = n + '0';}}


/*****************************************************************************************************************/
ISR(PCINT2_vect) {                                                            //PORTB switch_1 interrupts

if(switch_1_up)return;                                                        //Ignore interrupt on switch release
if(switch_2_up){                                                              //User triggered WDTout to adjust clock

switch(mode){
case 0: mode = 1; break;                                                      //Enter the start timer
case 'H':
case 'F':
case 'B':  mode = 'C'; break;                                                 //Mode B clock is running Mode C Display paused
case 'C':  mode = 'G'; break;                                                 //Mode G clear display
case 'G':  mode = 'D';break;}}

else

if(mode != 'B')return;
else { 
payload = 8;sei(); while(1);}}


/*****************************************************************************************************************/
long clockTOstop_watch(void){                                                  //Saves time before selecting stop watch display (i.e. resetting time to zero)
long clock_time_secs_L;
clock_time_long = (sec_counter * 100) + ms_counter;                            //Current time "secs*10 + ms" saved in global variable clock_time.
ms_counter = 0;                                                                 //Reset msecs
sec_counter = 0;                                                                //Reset secs
clock_time_secs_L = clock_time_long/100;                                        //Ignore ms when saving clock time
update_7_seg_display();                                                         //Runs "update_7_seg_display()" to complete the current I2C transaction
payload = 8;                                                                    //Set payload to 8 for full display update during next I2C transaction
while(!(ms_counter));                                                           //Wait for next interrupt ms_counter increments from 0 to 1
TWI_flag = 0;                                                                   //Clear TWI_flag
for(int m = 0; m<=7; m++)digits[m] = '0';                                        //Clears all digits
update_7_seg_display();                                                         //Runs "update_7_seg_display()" to complete I2C transaction
payload=2;                                                                      //Payload of 2 required for next update
cli();sec_counter_V = 0;sei();                                                  //Update volatile version of the sec_counter and return to "main loop"
return clock_time_secs_L;}


/*****************************************************************************************************************/
long restore_Clock(void){                                                       //Restores time by adding "clock_time" to the current SW time
long time, clock_restore_time;
unsigned char x;
time = (sec_counter * 100) + ms_counter + clock_time_long;                      //Add saved "clock_time" to SW time to get current value of time      
sec_counter = time/100;                                                         //Calculate new value of "sec_counter" and "ms_counter"
ms_counter = time%100 - 2;                                                      //2mS fudge factor required for mseconds
if(sec_counter >=86400)sec_counter -=86400;                                     //compensates for 24Hr roll-over
update_7_seg_display();                                                         //Run "update_7_seg_display()" to complete the current I2C transaction
x=ms_counter;                                                                   //Record "ms_counter"
clock_restore_time = sec_counter;                                               //saves "sec_counter" in "clock_restore_time"
Calculate_time();                                                               //Calculate time Hr:Min:sec
digits[1] = ms_counter/10 + '0';                                                //Calculate mS display MSB
digits[0] = ms_counter%10 + '0';                                                //Calculate mS display LSB
payload = 8;                                                                    //Sets the payload to 8 ahead of next I2C transaction (update full disdplay)
while(ms_counter==x);                                                           //Wait for next interrupt (ms_counter increments from x to x+1)
TWI_flag = 0;                                                                   //Set TWI_flag to zero
update_7_seg_display();                                                         //Full display update (completes the current I2C transaction)
if(ms_counter == 99)payload=8; else payload=2;                                  //Enables contiguous 8 digit updates when necessary
cli();sec_counter_V = sec_counter;sei();                                        //Update volatile version of second_counter
newline();return clock_restore_time;}                                           //Return "clock_restore_time" to "main loop"


/*****************************************************************************************************************/
long restore_stop_watch(){                                                      //Restores SW time by subtracting the current time from the value stored in "clock_time"
unsigned long time, SW_restore_time;
unsigned char x;
time = 8640000 + (sec_counter * 100) + ms_counter - clock_time_long;            //Computes the updated value of SW time
if (time >= 8640000) time -= 8640000;                                           //Compensate for 24 Hr roll-over
sec_counter = time/100;                                                         //Calculate new value of "sec_counter" and "ms_counter"
ms_counter = time%100 - 2;                                                      //2mS fudge factor required for mseconds
update_7_seg_display();                                                         //Run "update_7_seg_display()" to complete the current I2C transaction
x=ms_counter;                                                                   //Record "ms_counter"
SW_restore_time = sec_counter;                                                  //saves "sec_counter" in "SW_restore_time"              
Calculate_time();                                                               //Calculate time Hr:Min:sec
digits[1] = ms_counter/10 + '0';                                                //Calculate mS display MSB
digits[0] = ms_counter%10 + '0';                                                //Calculate mS display LSB
payload = 8;                                                                    //Sets the payload to 8 ahead of next I2C transaction (update full disdplay)
while(ms_counter==x);                                                           //Wait for next interrupt (ms_counter increments from x to x+1)
TWI_flag = 0;                                                                   //Clear TWI_flag
update_7_seg_display();                                                         //Full display update (completes the current I2C transaction)
if(ms_counter == 99)payload=8; else payload=2;                                  //Enables contiguous 8 digit updates when necessary
cli();sec_counter_V = sec_counter;  sei();                                      //Update volatile version of the sec_counter
return SW_restore_time;}                                                        //Return "SW_restore_time" to "main loop"


/*****************************************************************************************************************/
void restore_display(void){                                                     //Re-activate Stop watch/clock display
int x;
{Calculate_time();                                                              //calculate time in Hours, mins and secs 
x = ms_counter;                                                                 //Prepare to wait for next interupt
payload = 8;                                                                    //Full display to be updated at next I2C transaction
while(ms_counter==x);                                                           //Wait for next interrupt 
update_7_seg_display();                                                         //Update display: I2C expects a transfer of 8 bytes 
if(ms_counter == 99)payload=8; else payload=2;                                  //Set payload for next interrupt
TWI_flag = 0;                                                                   //Clear TWI flag
display_clear = 0; }}                                                             //Resets global variable "display_clear"


/*****************************************************************************************************************/
void clear_display(void){                                                       //Blanks the display
unsigned char x;
display_clear = 1;                                                              //sets global variable "display_clear"                
for(int m = 0; m<=7; m++)digits[m]=0;                                           //Clears all digits
x=ms_counter;                                                                   //records current value of "ms_counter"
payload = 8;                                                                    //Prepare for full display ubdate at next interrupt
while(ms_counter==x);                                                           //Wait for next interrupt ("ms_counter" increments to x+1)
TWI_flag = 0;                                                                   //Reset "TWI_flag"
update_7_seg_display();}                                                        //Complete I2C_transaction



/*****************************************************************************************************************/
/*ISR(TIMER0_OVF_vect){
T0_OVF_FLAG = 2;
TCCR0B = 0;
TIMSK0 &= (~(1 << TOIE0));}
*/

/*****************************************************************************************************************/
ISR (WDT_vect){                                                                //the clock often crashes when the display 
set_Arduino_WDTout;                                                            //is being switch between stationary and blank
for(int m = 0; m<=3; m++)                                                      //WDTouts are also used to adjust the clock
{eeprom_write_byte(((uint8_t*)(7+m)), (sec_counter >> (m*8)) );}  

switch (mode){                                                                  //Save mode
case 'B':
case 'F':
case 'H':
eeprom_write_byte((uint8_t*)(15),mode); 
case 'C':  case 'G': case 'D': eeprom_write_byte((uint8_t*)(15),old_mode);
default: break;}

for(int m = 0; m<=3; m++)
{eeprom_write_byte(((uint8_t*)(11+m)), (clock_time_secs >> (m*8)) );}}
