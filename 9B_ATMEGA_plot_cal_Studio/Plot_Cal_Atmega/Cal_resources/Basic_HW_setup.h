
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>


void save_cal_values(unsigned char);
void printout_cal_values(void);
void initialise_timers_for_cal_error(void);
void start_timers_for_cal_error(void);
long compute_error(char, char, char);
void Plot_cal (void);
void set_up_target_parameters(void);



unsigned char OSCCAL_WV;
unsigned char OSCCAL_UV;
unsigned char OSCCAL_DV;
int EE_size;
int FlashSZ;
int EEP_MAX = 0x2000;

volatile long error_SUM;
volatile int EA_counter;

volatile char cal_mode; 			//Defines number of averages used when measuring osccal_error	
volatile char T1_OVF;

#define delay_2ms 4,195
#define T0_delay_10ms 5,178
#define T1_delay_1sec 5,0xE17D


/*****************************************************************************/
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>



/*****************************************************************************/
#define setup_HW_basic \
setup_watchdog;\
USART_init(0,25);\
nop_delay(10);\
ADMUX |= (1 << REFS0);\
Timer_T0_sub(T0_delay_10ms);\
initialise_IO;\
set_device_type_and_memory_size;\
cal_device;

/*****************************************************************************/
#define SW_reset  {wdt_enable(WDTO_30MS);while(1);}
	
	
#define setup_watchdog \
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;

#define wdr()  __asm__ __volatile__("wdr")



/*****************************************************************************/
#define initialise_IO \
MCUCR &= (~(1 << PUD));\
DDRB = 0;\
DDRC = 0;\
DDRD = 0;\
PORTB = 0xFF;\
PORTC = 0xFF;\
PORTD = 0xFF;



/*****************************************************************************/
#define Get_ready_to_calibrate  \
TIMSK2 |= (1 << TOIE2);\
TIMSK1 |= (1 << TOIE1);\
initialise_timers_for_cal_error();\
start_timers_for_cal_error();



/*****************************************************************************/
#define close_calibration \
initialise_timers_for_cal_error();\
TIMSK2 &= (~(1 << TOIE2));\
TIMSK1 &= (~(1 << TOIE1));



/*****************************************************************************/
#define calibrate_without_sign_plus_warm_up_time \
cal_mode = 5;\
cal_error = compute_error(0,cal_mode,0);\
cal_error = compute_error(0,cal_mode,0);\
cal_error = compute_error(0,cal_mode,0);




/**********************************************************************************/
#define cal_device \
eeprom_write_byte((uint8_t*)(EE_size - 3), OSCCAL);\
if ((eeprom_read_byte((uint8_t*)(EE_size - 2)) > 0x0F)\
&&  (eeprom_read_byte((uint8_t*)(EE_size - 2)) < 0xF0) && (eeprom_read_byte((uint8_t*)(EE_size - 2))\
== eeprom_read_byte((uint8_t*)(EE_size - 1)))) OSCCAL = eeprom_read_byte((uint8_t*)(EE_size - 2));



/**********************************************************************************/
#define User_prompt \
while(1){\
do{sendString("R?    ");}	 while((isCharavailable(250) == 0));\
User_response = receiveChar();\
if((User_response == 'R') || (User_response == 'r'))break;} sendString("\r\n");



/*****************************************************************************/
const char * Device_92 = "48/P";
const char * Device_93 = "88/P";
const char * Device_94 = "168/P";
const char * Device_95 = "328/P";
const char * Device_96 = "644";

const char * Device_type[6];
int device_ptr;




/*****************************************************************************/
#define Set_device_signatures \
Device_type[1] = Device_92;\
Device_type[2] = Device_93;\
Device_type[3] = Device_94;\
Device_type[4] = Device_95;\
Device_type[5] = Device_96;



/*****************************************************************************/
#define set_device_type_and_memory_size \
Set_device_signatures;\
switch(eeprom_read_byte((uint8_t*)(EEP_MAX - 4))){\
	case 0x92: FlashSZ = 0x800;  EE_size = 0x100; device_ptr = 1; break;\
	case 0x93: FlashSZ = 0x1000; EE_size = 0x200; device_ptr = 2; break;\
	case 0x94: FlashSZ = 0x2000; EE_size = 0x200; device_ptr = 3; break;\
	case 0x95: FlashSZ = 0x4000; EE_size = 0x400; device_ptr = 4; break;\
	case 0x96: FlashSZ = 0x8000; EE_size = 0x800; device_ptr = 5; break;}
	
	