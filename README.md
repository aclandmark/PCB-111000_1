# PCB-111000_1

/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-//-/-//-/-//-/-/-/-/-/-
PCB-111000_1 is largely obsolete



/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-//-/-//-/-//-/-/-/-/-/-


This is a plug in version of PCB-111000 for use with the Arduino Uno pcb.

Project PCB-111000 (published in October 2018) was a pcb designed to introduce beginners to programming Atmega devices in C.
The PCB contained two Atmega devices, a USB bridge and an 8 digit seven segment display.
One of the Atmega devices hosted user projects, the other hosted a mini-OS.  The mini-OS drove the display and included a programmer
that was used to download user projects to the pcb. The two IC's communicated over the I2C bus.

To simplify pcb assembly, a new and simplified version, PCB-11000_1 is to be developed.
This contains the display and an Atmega 328 which hosts the mini-OS, and is known as the OS device.
The UNO board provides the USB bridge and the user device, an Atmega 328 device used to host user projects.

Details of this project have been posted to AVR_freaks as PCB_111000_UNO parts 1,2 and 3.

Links to relevent AVRfreaks projects:


https://github.com/MicrochipTech/avrfreaks-projects/blob/9e6bc971411a2d69374012572b3b6144f2f3f7f2/projects/pcb111000uno-part-1-pcb-1110001-unobootloaderforhextextv6/README.md

https://github.com/MicrochipTech/avrfreaks-projects/blob/9e6bc971411a2d69374012572b3b6144f2f3f7f2/projects/pcb111000uno-part-2-assembly-and-programming-uno-and-pcba/README.md

https://github.com/MicrochipTech/avrfreaks-projects/blob/main/projects/pcb111000uno-part-3-selection-user-projects/README.md



