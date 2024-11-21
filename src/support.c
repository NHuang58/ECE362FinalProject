#include <math.h>
#include "stm32f0xx.h"
#include <string.h> // for memmove()
#include <stdio.h> // for memmove()
#include "main.h"

#define time 250000000

void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}

const char font[] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x00, // 32: space
    0x86, // 33: exclamation
    0x22, // 34: double quote
    0x76, // 35: octothorpe
    0x00, // dollar
    0x00, // percent
    0x00, // ampersand
    0x20, // 39: single quote
    0x39, // 40: open paren
    0x0f, // 41: close paren
    0x49, // 42: asterisk
    0x00, // plus
    0x10, // 44: comma
    0x40, // 45: minus
    0x80, // 46: period
    0x00, // slash
    // digits
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x67,
    // seven unknown
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    // Uppercase
    0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x6f, 0x76, 0x30, 0x1e, 0x00, 0x38, 0x00,
    0x37, 0x3f, 0x73, 0x7b, 0x31, 0x6d, 0x78, 0x3e, 0x00, 0x00, 0x00, 0x6e, 0x00,
    0x39, // 91: open square bracket
    0x00, // backslash
    0x0f, // 93: close square bracket
    0x00, // circumflex
    0x08, // 95: underscore
    0x20, // 96: backquote
    // Lowercase
    0x5f, 0x7c, 0x58, 0x5e, 0x79, 0x71, 0x6f, 0x74, 0x10, 0x0e, 0x00, 0x30, 0x00,
    0x54, 0x5c, 0x73, 0x7b, 0x50, 0x6d, 0x78, 0x1c, 0x00, 0x00, 0x00, 0x6e, 0x00
};

//=============================================================================
// Part 1: 7-segment display update with DMA
//=============================================================================

extern uint16_t msg[8];

void set_digit_segments(int digit, char val) {
    msg[digit] = (digit << 8) | val;
}

void print(const char str[])
{
    const char *p = str;
    for(int i=0; i<8; i++) {
        if (*p == '\0') {
            msg[i] = (i<<8);
        } else {
            msg[i] = (i<<8) | font[*p & 0x7f] | (*p & 0x80);
            p++;
        }
    }
}

void printfloat(float f)
{
    char buf[10];
    snprintf(buf, 10, "%f", f);
    for(int i=1; i<10; i++) {
        if (buf[i] == '.') {
            // Combine the decimal point into the previous digit.
            buf[i-1] |= 0x80;
            memcpy(&buf[i], &buf[i+1], 10-i-1);
        }
    }
    print(buf);
}

void append_segments(char val) {
    for (int i = 0; i < 7; i++) {
        set_digit_segments(i, msg[i+1] & 0xff);
    }
    set_digit_segments(7, val);
}

void clear_display(void) {
    for (int i = 0; i < 8; i++) {
        msg[i] = msg[i] & 0xff00;
    }
}



//=============================================================================
// Part 2: Debounced keypad scanning.
//=============================================================================

// 16 history bytes.  Each byte represents the last 8 samples of a button.
uint8_t hist[16];
char queue[2];  // A two-entry queue of button press/release events.
int qin;        // Which queue entry is next for input
int qout;       // Which queue entry is next for output

const char keymap[] = "DCBA#9630852*741";

void push_queue(int n) {
    queue[qin] = n;
    qin ^= 1;
}

char pop_queue() {
    char tmp = queue[qout];
    queue[qout] = 0;
    qout ^= 1;
    return tmp;
}

void update_history(int c, int rows)
{
    // We used to make students do this in assembly language.
    for(int i = 0; i < 4; i++) {
        hist[4*c+i] = (hist[4*c+i]<<1) + ((rows>>i)&1);
        if (hist[4*c+i] == 0x01)
            push_queue(0x80 | keymap[4*c+i]);
        if (hist[4*c+i] == 0xfe)
            push_queue(keymap[4*c+i]);
    }
}

void drive_column(int c)
{
    GPIOC->BSRR = 0xf00000 | ~(1 << (c + 4));
}

int read_rows()
{
    return (~GPIOC->IDR) & 0xf;
}

char get_key_event(void) {
    for(;;) {
        asm volatile ("wfi");   // wait for an interrupt
       if (queue[qout] != 0)
            break;
    }
    return pop_queue();
}

char get_keypress() {
    char event;
    for(;;) {
        // Wait for every button event...
        event = get_key_event();
        // ...but ignore if it's a release.
        if (event & 0x80)
            break;
    }
    return event & 0x7f;
}

