//Carl Agbemabiese, Alex Bowe, Akuma Akuma-Ukpo

#include <p32xxxx.h>
#include <stdio.h>
#include <stdlib.h>
#include <plib.h>


#define SYS_FREQ     80000000L

#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_2

// The speed control
#define DELAY 100
// The refresh rate of the segments
#define REFRESH_RATE 500   // ~1/100 secs


//Column pins
#define col4 PORTBbits.RB0
#define col3 PORTBbits.RB1
#define col2 PORTBbits.RB2
#define col1 PORTBbits.RB3
//Row pins
#define row4 LATBbits.LATB4
#define row3 LATBbits.LATB5
#define row2 LATBbits.LATB8
#define row1 LATBbits.LATB9

// 7 Segment Display pmod for using the TOP JA & JB jumpers
// Segments
#define SegA    LATEbits.LATE0
#define SegB    LATEbits.LATE1
#define SegC    LATEbits.LATE2
#define SegD    LATEbits.LATE3
#define SegE    LATGbits.LATG9
#define SegF    LATGbits.LATG8
#define SegG    LATGbits.LATG7
#define SegA1    LATGbits.LATG12
#define SegB1    LATGbits.LATG13
#define SegC1    LATGbits.LATG14
#define SegD1    LATGbits.LATG15
#define SegE1    LATDbits.LATD7
#define SegF1    LATDbits.LATD1
#define SegG1    LATDbits.LATD9
#define DispSel LATGbits.LATG6
#define DispSel1 LATCbits.LATC1

int n;
int press;
int resultarr[4];
int pressedkey;
int release;
int keyaction;
int digit2;
int digit3=16;
int digit4=16;
int digit5;
int digit5old;
int add1;
int add2;
int result;
int buttonLock;
int i;
int sum;
int sum2;
int totaladd;
int totalsub;
int determinant;
int temp;


unsigned int displayssd2_l =16; // The initial displayed result
unsigned int displayssd2_r = 0;
unsigned int displayssd1_l = 16;
unsigned int displayssd1_r = 16;

//Subroutine, add yours for the project
void displayDigit(unsigned char, unsigned char); // Display one digit among two digits
void displayDigit2(unsigned char, unsigned char); // Display one digit among two digits
void lag_a(unsigned char, unsigned char); // Frequency control function
void lag_b(unsigned char display_digit, unsigned char result);

enum Mode {ll,rr};
enum Mode dir=ll;
// look-up table for the numbers
unsigned char number[]={
    0b0111111,   //0
    0b0000110,   //1
    0b1011011,   //2
    0b1001111,   //3
    0b1100110,   //4
    0b1101101,   //5
    0b1111101,   //6
    0b0000111,   //7
    0b1111111,   //8
    0b1101111,   //9
    0b1110111,   //A
    0b1111100,   //b
    0b0111001,   //C
    0b1011110,   //d
    0b1111001,   //E
    0b1110001,   //F
    0b0000000   //clear
};

void displayDigit (unsigned char display_digit, unsigned char result){
    DispSel = display_digit;
    SegA    = result & 1;
    SegB    = (result >> 1) & 1;
    SegC    = (result >> 2) & 1;
    SegD    = (result >> 3) & 1;
    SegE    = (result >> 4) & 1;
    SegF    = (result >> 5) & 1;
    SegG    = (result >> 6) & 1;
}

void displayDigit2 (unsigned char display_digit, unsigned char result){

    DispSel1 = display_digit;
    SegA1    = result & 1;
    SegB1    = (result >> 1) & 1;
    SegC1    = (result >> 2) & 1;
    SegD1    = (result >> 3) & 1;
    SegE1    = (result >> 4) & 1;
    SegF1    = (result >> 5) & 1;
    SegG1    = (result >> 6) & 1;
    
    
}
void lag_a(unsigned char display_digit, unsigned char result){
    unsigned int slow = 0;
    while(slow < DELAY){
        displayDigit(display_digit,result);
        slow++;
    }
}
void lag_b(unsigned char display_digit, unsigned char result){
    unsigned int slow = 0;
    while(slow < DELAY){
        displayDigit2(display_digit,result);
        slow++;
    }
   }

