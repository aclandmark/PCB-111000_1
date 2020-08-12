
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>


void save_cal_values(unsigned char);
void printout_cal_values(void);
void initialise_timers_for_cal_error(void);
void start_timers_for_cal_error(void);
void Minimise_error(int, unsigned char*, unsigned char*, long*, unsigned char*, char);
long compute_error(char, char, char);

void Auto_cal (void);
void Manual_cal(void);



unsigned char OSCCAL_WV;
unsigned char OSCCAL_UV;
unsigned char OSCCAL_DV;
int EE_size;
int FlashSZ;

long buffer[45];
volatile int EA_counter, EA_buff_ptr;
volatile long error_SUM;



volatile char cal_mode; 			//Defines number of averages used when measuring osccal_error	
volatile char T1_OVF;


#define timer_T0_sub Timer_T0_sub
#define delay_2ms 4,195
#define delay_20ms 5,100

#define T0_delay_5ms 5,220
#define T0_delay_10ms 5,178
#define T0_delay_20ms 5,100

#define T1_delay_100ms 5,0xFCF2
#define T1_delay_1sec 5,0xE17D
#define delay_2us 1,254
#define delay_10ms 5,183

/*****************************************************************************/
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>


/*****************************************************************************/
#define SW_reset  {wdt_enable(WDTO_30MS);while(1);}


/*****************************************************************************/
#define setup_HW_basic \
setup_watchdog;\
USART_init(0,25);\
nop_delay(10);\
ADMUX |= (1 << REFS0);\
Timer_T0_10mS_delay_x_m(1);\
initialise_IO;



#define setup_watchdog \
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;

#define wdr()  __asm__ __volatile__("wdr")

#define Set_device_signatures \
Device_type[0] = Device_95;\
Device_type[1] = Device_94;\
Device_type[2] = Device_93;\
Device_type[3] = Device_92; 




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





