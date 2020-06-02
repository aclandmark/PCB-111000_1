
/**************Proj_1F_LED_display***************************
Generates 6 displays depending on the pcb switch presses****/



#include "Proj_1F_header_file.h"



volatile char T1_OVF;                           //T1_OVF is set to 1 by TIMER 1 ISR or alternatively by a keypress ISR
unsigned int PORT_1,PORT_2,n, n_max;           //"n" records the increment number and "n_max" records the value at which it is reset to 0    
char switch_press, p;                           //"switch_press" is used to remember which switches have most recently been pressed
 
                                                
                                                //here it can vary between 1 and 5 resulting in 5 different displays
int main (void){
setup_UNO_extra;
setup_and_enable_PCI;                           //All three user switched generate PCI
T1_OVF = 0;
sei();
T1_65ms_clock_tick();                           //Timer_1 interrupt generated every 65mS
initialise_display();
wdt_enable(WDTO_120MS);                         //Set up watch dog timer: This will be studied shortly

while(1)                                        //Start a continuous loop
{Inc_Display(switch_press);                     //Increment the display depending upon which user switches have been pressed
while(!(T1_OVF));T1_OVF = 0;                    //Wait for the timer or keypress interrupt and then reset it.
wdr();}}                                        //Watchdog reset (prevents it from timing out).
                                                //a watchdog timeout re-establishes "key_press" as 0 which cannot be selected using the switches


/************Interrupt Service routines****************************/
ISR(TIMER1_OVF_vect) {T1_OVF = 1;}              //Timer interrupt ISR
ISR(PCINT0_vect) {                               //ISR for PCI on sw3
if(switch_3_up)return;                            //No interrupts on switch release
if(switch_3_down) switch_press = 5;               //Depending on the switch presses initialise the display 
initialise_display();}                            //and then increment it appropriately


ISR(PCINT2_vect) {                                //ISR for PCI on sw_1 and sw_2
if(switch_2_down) {if(switch_press == 5)switch_press = 2; else switch_press = 1; initialise_display();}
if(switch_1_down) {if(switch_press == 5)switch_press = 4; else switch_press = 3; initialise_display();}}



/***********initialise_display() subroutine***********************/
void initialise_display(void){
n=0;
switch(switch_press){

case 0: PORT_1 = 1<<p; PORT_2 = 0x8000>>p; n_max = 16-(2*p); break;
case 1: PORT_1 = 1; PORT_2 = 1; n_max = 16; break;
case 2: n=1;
case 3: PORT_1=0b0000000000000001;
    PORT_2=0b1000000000000000; 
    n_max = 16;break;
case 4: 
    PORT_1 = (0xFFFF >> 1) ^ (0xFFFF << 1); PORT_2 = ~PORT_1;
    n_max = 24;n=1;break;
case 5: PORT_1=0b0000000000000001;
    PORT_2 = 0;
    n_max = 32;
    break;}
I2C_Tx_2_integers(PORT_1, PORT_2);}



/***********Inc_Display() subroutine*****************************/
void Inc_Display(char mode) {

switch (mode){
case 0: PORT_1= PORT_1 << 1;  PORT_2 = PORT_2 >> 1; break;
case 1: PORT_1 = PORT_1 << 1; PORT_2 = PORT_1; break;

case 2: 
if(n<8){PORT_1 |= 1 << (2*n); PORT_2 |= 0x8000 >> (2*n);}
else {PORT_1 = PORT_1 << 2; PORT_2 = PORT_2 >> 2;}break;

case 3: if(n < 5){PORT_1 |= PORT_1 << 1; PORT_2 |= PORT_2 >> 1;}
    else{PORT_1= PORT_1 << 1;PORT_2 = PORT_2 >> 1;}break;
  
case 4: 
    if ((n <= 7)){PORT_1 = ((0xFFFF >> (n+1)) ^ (0xFFFF << (n+1)));PORT_2 = ~PORT_1;}
    if(n==8){PORT_1= PORT_1<<1; PORT_2 = 1;}
    if(n > 8){PORT_1 = PORT_1<< 1;PORT_2 |= (PORT_2 << 1);}break;

case 5: if (n<16){PORT_1= PORT_1 << 1;}
    if (n>=15) {PORT_2 = ((~(~PORT_2 >> 1)) & (0xFFFF >> (n-15)));}break;}

I2C_Tx_2_integers(PORT_1, PORT_2); n++; 
if (n == n_max) {n=0; p += 1; p = p%8; initialise_display();}}
