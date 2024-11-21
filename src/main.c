/**
  ******************************************************************************
  * @file    main.c
  * @author  Weili An, Niraj Menon
  * @date    Jan 24 2024
  * @brief   ECE 362 Lab 4 Student template
  ******************************************************************************
*/

/**
******************************************************************************/

// Fill out your username, otherwise your completion code will have the 
// wrong username!
const char* username = "huan1811";

/******************************************************************************
*/ 

#include "stm32f0xx.h"
#include "spi_functions.h"
#include <math.h>   // for M_PI
#include <stdint.h>
#include <stdio.h>

void nano_wait(int);


//=============================================================================
// Part 1: 7-segment display update with DMA
//=============================================================================

// 16-bits per digit.
// The most significant 8 bits are the digit number.
// The least significant 8 bits are the segments to illuminate.
uint16_t msg[8] = { 0x0000,0x0100,0x0200,0x0300,0x0400,0x0500,0x0600,0x0700 };
extern const char font[];
// Print an 8-character string on the 8 digits
void print(const char str[]);
// Print a floating-point value.
void printfloat(float f);


//============================================================================
// enable_ports()
//============================================================================
void enable_ports(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    GPIOB->MODER &= 0xffffff00;
    GPIOC->MODER &= 0xffffff00;

    GPIOB->MODER |= 0x00d55555;
    GPIOC->MODER |= 0x00005500;

    GPIOC->OTYPER |= 0xf0;
    GPIOC->PUPDR |= 0x55;
}

//============================================================================
// setup_dma() + enable_dma()
//============================================================================
void setup_dma(void) {
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    DMA1_Channel5->CMAR = (uint32_t) &msg;
    DMA1_Channel5->CPAR = (uint32_t) &GPIOB->ODR;

    DMA1_Channel5->CNDTR = 8;
    DMA1_Channel5->CCR |= DMA_CCR_DIR;
    DMA1_Channel5->CCR |= DMA_CCR_MINC;
    DMA1_Channel5->CCR |= 0x400;
    DMA1_Channel5->CCR |= 0x100;
    DMA1_Channel5->CCR |= 0x20;

}

void enable_dma(void) {
    DMA1_Channel5->CCR |= 0x1;
}

//============================================================================
// init_tim15()
//============================================================================
void init_tim15(void) {
    RCC->APB2ENR |= RCC_APB2ENR_TIM15EN; //enable timer

    TIM15->PSC = (4800 - 1);
    TIM15->ARR = (10 - 1);

    TIM15->DIER |= TIM_DIER_UDE;

    TIM15->CR1 |= 0x1;
}

//=============================================================================
// Part 2: Debounced keypad scanning.
//=============================================================================

uint8_t col; // the column being scanned

void drive_column(int);   // energize one of the column outputs
int  read_rows();         // read the four row inputs
void update_history(int col, int rows); // record the buttons of the driven column
char get_key_event(void); // wait for a button event (press or release)
char get_keypress(void);  // wait for only a button press event.
float getfloat(void);     // read a floating-point number from keypad
void show_keys(void);     // demonstrate get_key_event()

//============================================================================
// The Timer 7 ISR
//============================================================================
// Write the Timer 7 ISR here.  Be sure to give it the right name.
void TIM7_IRQHandler() {
    TIM7->SR &= ~TIM_SR_UIF;

    // int rows = read_rows();
    uint8_t rows = read_rows();
    update_history(col, rows);
    col = (col + 1) & 3;
    drive_column(col);
}

//============================================================================
// init_tim7()
//============================================================================
void init_tim7(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;

    TIM7->PSC = 47;
    TIM7->ARR = 999;

    TIM7->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM7_IRQn);
    TIM7->CR1 |= TIM_CR1_CEN;
}

//=============================================================================
// Part 3: Analog-to-digital conversion for a volume level.
//=============================================================================
uint32_t volume = 2048;

