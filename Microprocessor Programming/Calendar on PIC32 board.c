
// CPEG222 - Project 1 template
// Inputs:  One on-board button: Btn1
// Outputs: One PmodSSD
// Function Description:  The SSD will display 9 on the left digit initially.
//                        Pressing btn1 will switch the display to the other digit.

// Akuma Akuma-Ukpo , Alex Bowe, Carl Agbemabiese

#include <p32xxxx.h>

#define SYS_FREQ     80000000L

// Configuration Bit settings (Don't touch them if not necessary)
// SYSCLK = 80 MHz (8MHz Crystal/ FPLLIDIV * FPLLMUL / FPLLODIV)
// PBCLK = 40 MHz
// Primary Osc w/PLL (XT+,HS+,EC+PLL)
// WDT OFF
// Other options are don't care
#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_2

// The speed control
#define DELAY 10
// The refresh rate of the segments
#define REFRESH_RATE 250   // ~1/100 secs

/****************************************************/
/* Port Definition                           */
/*    You should add your port definition if necessary*/
/****************************************************/

// Buttons
#define Btn1    PORTAbits.RA6
#define Btn2    PORTAbits.RA7  

// 7 Segment Display pmod for using the TOP JA & JB jumpers
// Segments
#define SegA    LATEbits.LATE0
#define SegB    LATEbits.LATE1
#define SegC    LATEbits.LATE2
#define SegD    LATEbits.LATE3
#define SegE    LATGbits.LATG9
#define SegF    LATGbits.LATG8
#define SegG    LATGbits.LATG7

#define SegA2   LATGbits.LATG12
#define SegB2   LATGbits.LATG13
#define SegC2   LATGbits.LATG14
#define SegD2   LATGbits.LATG15
#define SegE2   LATDbits.LATD7
#define SegF2   LATDbits.LATD1
#define SegG2   LATDbits.LATD9


//pmod 7 of 8 leds

#define PLed1   LATDbits.LATD14
#define PLed2   LATFbits.LATF8
#define PLed3   LATFbits.LATF2
#define PLed4   LATDbits.LATD15
#define PLed5   LATEbits.LATE9
#define PLed6   LATDbits.LATD3
#define PLed7   LATDbits.LATD11


//Define on board LED


#define Led1	LATBbits.LATB10
#define Led2    LATBbits.LATB11 
#define Led3	LATBbits.LATB12
#define Led4    LATBbits.LATB13 


// Display selection. 0 = right digit, 1 = left digit
#define DispSel LATGbits.LATG6
#define DispSelR LATCbits.LATC1

/*    Definition of Modes    */
/*    Init/Left : Left digit display    */
/*   Right:      Right digit display   */
///enum Mode {Left,Right};

//Subroutine, add yours for the project
void displayDigit(unsigned char, unsigned char); // Display one digit among two digits
void slowDown(unsigned char, unsigned char); // Frequency control function
void viewMonth (int display_value);
void viewDay (int display_value);
void mode4 (int month, int day);
void DayofWeek(int daysofweek);

// look-up table for the numbers
unsigned int Months[]={0,31,29,31,30,31,30,31,31,30,31,30,31};
unsigned int dayofweekarray[]={0,1,2,3,4,5,6};



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
    0x77,//10
    0x7C, //11
    0b1110111,   //A
    0b1111100,   //b
    0b0111001,   //C
    0b1011110,   //d
    0b1111001,   //E
    0b1110001,   //F
    0b0000000   //clear
};

//variable definition
unsigned int display_value =0x9; // The initial displayed value
unsigned int Buttonlock=0;
//enum Mode mode=Left; //Initial mode is left


