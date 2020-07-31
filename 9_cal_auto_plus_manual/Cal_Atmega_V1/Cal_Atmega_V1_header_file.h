

#include "../Cal_resources/Basic_HW_setup.h"
#include "../Cal_resources/Basic_HW_plus_Timer.c"
#include "../Cal_resources/Basic_PC_comms.c"



#define Get_ready_to_calibrate; \
TIMSK2 |= (1 << TOIE2);\
TIMSK1 |= (1 << TOIE1);\
initialise_timers_for_cal_error();\
start_timers_for_cal_error();


#define close_calibration; \
initialise_timers_for_cal_error();\
TIMSK2 &= (~(1 << TOIE2));\
TIMSK1 &= (~(1 << TOIE1));

#define calibrate_without_sign_plus_warm_up_time; \
cal_mode = 5;\
cal_error = compute_error(0,cal_mode,0);\
cal_error = compute_error(0,cal_mode,0);\
cal_error = compute_error(0,cal_mode,0);



const char * Device_95 = "328/P";
const char * Device_94 = "168/P";
const char * Device_93 = "88/P";
const char * Device_92 = "48/P";

const char * Device_ptr[4];
int Device;