void __ISR(_CHANGE_NOTICE_VECTOR, ipl6) ChangeNotice_Handler(void){
    
    IEC1CLR = 0x1;
    i = 0;
    while (i < 100) { // delay loop
        i++;
    }
    i = 0;

    

    press = 16;

//
    

    row1 = 0;
    row2 = 1;
    row3 = 1;
    row4 = 1;
    
    
    if (col1 == 0 ){
        press = 1;
        
    }
    if (col2 == 0){
        press = 2;
        
    }
    if (col3 == 0 ){
        press = 3;
    }
    if (col4 == 0 ){
        press = 10; //This is A
    }
    
    
    row1 = 1;
    row2 = 0;
    row3 = 1;
    row4 = 1;
    if (col4 ==0){
        press = 11; //This is equal to B
    }
    else if (col3 == 0){
        press = 6;
    }
    else if (col2 ==0){
        press = 5;
    }
    else if (col1 == 0){
        press = 4;
    }
    
    
    

     row1 = 1;
    row2 = 1;
    row3 = 0;
    row4 = 1;
    if (col1 == 0 ){
        press = 7;
    }
    else if (col2 == 0){
        press = 8;
    }
    else if (col3 == 0){
        press = 9;
    }
    else if (col4 == 0){
        press = 12; //This is equal to C

    }
    
    row1 = 1;
    row2 = 1;
    row3 = 1;
    row4 = 0;
    if (col1 == 0){
        press = 0;
    }
    else if (col2 == 0){
        press = 15; //This is equal to F
    }
    else if (col3 == 0){
        press = 14; //This is equal to E
    }
    else if (col4 == 0){
        press = 13; //This is equal to D
        
    

}
    
    
    
    
    row1 = 0;
    row2 = 0;
    row3 = 0;
    row4 = 0;
    PORTB;
    //Clears interrupt flag
    IFS1CLR = 0x0001;
    IEC1SET = 0x1;
}
main(){
    PORTE = 0;      
    PORTG = 0;      
    PORTD = 0;
    PORTC = 0;
    PORTB = 0;
    AD1PCFG = 0xFFFF;
    TRISC = 0;
    TRISD = 0;
    TRISE = 0;      
    TRISG = 0;         
    TRISB = 0x0F;
    
    row1 = 0;
    row2 = 0;
    row3 = 0;
    row4 = 0;
    
    /*
The following code example illustrates a Change Notice interrupt configuration for pins
CN1(PORTC.RC13), CN4(PORTB.RB2) and CN18(PORTF.RF5).
*/

    
    // Initialize CN interrupts
    // 1. Configure CNCON , CNEN, CNPUE
// 2. Perform a dummy read to clear mismatch
// 3. Configure IPC6, IFS1, IEC1
// 4. Enable vector interrupt
    //INTEnableSystemMultiVectoredInt();
    INTDisableInterrupts();
    
    /* NOTE: disable vector interrupts prior to configuration */
     // Initialize CNCON, CNEN, CNPUE
CNCON = 0x8000; // Enable Change Notice module
CNEN = 0x0000003C; // Enable individual CN pins CN1, CN4 and CN18
CNPUE = 0x0000003C; // Enable weak pull ups for pins CN1, CN4 and CN18
/* read port(s) to clear mismatch on change notice pins */

PORTB;
    IPC6SET = 0x00180000; 	// Set priority level=6
    IFS1CLR = 0x0001; 		// Clear the interrupt flag status bit
    IEC1SET	= 0x0001; 		// Enable Change Notice interrupts

    //Enable multi-vector interrupt
    INTEnableSystemMultiVectoredInt();
    
    while(1)
    {
         if (press!=16 && press==12 && !buttonLock) {
        
        buttonLock=1;
        displayssd2_r=0;
        displayssd2_l=16;
        displayssd1_r=16;
        displayssd1_l=16;
        pressedkey=0;
        
         }
    
    if (press!=16 && press==13 && !buttonLock) {
        
        buttonLock=1;
        displayssd2_r=digit3;
        displayssd2_l=digit4;
        displayssd1_r=digit5;
        displayssd1_l=16;
        digit2=digit3;
        digit3=digit4;
        digit4=digit5;
        digit5=16;
        pressedkey=0;
        
         }
        
         if (press!=16 && press==10 && !buttonLock) {
        // ADDITION
        buttonLock=1;
        determinant=0;
        sum = digit2*1 + digit3*10 + digit4*100 + digit5*1000;
        add1=digit3;
        add2=digit4;
        displayssd2_r=0;
        displayssd2_l=16;
        displayssd1_r=16;
        displayssd1_l=16;
        pressedkey=0;
        
        
         }
    
         if (press!=16 && press==11 && !buttonLock) {
        // SUBTRACTION
        buttonLock=1;
        determinant=1;
        sum = digit2*1 + digit3*10 + digit4*100 + digit5*1000;
        add1=digit3;
        add2=digit4;
        displayssd2_r=0;
        displayssd2_l=16;
        displayssd1_r=16;
        displayssd1_l=16;
        pressedkey=0;
        
        
         }
    
    
        
        if (press!=16 && press==14 && !buttonLock) {
        // EQUALS
        buttonLock=1;
        sum2 = (digit2*1 + digit3*10 + digit4*100 + digit5*1000);
        if(determinant==0){
        totaladd = sum+sum2;
        displayssd2_r=totaladd%10;
        displayssd2_l=(totaladd/10)%10;
        displayssd1_r=(totaladd/100)%10;
        displayssd1_l=(totaladd/1000)%10;
        }
        else if(determinant==1){
            
        totalsub = sum-sum2;
        displayssd2_r=totalsub%10;
        displayssd2_l=(totalsub/10)%10;
        displayssd1_r=(totalsub/100)%10;
        displayssd1_l=(totalsub/1000)%10;
        }
        pressedkey=0;
        }
        
        if (press!=16 && !buttonLock) {
        for(i=0;i<20000;i++);
        buttonLock=1;
        pressedkey = press;
        
        
        
        if(digit2!=0 && displayssd1_l==16 && displayssd1_r==16){
            displayssd2_r=pressedkey;
            displayssd2_l=digit2;
            displayssd1_r=16;
            displayssd1_l=16;   
        }
        
        if(displayssd2_l != 16 && displayssd2_r != 16 && digit3!=0 && displayssd1_r==16){
            displayssd2_r=pressedkey;
            displayssd2_l=digit2;
            displayssd1_r=digit3;
            displayssd1_l=16;
        }
          
        if(digit4!=16 && press!=10 && press!=11 && press!=12 && press!=13 && press!=14 && displayssd1_r!=16 && displayssd2_r!=16 && displayssd2_l!=16 && digit4!=0){
            displayssd2_r=pressedkey;
            displayssd2_l=digit2;
            displayssd1_r=digit3;
            displayssd1_l=digit4;
        }
        else if(displayssd2_r==digit2 && digit2==0){
            displayssd2_r=pressedkey;
            
        }
        
        
        
        
        
    }
         
         if (dir == 0){
            lag_a(dir,number[displayssd2_r]);
            lag_b(dir,number[displayssd1_r]);
            dir = 1;
            }
            else if (dir == 1){
                lag_a(dir,number[displayssd2_l]);
                lag_b(dir,number[displayssd1_l]);

                dir = 0;
            }
         
    if (press==16 && buttonLock) {
        for(i=0;i<2000;i++);
        buttonLock=0;
        
        
        if(digit3!=16){
        digit5=digit4;
        digit4=digit3;
        digit3=digit2;
        digit2=pressedkey;
        }
        
        else if(digit2!=16){
        digit3=digit2;
        digit2=pressedkey;
        digit4=16;
        }
        
        else if(digit2==0){
        digit2=pressedkey;
        digit3=16;
        digit4=16;
        }
  
        
    }
        
        
        
        }
}
