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

const Note notes[] = {
    {.period = NOTE_C4, .time = 1000000},
    {.period = NOTE_A4, .time = 1000000},
    {.period = NOTE_B4, .time = 500000}
};

void delay_us(unsigned int us) {
    while(us > 0) {
       us-=10;
        __delay_us(9);
    }
}

int main(void) {
    // Configure oscillator and I/O ports. These functions run once at start-up.
    OSC_config();               // Configure internal oscillator for 48 MHz
    UBMP4_config();             // Configure on-board UBMP4 I/O devices
	
    // Code in this while loop runs repeatedly.
    while(1) {
        // Change pitch
        
        /*if(SW2 == 0) {
            for(int i = 0; i < NOTES_LENGTH; i++) {
                
                for(unsigned long repeats = notes[i].time / notes[i].period; repeats != 0; repeats--) {
                    BEEPER = !BEEPER;
                    delay_us(notes[i].period);
                }
                if(SW1 == 0) {
                    RESET();
                }
            }
        }*/
        
        for(unsigned long repeats = 100; repeats != 0; repeats--) {
            BEEPER = !BEEPER;
            __delay_us(NOTE_C4);
        }
        
        // Activate bootloader if SW1 is pressed.
        if(SW1 == 0)
        {
            RESET();
        }
    }
}