//============================================================================
// setup_adc()
//============================================================================
void setup_adc(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER |= GPIO_MODER_MODER1;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->CR2 |= RCC_CR2_HSI14ON;

    while (!(RCC->CR2 & RCC_CR2_HSI14RDY)) {
        nano_wait(1);
    }

    ADC1->CR |= ADC_CR_ADEN;

    while (!(RCC->CR2 & RCC_CR2_HSI14RDY)) {
        nano_wait(1);
    }

    ADC1->CHSELR |= ADC_CHSELR_CHSEL1;


    while (!(RCC->CR2 & RCC_CR2_HSI14RDY)) {
        nano_wait(1);
    }
}

//============================================================================
// Varables for boxcar averaging.
//============================================================================
#define BCSIZE 32
int bcsum = 0;
int boxcar[BCSIZE];
int bcn = 0;

//============================================================================
// Timer 2 ISR
//============================================================================
// Write the Timer 2 ISR here.  Be sure to give it the right name.
void TIM2_IRQHandler(void) {
    TIM2->SR &= ~TIM_SR_UIF;
    ADC1->CR |= ADC_CR_ADSTART;

    while (!(ADC1->ISR & ADC_ISR_EOC)) {
        nano_wait(1);
    }

    bcsum -= boxcar[bcn];
    bcsum += boxcar[bcn] = ADC1->DR;
    bcn += 1;
    if (bcn >= BCSIZE) bcn = 0;
    volume = bcsum / BCSIZE;

    // for(int x=0; x<10000; x++)
    // ;
}

//============================================================================
// init_tim2()
//============================================================================
void init_tim2(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->PSC = (4800 - 1);
    TIM2->ARR = (1000 -1);

    TIM2->DIER |= TIM_DIER_UIE;
    
    // NVIC_SetPriority(TIM2_IRQn, 3);
    NVIC_EnableIRQ(TIM2_IRQn);
    TIM2->CR1 |= TIM_CR1_CEN;
}

void setup_tim1(void) {
    // Generally the steps are similar to those in setup_tim3
    // except we will need to set the MOE bit in BDTR. 
    // Be sure to do so ONLY after enabling the RCC clock to TIM1.
    
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    GPIOA->MODER &= (0xff00ffff);

    GPIOA->MODER |= 0xaa0000;

    GPIOA->AFR[1] &= ~(0x0000ffff);
    GPIOA->AFR[1] |= 0x2222;

    TIM1->BDTR |= TIM_BDTR_MOE;

    TIM1->PSC = (1 - 1);
    TIM1->ARR = (2400 -1);

    TIM1->CCMR2 &= (TIM_CCMR2_OC4M_0 | TIM_CCMR2_OC3M_0);
    TIM1->CCMR1 &= (TIM_CCMR1_OC2M_0 | TIM_CCMR1_OC1M_0);

    TIM1->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2;
    TIM1->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;
    TIM1->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;
    TIM1->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;

    TIM1->CCMR2 |= TIM_CCMR2_OC4PE;

    TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;

    TIM1->CR1 |= TIM_CR1_CEN;
}


//===========================================================================
// Part 4: Create an analog sine wave of a specified frequency
//===========================================================================
void dialer(void);

// Parameters for the wavetable size and expected synthesis rate.
#define N 1000
#define RATE 20000
short int wavetable[N];
int step0 = 0;
int offset0 = 0;
int step1 = 0;
int offset1 = 0;

//===========================================================================
// init_wavetable()
// Write the pattern for a complete cycle of a sine wave into the
// wavetable[] array.
//===========================================================================
void init_wavetable(void) {
    for(int i=0; i < N; i++)
        wavetable[i] = 32767 * sin(2 * M_PI * i / N);
}

//============================================================================
// set_freq()
//============================================================================
void set_freq(int chan, float f) {
    if (chan == 0) {
        if (f == 0.0) {
            step0 = 0;
            offset0 = 0;
        } else
            step0 = (f * N / RATE) * (1<<16);
    }
    if (chan == 1) {
        if (f == 0.0) {
            step1 = 0;
            offset1 = 0;
        } else
            step1 = (f * N / RATE) * (1<<16);
    }
}

