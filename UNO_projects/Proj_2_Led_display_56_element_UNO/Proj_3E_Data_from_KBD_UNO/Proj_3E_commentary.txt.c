
/*Proj_3E_Data_from_KBD to display
***********************************************************/


IT INTRODUCES


1.	Project subroutine "Num_from_KBD_Local()".

This subroutine acquires numerical keypresses and echos them to the display. Note that the display is shifted one place to the left every time that a new digit is entered.

This subroutine ends by calling calls "I2C_displayToNum" which reads the display, converts the result to a number and sends it to the UNO.

2.	Project subroutine "Num_to_PC()".	This converts a number to askii characters using a radix of either 10 or 16. The resulting string is then sent to the PC using subroutine  "NumericString_to_PC()". 

3.	Project subroutine I2C_Tx_long(Num_1);	This uses the mini-OS to display a long number

4.	Project subroutine "wait_for_return_key()".  	This traps symbols '\r' (return) and '\n' (new line) and ensures that if both are present they are combined into a single '\r'. 

5.	A simple method of trapping overflow that occurs with multiplication.

	