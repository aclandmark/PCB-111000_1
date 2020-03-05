
/*
All these routines interact with the on-chip EEPROM
Reading/writing to/from a target device is done in programming mode
*/

void Char_to_EEPROM(int, char);
char Char_from_EEPROM(int);
void Text_to_EEPROM(int*, char);
char Text_from_EEPROM(int*);
void Read_on_chip_EEPROM(int);
void Read_Hello_world_string(void);

char PRN_8bit_GEN(void);

void Char_to_PC(char);
void newline(void);
void Num_to_PC(char, long);	
void send_byte_with_Ack(char);
void send_byte_with_Nack(char);
char receive_byte_with_Ack(void);
char receive_byte_with_Nack(void);


void Char_to_EEPROM(int location, char text){eeprom_write_byte((uint8_t*)(location), text);}
char Char_from_EEPROM(int location){return eeprom_read_byte((uint8_t*) location);}



/****************************************************/
void Text_to_EEPROM(int*w_a, char byte){							//on-chip EEPROM
eeprom_write_byte((uint8_t*)(*w_a),byte); 							//macro provided by winavr
*w_a = *w_a + 1;}



/****************************************************/
char Text_from_EEPROM(int*r_a){
return eeprom_read_byte((uint8_t*)((*r_a)++));}					//macro provided by winavr



/****************************************************/
void Read_on_chip_EEPROM(int EEPROM_address){
char temp_char;
while(1){
temp_char =  eeprom_read_byte((uint8_t*)(EEPROM_address++));   
if (temp_char != '\0') Char_to_PC(temp_char);
else {newline(); break;}}}



/****************************************************/
void Read_Hello_world_string(void){
char receive_byte, num_bytes=0;


waiting_for_I2C_master;											//Initiate comuninations: master requests data
send_byte_with_Ack(num_bytes);									//Slave sends payload size
send_byte_with_Nack('H');										//Slave sends mode
clear_I2C_interrupt;											//Complete transaction

waiting_for_I2C_master;											//Initiate comuninations:master to send string

do{receive_byte = receive_byte_with_Ack();						//print out string as characters are received
if(receive_byte)Char_to_PC(receive_byte);}
while(receive_byte);											//Detect '\0' character used to terminate a string
receive_byte = receive_byte_with_Nack();						//Detect second '\0' char
clear_I2C_interrupt;}											//Complete transaction




