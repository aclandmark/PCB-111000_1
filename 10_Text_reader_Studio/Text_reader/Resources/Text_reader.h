
char Char_from_flash(int);
void Determine_device_type(void);
void set_up_target_parameters(void);
unsigned char string_counter(int);
void print_string_num(int, int);


/***************************************************************************/
volatile char Prog_mem_address_H, Prog_mem_address_L;
volatile char Flash_readout;
int char_counter;
int FlashSZ;
int EE_size;
char User_response;
int EEP_MAX = 0x2000;





/**********************************************************************************/
#define setup_HW \
config_WDT;\
Unused_I_O;\
ADMUX |= (1 << REFS0);\
USART_init(0,25);\
set_device_type_and_memory_size;\
cal_device;


/**********************************************************************************/
#define wdr()  __asm__ __volatile__("wdr")


#define config_WDT \
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;



/**********************************************************************************/
#define cal_device \
eeprom_write_byte((uint8_t*)(EE_size - 3), OSCCAL);\
if ((eeprom_read_byte((uint8_t*)(EE_size - 2)) > 0x0F)\
&&  (eeprom_read_byte((uint8_t*)(EE_size - 2)) < 0xF0) && (eeprom_read_byte((uint8_t*)(EE_size - 2))\
== eeprom_read_byte((uint8_t*)(EE_size - 1)))) OSCCAL = eeprom_read_byte((uint8_t*)(EE_size - 2));


/**********************************************************************************/
#define Unused_I_O \
MCUCR &= (~(1 << PUD));\
DDRB = 0;\
DDRC = 0;\
DDRD = 0;\
PORTB = 0xFF;\
PORTC = 0xFF;\
PORTD = 0xFF;



/**********************************************************************************/
#define User_prompt \
while(1){\
	do{sendString("R?    ");}	 while((isCharavailable(250) == 0));\
	User_response = receiveChar();\
if((User_response == 'R') || (User_response == 'r'))break;} sendString("\r\n");



/**********************************************************************************/
const char * Device_95 = "328/P";
const char * Device_94 = "168/P";
const char * Device_93 = "88/P";
const char * Device_92 = "48/P";

const char * Device_type[4];
int device_ptr;



/*****************************************************************************/
#define Set_device_signatures \
Device_type[0] = Device_95;\
Device_type[1] = Device_94;\
Device_type[2] = Device_93;\
Device_type[3] = Device_92;



/*****************************************************************************/
#define set_device_type_and_memory_size \
Set_device_signatures;\
switch(eeprom_read_byte((uint8_t*)(EEP_MAX - 4))){\
	case 0x95: FlashSZ = 0x4000; EE_size = 0x400; device_ptr = 0; break;\
	case 0x94: FlashSZ = 0x2000; EE_size = 0x200; device_ptr = 1; break;\
	case 0x93: FlashSZ = 0x1000; EE_size = 0x200; device_ptr = 2; break;\
	case 0x92: FlashSZ = 0x800;  EE_size = 0x100; device_ptr = 3; break;}
	
	
	
	
	
	
	
	
	
	