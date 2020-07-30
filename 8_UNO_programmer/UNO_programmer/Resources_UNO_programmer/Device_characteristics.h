

#define Device "328"
#define PageSZ 0x40 
#define PAmask 0x3FC0 
#define FlashSZ 0x4000
#define EE_size 0x400
#define target_type 0x14
#define target_type_M 0x95
#define  Fuse_Ex 0xFF
#define  Fuse_H 0xD7
#define  Fuse_L 0xE2
#define  Lock 0xFF







/*
#define Device "328P"
#define PageSZ 0x40 
#define PAmask 0x3FC0 
#define FlashSZ 0x4000
#define EE_size 0x400
#define target_type 0x0F
#define target_type_M 0x95
#define  Fuse_Ex FF
#define  Fuse_H D7
#define  Fuse_L E2
#define  Lock FF



#define Device "328"
#define PageSZ 0x40 
#define PAmask 0x3FC0 
#define FlashSZ 0x4000
#define EE_size 0x400
#define target_type 0x14
#define target_type_M 0x95
#define  Fuse_Ex 0xFF
#define  Fuse_H 0xD7
#define  Fuse_L 0xE2
#define  Lock 0xFF
*/




/*
Signature bytes

Byte address 			0000		0001		0002

ATmega48				0x1E		0x92		0x05
ATmega48P				0x1E		0x92		0x0A


Atmega88				0x1E		0x93		0x0A
Atmega88P				0x1E		0x93		0x0F

Atmega168				0x1E		0x94		0x06
Atmega168P				0x1E		0x94		0x0B


Atmega328				0x1E		0x95		0x14
Atmega328P				0x1E		0x95		0x0F







*/