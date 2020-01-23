



#include "Proj_1G_header_file.h"

char Dimmer_control;

int main (void){
unsigned int PORT_1;
setup_HW;

UCSR0B |= (1 << RXCIE0); 
sei();
Dimmer_control = 1;

while(1){
PORT_1=1;
for(int m = 1; m < 17; m++){
cli();I2C_Tx_2_integers(PORT_1, PORT_1);sei();
Timer_T0_10mS_delay_x_m(6);
PORT_1 = PORT_1 << 1;}}}



ISR(USART_RX_vect){
receiveChar();
I2C_Tx(1, 'Q', &Dimmer_control);}	




/*Local version of subroutine "I2C_Tx()"

void I2C_Tx_local(char num_bytes, char mode, char* s){
waiting_for_I2C_master;											//Turn on I2C slave and await call from master
send_byte_with_Ack(num_bytes);									//send data byte, request acknowledgement
send_byte_with_Ack(mode);
for (int m = 0;  m < num_bytes; m++){
if (m==num_bytes-1){send_byte_with_Nack(s[m]);}					//Last byte, no ackowledgement needed
else {send_byte_with_Ack(s[m]);}}
TWCR = (1 << TWINT);}	*/										//Clear interrupt and close I2C slave