void show_keys(void)
{
    char buf[] = "        ";
    for(;;) {
        char event = get_key_event();
        memmove(buf, &buf[1], 7);
        buf[7] = event;
        print(buf);
    }
}

// Turn on the dot of the rightmost display element.
void dot()
{
    msg[7] |= 0x80;
}

// Read an entire floating-point number.
float getfloat(void)
{
    int num = 0;
    int digits = 0;
    int decimal = 0;
    int enter = 0;
    
    clear_display();
    set_digit_segments(7, font['0']);
    while(!enter) {
        int key = get_keypress();
        if (digits == 8) {
            if (key != '#')
                continue;
        }
        switch(key) {
        case '0':
            if (digits == 0)
                continue;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            num = num*10 + key-'0';
            decimal <<= 1;
            digits += 1;
            if (digits == 1)
                set_digit_segments(7, font[key]);
            else
                append_segments(font[key]);
            break;
        case '*':
            if (decimal == 0) {
                decimal = 1;
                dot();
            }
            break;
        case '#':
            enter = 1;
            break;
        default: continue; // ABCD
        }
    }
    float f = num;
    while (decimal) {
        decimal >>= 1;
        if (decimal)
            f = f/10.0;
    }
    return f;
}

uint8_t bcd2dec(uint8_t bcd) {
    // Lower digit
    uint8_t dec = bcd & 0xF;

    // Higher digit
    dec += 10 * (bcd >> 4);
    return dec;
}

void setrgb(int rgb) {
    uint8_t b = bcd2dec((rgb) & 0xFF);
    uint8_t g = bcd2dec((rgb >> 8) & 0xFF);
    uint8_t r = bcd2dec((rgb >> 16) & 0xFF);

    // TODO: Assign values to TIM1->CCRx registers
    // Remember these are all percentages
    // Also, LEDs are on when the corresponding PWM output is low
    // so you might want to invert the numbers. 
    

    TIM1->CCR1 = (2400 - ((2400 * r) / 99));
    TIM1->CCR2 = (2400 - ((2400 * g) / 99));
    TIM1->CCR3 = (2400 - ((2400 * b) / 99));
}


//===========================================================================
// Part 4: Create an analog sine wave of a specified frequency
//===========================================================================
void set_freq(int chan, float freq);

void dialer(void)
{
    clear_display();
    // North American dial tone is 350 Hz and 440 Hz together...
    set_freq(0,350);
    set_freq(1,440);
    char dial[16] = { 0 };
    int dialed = 0;
    int replay = 0;
    int replayed = 0;
    clear_display();
    for(;;) {
        char event;
        if (replay) {
            if (replayed == dialed) {
                replay = 0;
                continue;
            }
            event = dial[replayed++];
        } else {
            event = get_key_event();
            if (event == (0x80 | '#')) {
                clear_display();
                replay = 1;
                replayed = 0;
                continue;
            }
            if ((event & 0x80) == 0) {
                set_freq(0,0);
                set_freq(1,0);
                continue;
            }
            event &= 0x7f;
            if (dialed < 16) {
                dial[dialed] = event;
                dialed++;
            }
        }
        append_segments(font[event+0]);
        switch(event) {
        case '1':
        case '2':
        case '3':
        case 'A': set_freq(0,697); break;
        case '4':
        case '5':
        case '6':
        case 'B': set_freq(0,770); break;
        case '7':
        case '8':
        case '9':
        case 'C': set_freq(0,852); break;
        case '*':
        case '0':
        case '#':
        case 'D': set_freq(0,941); break;
        }
        switch(event) {
        case '1':
        case '4':
        case '7':
        case '*': set_freq(1,1209); break;
        case '2':
        case '5':
        case '8':
        case '0': set_freq(1,1336); break;
        case '3':
        case '6':
        case '9':
        case '#': set_freq(1,1477); break;
        case 'A':
        case 'B':
        case 'C':
        case 'D': set_freq(1,1633); break;
        }
        if (replay) {
            nano_wait(125000000);
            set_freq(0,0);
            set_freq(1,0);
            nano_wait(80000000);
        }
    }
}

