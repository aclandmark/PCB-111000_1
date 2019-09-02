# PCB-111000_1
This is a plug in version of PCB-111000 for use with the Arduino Uno pcb.

Project PCB-111000 (published in October 2018) was a pcb designed to introduce beginners to programming Atmega devices in C.
The PCB contained two Atmega devices, a USB bridge and an 8 digit seven segment display.
One of the Atmega devices hosted user projects, the other hosted a mini-OS and programmer.  The mini-OS drove the display.
The two IC's communicated over the I2C bus.

To simplify pcb assembly a new and simplified version, PCB-11000_1 is to be developed.
This contains the display and the Atmega which hosts the mini-OS and programmer.
The UNO board provides the USB bridge and Atmega device used to host user projects.
