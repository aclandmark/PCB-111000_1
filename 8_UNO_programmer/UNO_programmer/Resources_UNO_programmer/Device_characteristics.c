


void set_up_target_parameters(void){
if(Target_type == 0x95)									//Atmega 328/P
{
PageSZ = 0x40;
PAmask = 0x3FC0;
FlashSZ = 0x4000;
EE_size = 0x400;
//Target_type = 0x95;
Target_type_M = 0x14;
Target_type_P = 0x0F;
Fuse_Ex = 0xFF;
Fuse_H = 0xD7;
Fuse_L = 0xE2;
Lock = 0xFF;}


if(Target_type == 0x94)									//Atmega 168/P
{
PageSZ = 0x40;			
PAmask = 0x1FC0;		
FlashSZ = 0x2000;		
EE_size = 0x200;		
//Target_type = 0x94;	
Target_type_M = 0x6;		
Target_type_P = 0x0B;	
Fuse_Ex = 0xFF;
Fuse_H = 0xD7;
Fuse_L = 0xE2;
Lock = 0xFF;}




}