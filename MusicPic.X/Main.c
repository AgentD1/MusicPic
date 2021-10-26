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

const int NOTES_LENGTH = 25;

extern unsigned char timerHit = 0;

extern unsigned int count = 0;

#define bpm 94
#define quarter_note 0.25
#define eigth_note 0.125
#define sixteenth_note .125
#define half_note 0.5

#define note(x, y) {.period = x, .time = (((bpm/60) * y * 4) * 1000000) }

const Note notes[] = {
    note(NOTE_C5, eigth_note),
    note(NOTE_DS5, eigth_note),
    note(NOTE_F5, eigth_note),
    note(NOTE_FS5, eigth_note),
    note(NOTE_F5, eigth_note),
    note(NOTE_DS5, eigth_note),
    note(NOTE_C5, eigth_note*1.5),
    note(NOTE_AS4, sixteenth_note/2),
    note(NOTE_D5, sixteenth_note/2),
    note(NOTE_C5, eigth_note),
    note(REST, half_note),
    note(NOTE_C5, eigth_note),
    note(NOTE_DS5, eigth_note),
    note(NOTE_F5, eigth_note),
    note(NOTE_GS5, eigth_note),
    note(NOTE_F5, eigth_note),
    note(NOTE_DS5, eigth_note),
    note(NOTE_FS5, half_note),
    note(REST, sixteenth_note),
    note(NOTE_FS5, eigth_note),
    note(NOTE_E5, eigth_note),
    note(NOTE_CS5, eigth_note),
    note(NOTE_FS5, eigth_note),
    note(NOTE_E5, eigth_note),
    note(NOTE_CS5, eigth_note)
};

extern void timer_function(void);

void __interrupt() timer_isr() {
    if(TMR0IF==1 && !timerHit) { // Timer flag has been triggered due to timer overflow
        NOP();
        NOP();
        TMR0IF=0;       // Clear timer interrupt flag
        count++;
        TMR0 = 246;     //Load the timer Value
    }
}

void delay_us(unsigned int us) {
    unsigned int requiredCount = us/4 - 2;
    
    timerHit = 0;
    
    count = 0;
    
    //OPTION_REG = 0b01010111;    // Enable port pull-ups, TMR0 internal, div-256

    OPTION_REG = 0b01010000; // Modified Prescaler
    TMR0IE = 1;
    GIE = 1;
    TMR0IF=0;       // Clear timer interrupt flag
    PEIE = 1;
    
    TMR0 = 240;
    
    while(count < requiredCount) {
        NOP();
    }
        
    TMR0IE = 0;
    GIE = 0;
    
    timerHit = 0xff;
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
                if(notes[i].period == 0) {
                    delay_us(notes[i].time);
                    continue;
                }
                for(unsigned long repeats = notes[i].time / notes[i].period; repeats != 0; repeats--) {
                    BEEPER = !BEEPER;
                    delay_us(notes[i].period);
                }
                delay_us(notes[i].time / 10);
                if(SW1 == 0) {
                    RESET();
                }
            }
            
        }
        
        // delay_us(100);
        // LATC = ~LATC;
        
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

/*
======================
        Songs
======================

Megalovania ish

{.period = NOTE_C4,  .time = 250000},
{.period = NOTE_C4,  .time = 250000},
{.period = NOTE_D5,  .time = 500000},
{.period = NOTE_A4,  .time = 500000},
{.period = 0,        .time = 500000},
{.period = NOTE_GS4, .time = 250000},
{.period = 0,        .time = 750000},
{.period = NOTE_G4,  .time = 250000},
{.period = 0,        .time = 500000},
{.period = NOTE_F4,  .time = 500000},
{.period = NOTE_D4,  .time = 250000},
{.period = NOTE_F4,  .time = 250000},
{.period = NOTE_G4,  .time = 250000},




 */
