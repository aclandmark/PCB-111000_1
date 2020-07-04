
/**************Proj_1F_LED_display***************************/



#include "Proj_1G_header_file.h"



unsigned int n = 0;
unsigned int PORT_1,PORT_2;
volatile char T1_OVF;                           			//The timer intrerrup flag, set every time a timer interrupt is generated



int main (void){
setup_UNO_extra;

UCSR0B |= (1 << RXCIE0);									//Set up interrupt on key press (Sets USART receive interrupt enable flag)
T1_OVF = 0;
sei();														//Enable interrupts
T1_65ms_clock_tick();                          				 //Timer_1 interrupt generated every 65mS
initialise_display();

while(1)                                        			//Start a continuous loop
{
while(!(T1_OVF));											//Wait for timer interrupt
T1_OVF = 0; 												//Reset timer interrupt flag.
Inc_Display();                				    			//Increment the display 
}}                					

  

/*****************************************************************************************************************/
ISR(TIMER1_OVF_vect) 										//Timer interrupt Intrerrupt Service Routine
{T1_OVF = 1;}              									//Set timer interrupt flag



/*****************************************************************************************************************/
void initialise_display(void){

PORT_1=0b0000000000000001;									//Right hand most segment
PORT_2=0b1000000000000000; 									//Left hand modt segment
I2C_Tx_2_integers(PORT_1, PORT_2);}							//Illuminate display



/*****************************************************************************************************************/
void Inc_Display() {

if(n < 5){PORT_1 |= PORT_1 << 1; 
PORT_2 |= PORT_2 >> 1;}									//Establish a sequence
else{PORT_1= PORT_1 << 1;PORT_2 = PORT_2 >> 1;}			//Shift the sequence

I2C_Tx_2_integers(PORT_1, PORT_2);							//Send the sequence to the display 
n = (n+1)%16;												//Increment "n" from 0 to 15 and then repeat		
if(!(n))													//If n is zero
{initialise_display();}}									//re-initialise the display										


/*****************************************************************************************************************/
ISR(USART_RX_vect){
char Dimmer_control_local;									//This routine provides memory for "Dimmer_control_local"
Dimmer_control_local = (receiveChar()-'0')%4;				//and passes its address to "I2C_Tx"
I2C_Tx_local(1, 'Q', &Dimmer_control_local);}					//The "&" indicates that an address is being passed to "I2C_TX()"					  					




/*****************************************************************************************************************/

void I2C_Tx_local(char num_bytes, char mode, char* s){		//Subroutine provides memory for "num_bytes" and "mode" but not "s"
waiting_for_I2C_master;                                   	//Turn on I2C slave and await call from master
send_byte_with_Ack(num_bytes);  							//Inform master of payload size and request acknowledgement                        
send_byte_with_Ack(mode);									//Inform master of mini-OS mode
for (int m = 0;  m < num_bytes; m++){						//Send payload (only one byte in this case, but usally more)
if (m==num_bytes-1){send_byte_with_Nack(s[m]);}           //Last byte, no ackowledgement needed
else {send_byte_with_Ack(s[m]);}}
TWCR = (1 << TWINT);}                                     //Clear interrupt and close I2C slave
