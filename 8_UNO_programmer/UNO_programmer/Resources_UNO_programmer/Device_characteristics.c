


void set_up_target_parameters(void){
if((Device[0] == '3') && (Device[1] == '2')
&& (Device[2] == '8'))
{
PageSZ = 0x40;
PAmask = 0x3FC0;
FlashSZ = 0x4000;
EE_size = 0x400;
target_type_M = 0x95;
target_type = 0x14;
target_type_P = 0x0F;
Fuse_Ex = 0xFF;
Fuse_H = 0xD7;
Fuse_L = 0xE2;
Lock = 0xFF;}


if((Device[0] == '1') && (Device[1] == '6')
&& (Device[2] == '8'))
{
PageSZ = 0x40;			
PAmask = 0x1FC0;		
FlashSZ = 0x2000;		
EE_size = 0x200;		
target_type_M = 0x94;	
target_type = 0x6;		
target_type_P = 0x0B;	
Fuse_Ex = 0xFF;
Fuse_H = 0xD7;
Fuse_L = 0xE2;
Lock = 0xFF;}




}