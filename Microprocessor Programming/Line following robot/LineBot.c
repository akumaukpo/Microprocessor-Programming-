#include<p32xxxx.h>
#include<plib.h>
#include <time.h>


#define DELAY 80
#define REFRESH_RATE 250

#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_8 //10 Mhz lower freq

//Light Sensors
#define LS1 PORTDbits.RD14 //sensor
#define LS2 PORTDbits.RD15 //sensor
#define LS3 PORTFbits.RF2 //sensor
#define LS4 PORTFbits.RF8 //sensor

//Onboard LEDs

#define Led1 LATBbits.LATB10
#define Led2 LATBbits.LATB11
#define Led3 LATBbits.LATB12
#define Led4 LATBbits.LATB13

//Button 1

#define Btn1 PORTAbits.RA6

//SSDs

#define SegA    LATEbits.LATE0
#define SegB    LATEbits.LATE1
#define SegC    LATEbits.LATE2
#define SegD    LATEbits.LATE3
#define SegE    LATGbits.LATG9
#define SegF    LATGbits.LATG8
#define SegG    LATGbits.LATG7

#define DispSel LATGbits.LATG6

unsigned char SSD_number[]={
    
0b0111111,    //0
0b0000110,    //1
0b1011011,    //2
0b1001111,    //3
0b1100110,    //4
0b1101101,    //5
0b1111101,    //6
0b0000111,    //7
0b1111111,    //8
0b1101111,    //9
0b0000000,    //clear
};

unsigned int Pwm; // variable to store calculated PWM value
unsigned char Mode = 0;
void Configure_PWM_Module();
int readADC();
void displayDigit(unsigned char, unsigned int);// Display one digit among two digits
void lag(unsigned char display_digit, unsigned char result);
void initADC();
unsigned int i = 0;
unsigned int ct = 0;
unsigned int total = 0;
unsigned int starter = 0;
unsigned int is_stopped = 0;
unsigned int displayssd_l = 0;
unsigned int displayssd_r = 0;
int count = 0;
int ssd = 0;
enum Mode {ll, rr};
enum Mode dir = ll;

void Configure_PWM_Module() 
{       //Right Wheel
        T2CONbits.T32 = 1; // Timer 2 in 32 bit mode 
        OC1CONbits.OC32 = 1; // 32 bit operation 
        PR2 = 200000; // Load PR2
        OC1R = 15000;
        OC1RS = 15000; // Load duty cycle 
        OC1CONbits.OCM = 6; // Enable PWM module 
        OC1CONbits.OCTSEL = 0; // TIMER 2 is the source
        T2CONbits.TCKPS = 0; // Set Timer 2 prescaler = 1 
        T2CONbits.ON = 1; // Enable Timer 2 
        OC1CONbits.ON = 1; // Enable OC1CON
        //Left Wheel
        
        OC2CONbits.OC32 = 1;
        PR4 = 200000;
        OC2R = 15000;
        OC2RS = 15000;
        OC2CONbits.OCM = 6; // Enable PWM module 
        OC2CONbits.OCTSEL = 0; // TIMER 2 is the source
        OC2CONbits.ON = 1; // Enable OC1CON
} 

void displayDigit (unsigned char display_digit, unsigned int value){
    DispSel = display_digit;
    SegA    = value & 1;
    SegB    = (value >> 1) & 1;
    SegC    = (value >> 2) & 1;
    SegD    = (value >> 3) & 1;
    SegE    = (value >> 4) & 1;
    SegF    = (value >> 5) & 1;
    SegG    = (value >> 6) & 1;
}