void play_piano(void) {
    float freq = 0.0;         // Base frequency (A4)
    int enter = 0;              // Sentinel variable
    int flat = 0;               // Indicates if the note is sharp
    int octave = 4;
    int rgb = 0;
    char note = 'A';            // Initial note
    char note_display;          // Stores the current note or note with sharp
    char noteToSend = 'A';

    clear_display();            // Initialize the display
    for (int i = 0; i < 8; i++) {
        set_digit_segments(i, font[' ']); // Start with a blank display
    }

    while (!enter) {
        DAC->CR &= ~DAC_CR_EN1;
        int key = get_keypress();

        // Handle octave adjustment without playing a note
        if (key == '*') {
            if (octave > 0) {
                octave--;
            }
            continue; // Skip playing a note
        } else if (key == '#') {
            if (octave < 8) {
                octave++;
            }
            continue; // Skip playing a note
        }

        // Determine note and frequency
        switch (key) {
            case '1': freq = 261.63; note = 'C'; flat = 0; noteToSend = 'c'; break; // C
            case '2': freq = 277.18; note = 'D'; flat = 1; noteToSend = 'C'; break; // C#/Db
            case '3': freq = 293.66; note = 'D'; flat = 0; noteToSend = 'd'; break; // D
            case 'A': freq = 311.13; note = 'E'; flat = 1; noteToSend = 'D'; break; // D#/Eb
            case '4': freq = 329.63; note = 'E'; flat = 0; noteToSend = 'e'; break; // E
            case '5': freq = 349.23; note = 'F'; flat = 0; noteToSend = 'f'; break; // F
            case '6': freq = 369.99; note = 'G'; flat = 1; noteToSend = 'F'; break; // F#/Gb
            case 'B': freq = 392.00; note = 'G'; flat = 0; noteToSend = 'g'; break; // G
            case '7': freq = 415.30; note = 'A'; flat = 1; noteToSend = 'G'; break; // G#/Ab
            case '8': freq = 440.00; note = 'A'; flat = 0; noteToSend = 'a'; break; // A
            case '9': freq = 466.16; note = '8'; flat = 1; noteToSend = 'A'; break; // A#/Bb
            case 'C': freq = 493.88; note = '8'; flat = 0; noteToSend = 'b'; break; // B
            
            case '0': freq = 440.00; note = ' '; octave = 4; flat = 0; break;
            case 'D': freq = 0     ; note = '0'; enter  = 1; flat = 0; break;  // Turn off
            default:  continue; // Ignore other keys
        }


        switch (octave) {
            case 0 : rgb = 332211; break;
            case 1 : rgb = 221144; break;
            case 2 : rgb = 554433; break;
            case 3 : rgb = 440000; break;
            case 4 : rgb = 004400; break;
            case 5 : rgb = 000044; break;
            case 6 : rgb = 224477; break;
            case 7 : rgb = 112233; break;
            default: rgb = 000000; break;
        }

        // Set the current note to display
        note_display = flat ? font['b'] : font[' '];

        // Shift the display two positions to the left and append the note
        drawNote(noteToSend);
        append_segments(font[note]);       // Note
        append_segments(note_display);    // Sharp or blank

        // Send the Char to the TFT
        // Set the rgb LED for the octave
        // Set the frequency for the note
        DAC->CR |= DAC_CR_EN1;

        set_freq(0, freq * pow(2, (octave - 4)));
        setrgb(rgb);

        nano_wait(time);
        
        setrgb(0);
        // set_freq(0, 0);
    }
}



void update_display(char display_buffer[8]) {
    // Iterate through the 8-character buffer
    for (int i = 0; i < 8; ++i) {
        if (display_buffer[i] == ' ') {
            set_digit_segments(i, 0);
        } else {
            // Display the character using the font map
            set_digit_segments(i, display_buffer[i]);
        }
    }
}

void play_note(char note, int flat, int octave) {
    float base_freq;
    
    // Determine the base frequency of the note
    switch (note) {
        case 'C': base_freq = 261.63; break; // C
        case 'D': base_freq = 293.66; break; // D
        case 'E': base_freq = 329.63; break; // E
        case 'F': base_freq = 349.23; break; // F
        case 'G': base_freq = 392.00; break; // G
        case 'A': base_freq = 440.00; break; // A
        case 'B': base_freq = 493.88; break; // B
        default: return; // Invalid note
    }

    // Adjust frequency for sharp/flat
    if (flat) {
        base_freq *= pow(2.0, 1.0 / 12.0); // Sharpen the note by a semitone
    }

    // Adjust frequency for the octave
    float adjusted_freq = base_freq * pow(2.0, (octave - 4));

    // Play the note
    set_freq(0, adjusted_freq);
    append_segments(font[note]);
    append_segments(font[' ']);

    // Wait for a short duration (e.g., 500ms)
    nano_wait(500000000);

    // Turn off the sound
    set_freq(0, 0);
}
