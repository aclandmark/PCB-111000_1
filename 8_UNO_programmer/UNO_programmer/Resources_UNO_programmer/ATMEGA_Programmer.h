
void Prog_EEPROM(void);

void Program_Flash_Hex (void);
void Program_Flash_Text(void);
void Verify_Flash_Hex(void);
void Verify_Flash_Text(void);
void upload_hex(void);
void upload_text(void);

void set_up_target_parameters(void);



void Clock_period(void){for(int p = 0; p<= 3; p++){asm("nop");}}

#define PGClock_L 
#define PGClock_H Clock_period();

#define T0_delay_10ms 5,183
#define T0_delay_5ms 5,220
#define T0_delay_2ms 4,195
#define T0_delay_20ms 5,100
#define T0_delay_33ms 5,0
#define T1_delay_250mS 5,0xF85E
#define T1_delay_500mS 5,0xF0BD
#define T1_delay_1sec 5,0xE17B


char Suffix;													//Either P or space (i.e. 328P or 328 ).
signed int EE_top;												//Limits EEPROM space available for strings and data		


signed int PageSZ;												//Size of a page of flash
signed int PAmask;												//Used to obtain the flash page address from the hex address
signed int FlashSZ;												//Amount of flash memory supplied on target device
int EE_size;													///EEPROM size
unsigned char target_type_M, target_type, target_type_P;
unsigned char Fuse_Ex, Fuse_H, Fuse_L, Lock;




int text_start;												//First 5 addresses reserved to define string/data EEPROM partitions		
unsigned char pcb_type = 0;									//1 for UNO and 2 for PCB_A
	
int  cmd_counter;											//Counts commands as they are downloaded from the PC
int prog_counter;											//Counts commands burned to flash
signed int  read_ops=0;										//Total number of commands read from flash
volatile int counter;										//Counts characters in a record as they are downloded from the PC
volatile int char_count;									//The number of askii character in a single record
volatile unsigned char Count_down;							//Counts commands as record is programmed
volatile int   tempInt1, tempInt2;							//Used to assemble commands and addresses as the are downloaded
int store[130];												//Used to store commands and address ready for the programmer
volatile unsigned char w_pointer,r_pointer;					//Read/write pointers to "store" to which hex file is saved
unsigned int Hex_cmd;										//Command read from flash during verification

unsigned char cmd_pin, resp_pin, clock_pin, reset_pin;		//Used to define the programming pins

unsigned int target;										//Target device type
int Hex_address;											//Address read from the hex file
int HW_address;												//Hard ware address (usually tracks Hex_address)
signed int page_address;									//Address of first location on a page of flash 
volatile int write_address;									//Address on page_buffer to which next command will be written

signed char short_record;									//Record  containing less that eight 16 bit commands
signed char page_offset;									//Address of first location on page buffer to be used
signed char space_on_page;									//Keeps a track of the space remaining on a page buffer
unsigned char Flash_flag;									//Indicates that the page buffer contains commands

signed char record_length;									//Num commands one one line of hex file (i.e. on one record)
signed char record_length_old;								//If record length changes, length of the previous one is important
signed char orphan;											//Indicates that the contents of a record span two flash pages
signed char section_break;									//Set to 1 if at least one page of flash memory will be unused.
signed char page_break;										//Page only partialy filled before programming next one starts
volatile signed char line_offset;							//LSB of address of first command in record (usually zero)
unsigned int prog_led_control;								//Used to control Leds as hex file is downloaded


unsigned char cal_factor=0; 								//Either default or user supplied




volatile char endoftext, text_started;
volatile char Rx_askii_char_old;                             //Required to check for a \r\n string
int address_in_flash;                                       //Address in flash at which text data is to be programmed
unsigned char  op_code;






