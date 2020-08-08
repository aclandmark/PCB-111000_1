

#include "../Cal_resources/Basic_HW_setup.h"
#include "../Cal_resources/Basic_HW_plus_Timer.c"
#include "../Cal_resources/Basic_PC_comms.c"
#include "../Cal_resources/Cal_subroutines.c"



const char *Device = "328";



#define set_device_type_and_memory_size \
Set_device_signatures;\
switch(waitforkeypress() - '0'){\
case 0: FlashSZ = 0x4000; EE_size = 0x400; device_ptr = 0; break;\
case 1: FlashSZ = 0x2000; EE_size = 0x200; device_ptr = 1; break;\
case 2: FlashSZ = 0x1000; EE_size = 0x200; device_ptr = 2; break;\
case 3: FlashSZ = 0x800;  EE_size = 0x100; device_ptr = 3; break;}\
sendString("\r\nCalibrating Atmega ");\
sendString (Device_type[device_ptr]);\
newline();			


