

#define T0_delay_10ms 5,178

#define waiting_for_I2C_master \
TWCR = (1 << TWEA) | (1 << TWEN);\
while (!(TWCR & (1 << TWINT)));\
TWDR;


void Timer_T0_sub(char, unsigned char);
void I2C_Tx(char, char, char*);
void send_byte_with_Ack(char);
void send_byte_with_Nack(char);




void USART_init (unsigned char UBRROH_N, unsigned char UBRR0L_N ){
UCSR0B = 0;
UBRR0H = UBRROH_N;  									
UBRR0L = UBRR0L_N;  								
UCSR0A = (1 << U2X0);
UCSR0B = (1 << RXEN0) | (1<< TXEN0);
UCSR0C =  (1 << UCSZ00)| (1 << UCSZ01);}



/*********************************************************************/
char receiveChar(void)
{return UDR0;}



/*********************************************************************/
char isCharavailable (char m){int n = 0;
while (!(UCSR0A & (1 << RXC0))){n++;
if (n>8000) {m--;n = 0;}if (m == 0)return 0;}
return 1;}


/*************************************************************************************/
char decimal_digit (char data){
if (((data > '9') || (data < '0')) )return 0;
else return 1;}


/*********************************************************************/
void Timer_T0_10mS_delay_x_m(int m)
{for (int n = 0; n < m; n++){Timer_T0_sub(T0_delay_10ms);}}


/*********************************************************************/
void Timer_T0_sub(char Counter_speed, unsigned char Start_point){ 
TCNT0 = Start_point;
TCCR0B = Counter_speed;
while(!(TIFR0 & (1<<TOV0)));
TIFR0 |= (1<<TOV0); TCCR0B = 0;}



/************************************************************************/
void I2C_Tx_LED_dimmer(void){
char Dimmer_control = 0; 
int m = 0,n = 0;

while((PINB & 0x04)^0x04) 
{n++;
if (n>1200) {m+=1;n = 0;}}
if (m >= 50){Dimmer_control = 1;
I2C_Tx(1, 'Q', &Dimmer_control);}}


/************************************************************************************/
void I2C_Tx(char num_bytes, char mode, char s[]){
waiting_for_I2C_master;
send_byte_with_Ack(num_bytes);
send_byte_with_Ack(mode);
for (int m = 0;  m < num_bytes; m++){
if (m==num_bytes-1){send_byte_with_Nack(s[m]);}
else {send_byte_with_Ack(s[m]);}}
TWCR = (1 << TWINT);}


/************************************************************************************/
void send_byte_with_Ack(char byte){
TWDR = byte;													//Send payload size: Zero in this case
TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);				//clear interrupt and set Enable Acknowledge
while (!(TWCR & (1 << TWINT)));}



/************************************************************************************/
void send_byte_with_Nack(char byte){
TWDR = byte;													//Send payload size: Zero in this case
TWCR = (1 << TWINT) | (1 << TWEN);								//clear interrupt and set Enable Acknowledge
while (!(TWCR & (1 << TWINT)));}