/************************************************************************************************************************************/
#define setup_328_HW \
setup_watchdog;\
cal_Atmega328;\
ADMUX |= (1 << REFS0);\
Set_LED_ports;\
LEDs_off;\
Unused_I_O;\
timer_T0_sub(T0_delay_5ms);\
Define_programmining_pins;\
Reset_L;\
Config_Xtal_port;\
USART_init(0,25);



/************************************************************************************************************************************/
#define wdr()  __asm__ __volatile__("wdr")



#define setup_watchdog \
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;


#define SW_reset {wdt_enable(WDTO_30MS);while(1);}


/************************************************************************************************************************************/
#define cal_Atmega328 \
eeprom_write_byte((uint8_t*)0x3FD, OSCCAL);\
if ((eeprom_read_byte((uint8_t*)0x3FE) > 0x0F)\
&&  (eeprom_read_byte((uint8_t*)0x3FE) < 0xF0) && (eeprom_read_byte((uint8_t*)0x3FE)\
== eeprom_read_byte((uint8_t*)0x3FF))) {OSCCAL = eeprom_read_byte((uint8_t*)0x3FE); cal_factor=1;}\
else cal_factor=0;



/************************************************************************************************************************************/
#define Initialise_variables_for_programming_flash \
prog_counter=0;  prog_led_control = 0; cmd_counter = 0; record_length_old=0;\
Flash_flag = 0;  HW_address = 0;  section_break = 0; orphan = 0;\
w_pointer = 0; r_pointer = 0; short_record=0;\
counter = 1;



/************************************************************************************************************************************/
#define Set_LED_ports 	DDRB = (1 << DDB0) | (1 << DDB1);
#define LEDs_on  PORTB |= (1 << PB0)|(1 << PB1);
#define LEDs_off  PORTB &= (~((1 << PB0)|(1 << PB1)));



/*************************Atmega programming commands (see data sheet)******************************/


#define inc_w_pointer \
w_pointer++;\
w_pointer = w_pointer & 0x1F;


#define inc_r_pointer \
r_pointer++;\
r_pointer = r_pointer & 0b00011111;




#define Prog_enable 0xAC530000
#define Prog_enable_h 0xAC53

#define signature_bit_1 0x30000000
#define signature_bit_1_h 0x3000

#define signature_bit_2 0x30000100
#define signature_bit_2_h 0x3000
#define signature_bit_2_l 0x0100

#define signature_bit_3 0x30000200
#define signature_bit_3_h 0x3000
#define signature_bit_3_l 0x0200

#define read_fuse_bits 0x50000000
#define read_fuse_bits_h 0x5000

#define read_fuse_bits_H 0x58080000
#define read_fuse_bits_H_h 0x5808

#define read_lock_bits 0x58000000
#define read_lock_bits_h 0x5800

#define read_extended_fuse_bits 0x50080000
#define read_extended_fuse_bits_h 0x5008

#define write_lock_bits 0xACE00000
#define write_lock_bits_h 0xACE0

#define write_fuse_bits_H 0xACA80000
#define write_fuse_bits_H_h 0xACA8

#define write_fuse_bits 0xACA00000
#define write_fuse_bits_h 0xACA0

#define write_extended_fuse_bits 0xACA40000
#define write_extended_fuse_bits_h 0xACA4

#define Chip_erase 0xAC800000
#define Chip_erase_h 0xAC80



/************************************************************************************************************************************/
#define Define_programmining_pins  cmd_pin =  0x08; resp_pin = 0x10; clock_pin =  0x20; reset_pin = 0x08; DDRB |= 0x28; DDRC |= 0x08;
#define PGD_cmd_H PORTB |= cmd_pin
#define PGD_cmd_L PORTB &= ~(cmd_pin)
#define PGD_resp_H PINB & resp_pin
#define PGC_L PORTB &= ~(clock_pin)
#define PGC_H PORTB |= clock_pin
#define Reset_L PORTC &= ~(reset_pin)
#define Reset_H PORTC |= reset_pin



