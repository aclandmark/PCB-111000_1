/*

sw1 is PIND2	The left hand switch
sw2 is PIND7	The middle switch
sw3 is PINB2	The right hand switch


Switches sw1 and sw2 generate a call to ISR(PCINT2_vect) 
Switch sw3 generates a call to ISR(PCINT0_vect)

If PCI is required: Set_up and enable PCI
PCI interrupt call can be delayed. Execute "hold_PCI" then "restore PCI" later on.
To temporarily prevent PCI from generating interrupts, execute "dissable pci".

Commands "clear_PCI_on_sw1_and_sw2" and "clear_PCI_on_sw3" are provided to clear
spurious interrupt calls generated by switch bounce.
*/

/***************************************************************************************/

volatile char PCMSK0_mem, PCMSK2_mem;			
char PCMSK0_backup, PCMSK2_backup, float_display_mode;

/***************************************************************************************/




#define Init_display_for_pci_data_entry \
clear_digits;\
digits[0] = '0';\
I2C_Tx_8_byte_array(digits);

/******************************************************************/
#define initialise_OS_chip_for_arithmetic \
clear_digits;\
I2C_Tx_8_byte_array(digits);\
digits[0] = 'A';\
I2C_Tx_2URNs_from_IO(digits);\
Timer_T0_10mS_delay_x_m(5);\
digits[0] = '0';\
I2C_Tx_2URNs_from_IO(digits);


/******************************************************************/

#define switch_1_down	((PIND & 0x04)^0x04)
#define switch_1_up		(PIND & 0x04)
#define switch_2_down	((PIND & 0x80)^0x80)
#define switch_2_up		(PIND & 0x80)
#define switch_3_down	((PINB & 0x04)^0x04)
#define switch_3_up		(PINB & 0x04)

/******************************************************************/
#define restore_PCI_on_sw3  set_up_PCI_on_sw3;
#define restore_PCI_on_sw1_and_sw2  set_up_PCI_on_sw1_and_sw2;
#define restore_PCI  set_up_PCI;

/******************************************************************/
#define setup_and_enable_PCI  config_sw3_for_PCI; config_sw1_and_sw2_for_PCI; 

/******************************************************************/
#define config_sw3_for_PCI  set_up_PCI_on_sw3; enable_pci_on_sw3;
#define config_sw1_and_sw2_for_PCI  set_up_PCI_on_sw1_and_sw2; enable_pci_on_sw1_and_sw2;

/******************************************************************/

#define set_up_PCI_on_sw3  			PCICR |= (1 << PCIE0);
#define set_up_PCI_on_sw1_and_sw2  	PCICR |= (1 << PCIE2);
#define hold_PCI_on_sw3 			PCICR &= (~(1 << PCIE0));
#define hold_PCI_on_sw1_and_sw2  	PCICR &= (~(1 << PCIE2));
#define PCI_on_sw3_on_hold			(!(PCICR & (1 << PCIE0)))
#define PCI_on_sw1_and_sw2_on_hold	(!(PCICR & (1 << PCIE2)))

/******************************************************************/
#define set_up_PCI \
PCICR |= ((1 << PCIE0) | (1 << PCIE2));

/******************************************************************/
#define hold_PCI \
PCICR &= (~((1 << PCIE0) | (1 << PCIE2)));

/******************************************************************/
#define PCIset_up ((PCICR & (1 << PCIE0))  || (PCICR & (1 << PCIE2)))
#define PCIenabled ((pci_on_sw1_and_sw2_enabled) || (pci_on_sw3_enabled))


/******************************************************************/
#define enable_pci_on_sw1_and_sw2  PCMSK2 |= (1 << PCINT18) | (1 << PCINT23);
#define disable_pci_on_sw1_and_sw2  PCMSK2 &= (~((1 << PCINT18) | (1 << PCINT23)));
#define pci_on_sw1_and_sw2_enabled (PCMSK2 & 0x84) == 0x84
#define pci_on_sw1_and_sw2_disabled (PCMSK2 & 0x84) == 0x00

#define enable_pci_on_sw1  	PCMSK2 |= (1 << PCINT18);
#define disable_pci_on_sw1  PCMSK2 &= (~(1 << PCINT18));
#define enable_pci_on_sw2  	PCMSK2 |= (1 << PCINT23);
#define disable_pci_on_sw2  PCMSK2 &= (~(1 << PCINT23));

/******************************************************************/
#define enable_pci_on_sw3  PCMSK0 |= (1 << PCINT2);
#define disable_pci_on_sw3  PCMSK0 &= (~(1 << PCINT2));
#define pci_on_sw3_enabled (PCMSK0 & 0x04) == 0x04
#define pci_on_sw3_disabled (PCMSK0 & 0x04) == 0x00

/******************************************************************/
#define enable_pci  PCMSK0 |= (1 << PCINT2);    PCMSK2 |= (1 << PCINT18) | (1 << PCINT23);
#define disable_pci PCMSK0 &= (~(1 << PCINT2)); PCMSK2 &= (~((1 << PCINT18) | (1 << PCINT23)));

/******************************************************************/
#define save_pci_enabled_state   PCMSK0_mem = PCMSK0; PCMSK2_mem = PCMSK2;
#define restore_pci_enabled_state   PCMSK0 = PCMSK0_mem; PCMSK2 = PCMSK2_mem;


/******************************************************************/
#define clear_PCI_on_sw1_and_sw2  	PCIFR |= (1<< PCIF2);
#define clear_PCI_on_sw3 		 	PCIFR |= (1<< PCIF0);



/******************************************************************/
#define I2C_Tx_float_display_control \
{\
PCMSK0_backup= PCMSK0;\
PCMSK2_backup= PCMSK2;\
float_display_mode = '0';\
if (PCIenabled){disable_pci_on_sw3;disable_pci_on_sw1_and_sw2;}\
while(1){\
if(switch_3_down)float_display_mode = '1'; else float_display_mode = '0';\
if((switch_1_down)||(switch_2_down))float_display_mode = '2';\
waiting_for_I2C_master;\
send_byte_with_Nack(float_display_mode);\
clear_I2C_interrupt;\
if(float_display_mode == '2')break;}\
PCMSK0 = PCMSK0_backup;\
PCMSK2 = PCMSK2_backup;}