//============================================================================
// setup_dac()
//============================================================================
void setup_dac(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  

    GPIOA->MODER |= GPIO_MODER_MODER4;
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;
    
    // DAC->CR |= DAC_CR_TSEL1_0;
    DAC->CR = (0x1 << 2);

    // DAC->CR |= DAC_CR_TEN1;
    DAC->CR |= DAC_CR_EN1;
}

//============================================================================
// Timer 6 ISR
//============================================================================
// Write the Timer 6 ISR here.  Be sure to give it the right name.
void TIM6_DAC_IRQHandler(){
    TIM6->SR &= ~TIM_SR_UIF;

    offset0 += step0;
    offset1 += step1;

    if (offset0 >= (N << 16)) {offset0 -= (N << 16);}
    if (offset1 >= (N << 16)) {offset1 -= (N << 16);}

    int samp = wavetable[offset0 >> 16] + wavetable[offset1 >> 16];//
    
    samp *= volume;
    samp = (samp >> 17);
    samp += 2048;
    DAC->DHR12R1 = (uint16_t) samp;

}

//============================================================================
// init_tim6()
//============================================================================
void init_tim6(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

    int scale = 48000000 / RATE;

    TIM6->PSC = ((scale / 2) - 1);
    TIM6->ARR = (2 - 1);

    TIM6->DIER |= TIM_DIER_UIE;
    
    NVIC_EnableIRQ(17);
    TIM6->CR2 |= TIM_CR2_MMS_1;    
    TIM6->CR1 |= TIM_CR1_CEN;
}