/************************************************************************************************************************************/
#define Atmel_powerup \
timer_T0_sub(T0_delay_2ms);\
Reset_L;\
PGC_L;\
timer_T0_sub(T0_delay_2ms);\
Reset_H;\
timer_T0_sub(T0_delay_2ms);\
Reset_L;\
timer_T0_sub(T0_delay_20ms);


/************************************************************************************************************************************/
#define Atmel_powerup_and_target_detect \
Atmel_powerup;\
while(1){if((Atmel_config(Prog_enable_h, 0)==0x53) && (Atmel_config(signature_bit_1_h, 0) == 0x1E))break;\
else {sendString("Target_not_detected\r\n"); wdt_enable(WDTO_60MS);while(1);}}\
\
Suffix = '?';\
if (Atmel_config(signature_bit_2_h, signature_bit_2_l)  == target_type_M)\
{if(Atmel_config(signature_bit_3_h, signature_bit_3_l)  == target_type)Suffix = ' ';\
if(Atmel_config(signature_bit_3_h, signature_bit_3_l)  == target_type_P)Suffix = 'P';}\
if(Suffix == '?'){sendString("Target_not_recognised"); sendString("\r\n"); wdt_enable(WDTO_60MS);while(1);}



/************************************************************************************************************************************/
#define send_as_askii \
if((((Hex_cmd>>8)<0x20)||((Hex_cmd>>8)>0x7E))&&(((Hex_cmd & 0x00FF)<0x20)||((Hex_cmd & 0x00FF)>0x7E)))sendHex(16, Hex_cmd);\
else{if(((Hex_cmd>>8)>=0x20) && ((Hex_cmd>>8)<=0x7E) && ((Hex_cmd & 0x00FF)>=0x20) && ((Hex_cmd & 0x00FF)<=0x7E))\
{sendChar(Hex_cmd>>8); sendChar(Hex_cmd & 0x00FF);}\
else { if(((Hex_cmd>>8)>=0x20) && ((Hex_cmd>>8)<=0x7E)){sendChar(Hex_cmd>>8); sendCharasASKI(16, (Hex_cmd & 0x00FF));}\
if(((Hex_cmd & 0x00FF)>=0x20) && ((Hex_cmd & 0x00FF)<=0x7E)){sendCharasASKI(16, (Hex_cmd>>8)); sendChar(Hex_cmd & 0x00FF);}}}



/************************************************************************************************************************************/
#define Unused_I_O \
MCUCR &= (~(1 << PUD));\
\
DDRB &= (~(1 << PB2));\
PORTB |= ((1 << PB2));\
\
DDRC &= (~((1 << PC0)|(1 << PC1)|(1 << PC2)|(1 << PC4)|(1 << PC5)));\
PORTC |= ((1 << PC0)|(1 << PC1)|(1 << PC2)|(1 << PC4)|(1 << PC5));\
\
DDRD &= (~((1 << PD2)|(1 << PD3)|(1 << PD4)|(1 << PD5)|(1 << PD6)|(1 << PD7)));\
\
PORTD |= ((1 << PD2)|(1 << PD3)|(1 << PD4)|(1 << PD5)|(1 << PD6)|(1 << PD7));



#define Config_Xtal_port \
ASSR = (1 << AS2);	

/************************************************************************************************************************************/


/**************************************************************************************************************************************/
#define Prog_config_bytes \
Atmel_config(write_extended_fuse_bits_h,Fuse_Ex );\
Atmel_config(write_fuse_bits_H_h,Fuse_H );\
Atmel_config(write_fuse_bits_h,Fuse_L );\
Atmel_config(write_lock_bits_h,Lock );\


#define Verify_config_bytes \
sendString("Config bytes: Fuses extended,\
high, low and lock\t");\
sendHex(16, Atmel_config(read_extended_fuse_bits_h, 0));\
sendHex(16, Atmel_config(read_fuse_bits_H_h,0));\
sendHex(16, Atmel_config(read_fuse_bits_h, 0));\
sendHex(16, Atmel_config(read_lock_bits_h, 0));