void lag(unsigned char display_digit, unsigned char result){
    unsigned int slow = 0;
    while(slow < DELAY){
        displayDigit(display_digit, result);
        slow++;
    }
}


 void __ISR(_OUTPUT_COMPARE_1_VECTOR, ipl7) OC1_IntHandler(void) {
        if (Mode) {
            if (Pwm < 0xFFFF) // ramp up mode
            {
                Pwm++; // If the duty cycle is not at max, increase
                OC1RS = Pwm; // Write new duty cycle
            } else {
                Mode = 0; // PWM is at max, change mode to ramp down
            }
        }// end of ramp up
        else {
            if (!Pwm) // ramp down mode
            {
                Pwm--; // If the duty cycle is not at min, increase
                OC1RS = Pwm; // Write new duty cycle
            } else {
                Mode = 1; // PWM is at min, change mode to ramp up
            }
        } // end of ramp down
        // insert user code here
        IFS0CLR = 0x0040; // Clear the OC1 interrupt flag
    }
 
 void __ISR(_TIMER_4_VECTOR, ipl7) T4_Interrupt_ISR(void){
     if (ssd >= 1){
     count = count+1;
     }
     IFS0bits.T4IF = 0;
 }
 
 void timer_set(){
     T4CONbits.ON = 0;
     TMR4 = 0;
     T4CONbits.TCKPS = 7;
     PR4 = 0x495D4;
     //T4CONbits.T32 = 1;
     IFS0bits.T4IF = 0;
     IPC4bits.T4IP = 7;
     IEC0bits.T4IE = 1;
     T4CONbits.ON = 1;
     
 }
 
 
 void initADC(){
    AD1PCFG = 0xF7FF; // all PORTB = digital but RB2 = analog
    AD1CON1 = 0; // manual conversion sequence control
    AD1CHS = 0x000B0000; // Connect RB11/AN11 as CH0 input ..
    AD1CSSL = 0; // no scanning required
    AD1CON2 = 0; // use MUXA, AVss/AVdd used as Vref+/-
    AD1CON3 = 0x0002; // Tad= 34 x Tpb
    AD1CON1bits.ADON = 1; // turn on the ADC
}
 
 int readADC(){
    AD1CON1bits.SAMP = 1;
    for(TMR1 = 0; TMR1<100; TMR1++);
    AD1CON1bits.SAMP = 0;
    while(!AD1CON1bits.DONE);
    return ADC1BUF0;
}
    
    void straight(){
        
        OC1RS = 20000;
        OC2RS = 10000;
    }
    
    void left(){
        
        OC1RS = 15000;
        OC2RS = 13000;
    }
    
    void right(){
        
        OC1RS = 17000;
        OC2RS = 15000;
    }
    
    void hardleft(){
        
        OC1RS = 13000;
        OC2RS = 13000;
    }
    
    void hardright(){
        
        OC1RS = 17000;
        OC2RS = 17000;
    }
    
    void stop(){
        OC1RS = 15000;
        OC2RS = 15000;
        ssd = 0;
    }
    
    void backwards(){
        OC1RS = 10000;
        OC2RS = 20000;
    }
    
    

main(){
     
    PORTB = 0;
    PORTG = 0;
    PORTE = 0;
    TRISE = 0;
    TRISG = 0;
    TRISB = 0b0000100000001111;
    initADC() ;
    Configure_PWM_Module();
    timer_set();
    int delay(int num){
        for(i=0;i<num*1000;i++);
    }
    INTEnableSystemMultiVectoredInt();
    delay(1000);
    

    
while(1){   
    
        displayssd_r = count;
        
        if(readADC() > 500){
            starter++;
            ssd++;
        }   
        
        if(Btn1){
            starter++;
            ssd++;
        } 
        
            if (dir == 0){
                lag(dir, SSD_number[count%10]);
                dir = 1;
            }
        
            else if (dir == 1){
                lag(dir, SSD_number[count/10]);
                dir = 0;
            }
        
        
        

    if (LS1 == 1 && LS2 == 0 && LS3 == 0 && LS4 == 1 && starter >= 1){
        straight();}
    
    else if(LS2 == 1 && LS3 == 0 && starter >= 1){
        hardleft();}
    
    else if(LS3 == 1 && LS2 == 0 && starter >= 1){
        hardright();}
    
    else if(LS4 == 1 && LS3 == 0 && LS2 ==0 && LS1 == 0 && starter >= 1){
        right();}
    
    else if(LS4 == 0 && LS3 == 0 && LS2 ==0 && LS1 == 1 && starter >= 1){
        left();}
    
    else if (LS4 == 0 && LS3 == 0 && LS2 == 0 && LS1 == 0 && count >= 56){
        stop();}
    
    else if (LS4 == 0 && LS3 == 0 && LS2 == 0 && LS1 == 0 && starter >= 1){
        straight();}
    
        if (LS1 == 1){
            Led1 = 1;
        }
        if (LS2 == 1){
            Led2 = 1;
        }
        if (LS3 == 1){
            Led3 = 1;
        }
        if (LS4 == 1){
            Led4 = 1;
        }
        if (LS1 == 0){
            Led1 = 0;
        }
        if (LS2 == 0){
            Led2 = 0;
        }
        if (LS3 == 0){
            Led3 = 0;
        }
        if (LS4 == 0){
            Led4 = 0;
        }
}
}