//============================================================================
// All the things you need to test your subroutines.
//============================================================================
int main(void) {
    internal_clock();
    // Initialize the display to something interesting to get started.
    //PIANO NOTES
    msg[0] |= font['C'];
    msg[1] |= font['D'];
    msg[2] |= font['E'];
    msg[3] |= font['F'];
    msg[4] |= font['G'];
    msg[5] |= font['A'];
    msg[6] |= font['B'];
    msg[7] |= font['C'];

    // Uncomment when you are ready to produce a confirmation code.
    // autotest();
    //debug here
/*
    enable_ports();
    setup_dma();
    enable_dma();
    init_tim15();
    init_tim7();
    setup_adc();
    init_tim2();
    init_wavetable();
    init_tim6();

    setup_tim1();
*/


#define TEST_TFT
#ifdef TEST_TFT
    Amir_init_lcd_spi();  
    Amir_init_lcd();      
    Amir_test_lcd();   
#endif
    // Comment this for-loop before you demo part 1!
    // Uncomment this loop to see if "ECE 362" is displayed on LEDs.
    
    // for (;;) {
    //     asm("wfi");
    // }

    // End of for loop

    // Demonstrate part 1
// #define SCROLL_DISPLAY
#ifdef SCROLL_DISPLAY
    for(;;)
        for(int i=0; i<8; i++) {
            print(&"CDEFGABC"[i]);
            nano_wait(250000000);
        }
#endif

    init_tim7();

    // Demonstrate part 2
// #define SHOW_KEY_EVENTS
#ifdef SHOW_KEY_EVENTS
    show_keys();
#endif

    setup_adc();
    init_tim2();

    // Demonstrate part 3
// #define SHOW_VOLTAGE
#ifdef SHOW_VOLTAGE
    for(;;) {
        printfloat(2.95 * volume / 4096);
    }
#endif

    init_wavetable();
    setup_dac();
    init_tim6();

// #define ONE_TONE
#ifdef ONE_TONE
    for(;;) {
        float f = getfloat();
        set_freq(0,f);
    }
#endif

    // demonstrate part 4
// #define MIX_TONES
#ifdef MIX_TONES
    for(;;) {
        char key = get_keypress();
        if (key == 'A')
            set_freq(0,getfloat());
        if (key == 'B')
            set_freq(1,getfloat());
    }
#endif

// #define PIANO
#ifdef PIANO
    // for(;;) {
        play_piano();
    // }
#endif

// #define HAPPY_BIRTHDAY
#ifdef HAPPY_BIRTHDAY
    // Define the notes for the "Happy Birthday" song
    struct Note {
        char note;  // Note character ('A'-'G')
        int flat;   // 1 for sharp, 0 for natural
        int octave; // Octave number (0-8)
        int duration; // Duration in milliseconds
    };

    struct Note happy_birthday_notes[] = {
        {'C', 0, 5, 200}, // Happy
        {'C', 0, 5, 200}, // Birthday
        {'D', 0, 5, 400}, // to
        {'C', 0, 5, 400}, // you
        {'F', 0, 5, 400}, // Happy
        {'E', 0, 5, 800}, // Birthday

        {'C', 0, 5, 200}, // Happy
        {'C', 0, 5, 200}, // Birthday
        {'D', 0, 5, 400}, // to
        {'C', 0, 5, 400}, // you
        {'G', 0, 5, 400}, // Happy
        {'F', 0, 5, 800}, // Birthday

        {'C', 0, 5, 200}, // Happy
        {'C', 0, 5, 200}, // Birthday
        {'C', 0, 6, 400}, // dear
        {'A', 0, 5, 400}, // [name]
        {'F', 0, 5, 400}, // Happy
        {'E', 0, 5, 400}, // Birthday
        {'D', 0, 5, 800}, // to you
    };


    // Play each note
    clear_display();

    for (;;) {
        for (int i = 0; i < sizeof(happy_birthday_notes) / sizeof(happy_birthday_notes[0]); i++) {
            play_note(happy_birthday_notes[i].note, 
                    happy_birthday_notes[i].flat, 
                    happy_birthday_notes[i].octave);

            // Wait for the specified duration
            nano_wait(happy_birthday_notes[i].duration * 1000000);

        }

    }

    set_freq(0,0);
#endif

// #define AMONG_US
#ifdef AMONG_US
// Define the notes for the "Among Us" theme
struct Note {
    char note;   // Note character ('A'-'G')
    int flat;    // 1 for sharp, 0 for natural
    int octave;  // Octave number (0-8)
    int duration; // Duration in milliseconds
};

struct Note among_us_notes[] = {
    {'C', 0, 5, 200}, // C
    {'D', 1, 5, 200}, // D#
    {'E', 0, 5, 200}, // E
    {'F', 1, 5, 200}, // F#
    {'E', 0, 5, 200}, // E
    {'D', 1, 5, 200}, // D#
    {'C', 0, 5, 200}, // C
    {'B', 1, 4, 200}, // Bb
    {'D', 0, 5, 200}, // D
    {'C', 0, 5, 200}, // C
    {'G', 0, 4, 200}, // G
    {'C', 0, 5, 400}, // C
    {'C', 0, 5, 200}, // C
    {'D', 1, 5, 200}, // D#
    {'E', 0, 5, 200}, // E
    {'F', 1, 5, 200}, // F#
    {'E', 0, 5, 200}, // E
    {'D', 1, 5, 200}, // D#
    {'E', 0, 5, 200}, // E
    {'E', 0, 5, 200}, // E
    {'D', 0, 5, 200}, // D
    {'C', 0, 5, 200}, // C
    {'E', 0, 5, 200}, // E
    {'D', 0, 5, 200}, // D
    {'C', 0, 5, 200}, // C
    {'B', 0, 4, 200}, // B
};

clear_display();

// Play each note
for (;;) {
    for (int i = 0; i < sizeof(among_us_notes) / sizeof(among_us_notes[0]); i++) {
        play_note(among_us_notes[i].note, 
                  among_us_notes[i].flat, 
                  among_us_notes[i].octave);

        // Wait for the specified duration
        nano_wait(among_us_notes[i].duration * 1000000);
    }
}

set_freq(0, 0);
#endif

    // Have fun.
    // dialer();
}