main(){
    //initialization
    TRISA |= 0xC0;    // set PORTA as input
    TRISE = 0;      // set PORTE as output
    TRISG = 0;      // set PORTG as output
    TRISD = 0;
    TRISB = 0;
    TRISF = 0;
    TRISC = 0;
    PORTE = 0;      // initialize PORTE to 0
    PORTG = 0;      // initialize PORTG to 0
    PORTD = 0;
    PORTC = 0;
    //PORTA = 0;
    PORTB = 0x000F;
    PORTF = 0;
    unsigned int a = 1;
    unsigned int b  = 1;
    unsigned int mode = 1;
    unsigned int threshold=375000;
    unsigned int j=0;
    unsigned int i=0;
    unsigned int k=0;
    unsigned int weekday=0;
    unsigned int c = 0;
  
   
    while(1){
        
        // main loop
        
            
            if (mode==1){
                
               
                viewMonth(a);
                viewDay(b);
                
                // Led Display Logic
                Led1=1;
                Led2=0;
                Led3=0;
                Led4=0;
                                       
                
                          
            }
            
            else if (mode==2){
                viewMonth(a);
                viewDay(b);
                
                // Led display logic
                Led1=0;
                Led2=1;
                Led3=0;
                Led4=0;
            }
            
            else if (mode==3){
                viewMonth(a);
                viewDay(b);
                Led1=0;
                Led2=0;
                Led3=1;
                Led4=0;
               
               
            }
            
            else if (mode==4){
                
               
                DayofWeek(weekday); // compute current weekday
                viewMonth(a);
                viewDay(b);
                
                Led1=0;
                Led2=0;
                Led3=0;
                Led4=1;
                
                mode4(a,b); // run mode 4 function
                c++;
                
            }
       
        
        

        
        // Next state logic
        if (Btn1&& !Btn2 &&!Buttonlock){
            Buttonlock = 1;
            
            for(i=0;i<2000;i++); // delay
            
            if (mode==1){
               
                mode = 2;  // switch to mode 2 if in mode 1
                
               
                
            }
            
            else if (mode==2){ // switch to mode 3 if in mode 2
                mode=3; 
            }
            
            else if (mode==3){ // switch to mode 4 if in mode 3
                mode=4;
            }
            
            else if (mode==4){ // switch back to mode 2 if in mode 4
                mode=2;
            }   
        }
        
        else if (Btn2&& !Btn1 &&!Buttonlock){ //Btn 2 Logic
            Buttonlock =1;
            for(i=0;i<2000;i++); // delay
            
            if (mode==1){
                mode=4;
            }
            else if (mode==2){
                a++;
                b=b;
                if (a==13){
                    a=1;
                }
            }
            else if (mode==3){
                a=a;
                b++;
                if(Months[a]==31){
                    if (b==32){
                        b=1;
                    }
                }
                
                if (Months[a]==30){
                    if (Months[a]==30){
                        if (b==31){
                            b=1;
                        }
                    }
                }
                
                if (Months[a]==29){
                    if (Months[a]==29){
                        if (b==30){
                            b=1;
                        }
                    }
                }
                
                
                
                
                
            }
            
            else if (mode==4){
              
             
            
                     
            
        }
            
            
    
        
        
        
        
        
       /* if (Btn1&&!Buttonlock)
        {
            //slowDown(mode==Left, number[display_value&0xF]);   // debouncing & display digit
            Buttonlock=1;
            
           /* if(mode==Left)
                mode=Right;
            else
                mode=Left;
        }
        
        */ 
        
        else if(!Btn1 && !Btn2&& Buttonlock)
        {
            //slowDown(mode==Left, number[display_value&0xF]);   // debouncing & display digit
            Buttonlock=0;
        }
        
        if(Btn1&&Btn2){   //pressing both buttons goes back to mode 1 
            Buttonlock=1;
            
            for(i=0;i<2000;i++); 
            
            mode=1;
            a=1;
            b=1;
            
            Led1=1;
            Led2=0;
            Led3=0;
            Led4=0;
            PLed1=0;
            PLed2=0;
            PLed3=0;
            PLed4=0;
            PLed5=0;
            PLed6=0;
            PLed7=0;
            
        }
        
    }



}

//display digit
void displayDigit (unsigned char display_digit, unsigned char value){
    DispSel = display_digit;
    SegA    = value & 1;
    SegB    = (value >> 1) & 1;
    SegC    = (value >> 2) & 1;
    SegD    = (value >> 3) & 1;
    SegE    = (value >> 4) & 1;
    SegF    = (value >> 5) & 1;
    SegG    = (value >> 6) & 1;
}

    
void displayDigit1(unsigned char display_digit, unsigned char value){    
    DispSelR = display_digit;
    SegA2    = value & 1;
    SegB2    = (value >> 1) & 1;
    SegC2    = (value >> 2) & 1;
    SegD2    = (value >> 3) & 1;
    SegE2    = (value >> 4) & 1;
    SegF2    = (value >> 5) & 1;
    SegG2    = (value >> 6) & 1;

}






// debouncing keys & display
void slowDown(unsigned char display_digit, unsigned char value){
    unsigned int slow = 0;
    while(slow < DELAY){
        
        displayDigit(display_digit,value);
        slow++;
    }
}

void viewDay(int display_value){
    int i;
    displayDigit(0, number[display_value%10]);
    for (i = 0; i < 2000; i++);
    displayDigit ( 1, number[(display_value/10)%10] );
    for (i = 0; i < 300; i++);
    

}

void viewMonth (int display_value){
    int i; 
    
    
    displayDigit1 (0, number[display_value%10]);
    for (i = 0; i < 2000; i++);
    displayDigit1 ( 1, number[(display_value/10)%10] );
    for (i = 0; i < 300; i++);
    
}


//mode 4 computation functions 


void mode4 (int a, int b){
    int k;
    int i;
    int j;
    
    int weekday = Calculate_weekday(a,b);
    
    
    for (k=0;k<365;k++){   
        
        
        if (Months[a]==31){
            for(j=0;j<9000;j++);  
                b++;
                DayofWeek(weekday);
                weekday++;
                
        if(b==32){
            if (a==12){
                DayofWeek(weekday);
                weekday++;
                a=1;
                b=1;
            
            }
            
            else {
            DayofWeek(weekday);
            weekday++;
            a++;
            b=1;}
      }
        }
          
    if (Months[a]==30){
        
        for(j=0;j<10000;j++);
        
        DayofWeek(weekday);
        weekday++;
        b++;
        
        
        if(b==31){
            
        DayofWeek(weekday);
        weekday++;
        a++;
        b=1;
        
        }
                       }
    if (Months[a]==29){
        
        for(j=0;j<10000;j++);
        
            DayofWeek(weekday);
            weekday++;
            b++;
            
            if(b==30){
                DayofWeek(weekday);
                weekday++;
                a++;
                b=1;
                        }
                    }
    viewMonth(a);
    viewDay(b);
    
    }
}

//function for detecting day 

void DayofWeek(int weekday){
    if (weekday%7==0){
        PLed1=1;
        PLed2=0;
        PLed3=0;
        PLed4=0;
        PLed5=0;
        PLed6=0;
        PLed7=0;
        
    }
    else if (weekday%7==1){
        PLed1=0;
        PLed2=1;
        PLed3=0;
        PLed4=0;
        PLed5=0;
        PLed6=0;
        PLed7=0;
        
    }
    else if (weekday%7==2){
        PLed1=0;
        PLed2=0;
        PLed3=1;
        PLed4=0;
        PLed5=0;
        PLed6=0;
        PLed7=0;
        
    }
        else if (weekday%7==3){
        PLed1=0;
        PLed2=0;
        PLed3=0;
        PLed4=1;
        PLed5=0;
        PLed6=0;
        PLed7=0;
        
    }   else if (weekday%7==4){
        PLed1=0;
        PLed2=0;
        PLed3=0;
        PLed4=0;
        PLed5=1;
        PLed6=0;
        PLed7=0;
        
    }   else if (weekday%7==5){
        PLed1=0;
        PLed2=0;
        PLed3=0;
        PLed4=0;
        PLed5=0;
        PLed6=1;
        PLed7=0;
        
    }   else if (weekday%7==6){
        PLed1=0;
        PLed2=0;
        PLed3=0;
        PLed4=0;
        PLed5=0;
        PLed6=0;
        PLed7=1;
        
    }  
}

//function to compute weekday from month and day 

int Calculate_weekday (int month, int day){
    int weekday;
    int year = 2016;
    int century = 2016/100;
    weekday = 1.25* year + day-2*(century%4);
    
    return weekday;
    
    
} 
}
 







 

