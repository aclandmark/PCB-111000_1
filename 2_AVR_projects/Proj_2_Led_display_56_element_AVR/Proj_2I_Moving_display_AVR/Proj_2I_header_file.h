

#include "../../Resources_V15/Header_files/Basic_HW_setup.h"
#include "../../Resources_V15/Subroutines/Basic_HW_plus_Timer.c"
#include "../../Resources_V15/I2C_Subroutines/I2C_subroutines_1.c"
#include "../../Resources_V15/Subroutines/Basic_PC_comms.c"
#include "../../Resources_V15/I2C_Subroutines/I2C_slave_Rx_Tx.c"

#define zero "abcdef"					//chars a,b,c,d,e,f stored in array named "zero"
#define one "bc"
#define two "abged"
#define three "abcdg"
#define four "fgbc"
#define five "afgcd"
#define six "fegdc"
#define seven "abc"
#define eight "abcdefg"
#define nine "abcfg"
#define null "\0"

void display_num_string (const char*, int);
char digit_array_to_display(char*, char);

#define rotate_display_left  for(int m = 16; m; m--)digit_array[m] = digit_array[m-1];\
digit_array[0] = digit_array[16];
