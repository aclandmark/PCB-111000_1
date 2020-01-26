

void I2C_Rx_get_version(char);
void PCB_test(void);
void scan_328_cal_factors(void);



/************************************************************************/
void I2C_Rx_get_version(char str_type){
char num_bytes=1; char mode='P';
char s[2];

s[0]= str_type; s[1]=0;
I2C_Tx(num_bytes,mode, s);
waiting_for_I2C_master;
num_bytes = (receive_byte_with_Ack() - '0') * 10;
num_bytes += (receive_byte_with_Ack() - '0');
for (int m = 0; m < num_bytes; m++){
if (m ==(num_bytes-1)){Char_to_PC(receive_byte_with_Nack());}
else {Char_to_PC(receive_byte_with_Ack());}}
TWCR = (1 << TWINT);}



/************************************************************************/
void PCB_test(void){					
char num_bytes=0;		 
char mode = 'O';

waiting_for_I2C_master;		
send_byte_with_Ack(num_bytes);
send_byte_with_Nack(mode);
TWCR = (1 << TWINT);}



/************************************************************************/
void scan_328_cal_factors(void){					
char num_bytes=0;		 
char mode = 'M';

waiting_for_I2C_master;		
send_byte_with_Ack(num_bytes);
send_byte_with_Nack(mode);
TWCR = (1 << TWINT);}


