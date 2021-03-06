
/**************Proj_3A_transmit_chars********************/
/*Upload the project and then open the serial monitor
At the user prompt press both R and r*/


#include "Proj_3A_header_file.h"

void UInt_to_PC (unsigned int);


int main (void){
unsigned char symbol;
char User_response;


setup_UNO;
User_prompt;                                          //Respond by pressing either 'R' or 'r'

switch(User_response){
case 'r':
newline();
    symbol = '!';                                   //number 33
    while(symbol <= '~'){                           //number 126
    Char_to_PC_local(symbol);                       //display symbol on pc screen     
    Timer_T0_10mS_delay_x_m(10);
    symbol++;                                     //go on to next symbol  
    }break; 
    Char_to_PC_local(163);
    
case 'R': 
symbol = '!';
    while(symbol <= '~'){
    Num_to_PC_local(symbol);                       //display number on the PC screen together      
    Char_to_PC_local(symbol);                      //with the corresponding symbol   
    Timer_T0_10mS_delay_x_m(10);
    symbol++; 
    if(!((symbol-'!')%8))newline();               //format the output
    else Char_to_PC_local('\t'); }     
    break;}                                       //End of switch block
    Num_to_PC_local(163);
    Char_to_PC_local(163);         
    newline();
    SW_reset;}     

/**************************************************************************************************/
void Num_to_PC_local(unsigned char x)             //convert a number to three chars
{unsigned char y; unsigned char z;                //Consider x = 115 (the symbol s)
y = x/100;                                        //y = x/100 = 1         
z = x%100;                                        //z = x%100 = 15
Char_to_PC_local(y + '0');                        //sends char  '1'
Char_to_PC_local(z/10 + '0');                     //sends char  15/10 +'0' = '1'
Char_to_PC_local(x%10 + '0');                     //sends Char  15%10 +'0' = '5'
Char_to_PC_local(' ');}

/**************************************************************************************************/
void Char_to_PC_local(unsigned char data)         //Local function only:  normally use "Char_to_PC()"
{while (!(UCSR0A & (1 << UDRE0)));                //Wait here until UDRE0 is set to 1       //
UDR0 = data;}                                     //Transfer data to UDR0 which will automatically be sent to the PC.
