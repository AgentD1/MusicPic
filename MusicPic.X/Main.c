#include    "xc.h"              // Microchip XC8 compiler include file
#include    "stdint.h"          // Include integer definitions
#include    "stdbool.h"         // Include Boolean (true/false) definitions

#include    "UBMP4.h"           // Include UBMP4 constants and functions

#include    "notes.h"

// TODO Set linker ROM ranges to 'default,-0-7FF' under "Memory model" pull-down.
// TODO Set linker code offset to '800' under "Additional options" pull-down.

// https://exploreembedded.com/wiki/PIC16f877a_Timer
#define SBIT_PS1  5
#define SBIT_PS0  4

#define TIMER1_PRESCALER 8

struct Note {
    unsigned long period;
    unsigned long time;
};

typedef struct Note Note;

const int NOTES_LENGTH = 3;

bool timerHit = false;

unsigned int count = 0;

const Note notes[] = {
    {.period = NOTE_C4, .time = 1000000},
    {.period = NOTE_A4, .time = 1000000},
    {.period = NOTE_B4, .time = 500000}
};

void __interrupt() timer_isr() {  
    if(TMR0IF==1 && !timerHit) { // Timer flag has been triggered due to timer overflow
        TMR0 = 206;     //Load the timer Value
        TMR0IF=0;       // Clear timer interrupt flag
        count++;
    }
}

void delay_us(unsigned int us) {
    unsigned int requiredCount = us/10;
    
    timerHit = false;
    
    count = 0;
    
    //OPTION_REG = 0b01010111;    // Enable port pull-ups, TMR0 internal, div-256

    OPTION_REG = 0b01010000; // Modified Prescaler
    TMR0 = 206;
    TMR0IE = 1;
    GIE = 1;
    PEIE = 1;
    TMR0IF=0;       // Clear timer interrupt flag
    
    
    while(count < requiredCount) NOP();
        
    TMR0IE = 0;
    GIE = 0;
    
    timerHit = true;
    H1OUT = 0;
}

int main(void) {
    // Configure oscillator and I/O ports. These functions run once at start-up.
    OSC_config();               // Configure internal oscillator for 48 MHz
    UBMP4_config();             // Configure on-board UBMP4 I/O devices
    
    TRISC = 0b00000000;         // Set LED pins as outputs, H1-H4 pins as outputs too

	
    // Code in this while loop runs repeatedly.
    while(1) {
        // Change pitch
        
        if(SW2 == 0) {
            for(int i = 0; i < NOTES_LENGTH; i++) {
                for(unsigned long repeats = notes[i].time / notes[i].period; repeats != 0; repeats--) {
                    BEEPER = !BEEPER;
                    delay_us(notes[i].period);
                }
                if(SW1 == 0) {
                    RESET();
                }
            }
        }
        
//        if(SW2 == 0) {
//            for(unsigned long repeats = 100; repeats != 0; repeats--) {
//                H1OUT = !H1OUT;
//                BEEPER = !BEEPER;
//                delay_us(NOTE_C4);
//            }
//        }
        
        // Activate bootloader if SW1 is pressed.
        if(SW1 == 0)
        {
            RESET();
        }
    }
}
