
void Prog_EEPROM(void);
void Program_Flash (void);
void Verify_Flash(void);

void Clock_period(void){for(int p = 0; p<= 3; p++){asm("nop");}}

#define PGClock_L 
#define PGClock_H Clock_period();

#define T0_delay_10ms 5,183
#define T0_delay_5ms 5,220
#define T0_delay_2ms 4,195
#define T0_delay_20ms 5,100
#define T1_delay_250mS 5,0xF85F
#define T1_delay_500mS 5,0xF0BD
#define T1_delay_1sec 5,0xE17B



unsigned char cmd_pin, resp_pin, clock_pin, reset_pin;		//Used to define the programming pins
int  cmd_counter;											//Counts commands as they are downloaded from the PC
int prog_counter;											//Counts commands burned to flash
signed int  read_ops=0;										//Total number of commands read from flash
unsigned int Hex_cmd;										//Command read from flash during verification
signed int FlashSZ;											//Amount of flash memory supplied on target device

#define Version "\r\nPCB_bootloader_V5.0\r\n" 


/*************************Atmega programming commands (see data sheet)******************************/

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
#define Define_programmining_pins; cmd_pin =  0x08; resp_pin = 0x10; clock_pin =  0x20; reset_pin = 0x08; DDRB |= 0x28; DDRC |= 0x08;
#define PGD_cmd_H PORTB |= cmd_pin
#define PGD_cmd_L PORTB &= ~(cmd_pin)
#define PGD_resp_H PINB & resp_pin
#define PGC_L PORTB &= ~(clock_pin)
#define PGC_H PORTB |= clock_pin
#define Reset_L PORTC &= ~(reset_pin)
#define Reset_H PORTC |= reset_pin




/************************************************************************************************************************************/
#define send_as_askii;\
if((((Hex_cmd>>8)<0x20)||((Hex_cmd>>8)>0x7E))&&(((Hex_cmd & 0x00FF)<0x20)||((Hex_cmd & 0x00FF)>0x7E)))sendHex(16, Hex_cmd);\
else{if(((Hex_cmd>>8)>=0x20) && ((Hex_cmd>>8)<=0x7E) && ((Hex_cmd & 0x00FF)>=0x20) && ((Hex_cmd & 0x00FF)<=0x7E))\
{sendChar(Hex_cmd>>8); sendChar(Hex_cmd & 0x00FF);}\
else { if(((Hex_cmd>>8)>=0x20) && ((Hex_cmd>>8)<=0x7E)){sendChar(Hex_cmd>>8); sendCharasASKI(16, (Hex_cmd & 0x00FF));}\
if(((Hex_cmd & 0x00FF)>=0x20) && ((Hex_cmd & 0x00FF)<=0x7E)){sendCharasASKI(16, (Hex_cmd>>8)); sendChar(Hex_cmd & 0x00FF);}}}


/************************************************************************************************************************************/
#define Config_Xtal_port;\
ASSR = (1 << AS2);	

#define initialise_leds;\
DDRD |= (1 << PD7);

#define led_off;\
PORTD &= (~(1 << PD7));

#define led_on;\
PORTD |= (1 << PD7);

