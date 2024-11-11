/**
  ******************************************************************************
  * @file    main.c
  * @author  Weili An, Niraj Menon
  * @date    Jan 31 2024
  * @brief   ECE 362 Lab 5 Student template
  ******************************************************************************
*/

/*******************************************************************************/

// Fill out your username, otherwise your completion code will have the 
// wrong username!
const char* username = "huan1811";

/*******************************************************************************/ 

#include "stm32f0xx.h"
#include <math.h>   // for M_PI

void nano_wait(int);

// 16-bits per digit.
// The most significant 8 bits are the digit number.
// The least significant 8 bits are the segments to illuminate.
uint16_t msg[8] = { 0x0000,0x0100,0x0200,0x0300,0x0400,0x0500,0x0600,0x0700 };
extern const char font[];
// Print an 8-character string on the 8 digits
void print(const char str[]);
// Print a floating-point value.
void printfloat(float f);

void autotest(void);

//============================================================================
// PWM Lab Functions
//============================================================================
void setup_tim3(void) {
    //enable clock for TIM3
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    //enable clock for GPIOC
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN; 

    //set mode to alternate function (6-9)
    GPIOC->MODER &= 0xfff00fff;
    GPIOC->MODER |= 0xaa000;
    
    //prescaler
    TIM3->PSC = (48000 - 1);
    //reload
    TIM3->ARR = (1000 - 1);
    
    //PWM mode 1
    TIM3->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2;
    TIM3->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;
    TIM3->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;
    TIM3->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;

    //channel outputs
    TIM3->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;

    //enable
    TIM3->CR1 |= TIM_CR1_CEN;
    
    //CCRx registers
    TIM3->CCR1 = 800;
    TIM3->CCR2 = 600;
    TIM3->CCR3 = 400;
    TIM3->CCR4 = 200;
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

    // TIM1->CCMR2 |= (TIM_CCMR2_OC4M_0 | TIM_CCMR2_OC3M_0);
    // TIM1->CCMR1 |= (TIM_CCMR1_OC2M_0 | TIM_CCMR1_OC1M_0);

    TIM1->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2;
    TIM1->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;
    TIM1->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;
    TIM1->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;

    TIM1->CCMR2 |= TIM_CCMR2_OC4PE;

    TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;

    TIM1->CR1 |= TIM_CR1_CEN;
}

int getrgb(void);

// Helper function for you
// Accept a byte in BCD format and convert it to decimal
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

//============================================================================
// Lab 4 code
// Add in your functions from previous lab
//============================================================================

// Part 3: Analog-to-digital conversion for a volume level.
uint32_t volume = 2400;

// Variables for boxcar averaging.
#define BCSIZE 32
int bcsum = 0;
int boxcar[BCSIZE];
int bcn = 0;

void dialer(void);

// Parameters for the wavetable size and expected synthesis rate.
#define N 1000
#define RATE 20000
short int wavetable[N];
int step0 = 0;
int offset0 = 0;
int step1 = 0;
int offset1 = 0;

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
    
    // samp *= volume;
    // samp = (samp >> 17);
    // samp += 2048;

    samp = (((samp * volume))>>18) + 1200;
    
    //replace
    // samp = (((samp * volume) & 0xfff)>>18) + 1200;
    
    //replace
    // DAC->DHR12R1 = (uint16_t) samp;
    TIM1->CCR4 = (uint16_t) samp;

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
    //Turn off TRGO generation

    // TIM6->CR2 |= TIM_CR2_MMS_1;    
    TIM6->CR1 |= TIM_CR1_CEN;
}

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
// All the things you need to test your subroutines.
//============================================================================
int main(void) {
    internal_clock();

    // Uncomment autotest to get the confirmation code.
    // autotest();

    // Demonstrate part 1
// #define TEST_TIMER3
#ifdef TEST_TIMER3
    setup_tim3();
    for(;;) { }
#endif

    // Initialize the display to something interesting to get started.
    msg[0] |= font['E'];
    msg[1] |= font['C'];
    msg[2] |= font['E'];
    msg[3] |= font[' '];
    msg[4] |= font['3'];
    msg[5] |= font['6'];
    msg[6] |= font['2'];
    msg[7] |= font[' '];

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

    // demonstrate part 2
// #define TEST_TIM1
#ifdef TEST_TIM1
    for(;;) {
        // Breathe in...
        for(float x=1; x<2400; x *= 1.1) {
            TIM1->CCR1 = TIM1->CCR2 = TIM1->CCR3 = 2400-x;
            nano_wait(100000000);
        }
        // ...and out...
        for(float x=2400; x>=1; x /= 1.1) {
            TIM1->CCR1 = TIM1->CCR2 = TIM1->CCR3 = 2400-x;
            nano_wait(100000000);
        }
        // ...and start over.
    }
#endif

    // demonstrate part 3
#define PIANO
#ifdef PIANO
    for (;;) {
        char key = get_keypress();
        float freq = 0; // Initialize frequency variable

        switch (key) {
            // SET NOTE TO C (Middle C = 261.63 Hz)
            case '1': 
                freq = 261.63;
                break;
            // SET NOTE TO C# / Db (C#4/Db4 = 277.18 Hz)
            case '2': 
                freq = 277.18;
                break;
            // SET NOTE TO D (D4 = 293.66 Hz)
            case '3': 
                freq = 293.66;
                break;
            // SET NOTE TO D# / Eb (D#4/Eb4 = 311.13 Hz)
            case 'A': 
                freq = 311.13;
                break;
            // SET NOTE TO E (E4 = 329.63 Hz)
            case '4': 
                freq = 329.63;
                break;
            // SET NOTE TO F (F4 = 349.23 Hz)
            case '5': 
                freq = 349.23;
                break;
            // SET NOTE TO F# / Gb (F#4/Gb4 = 369.99 Hz)
            case '6': 
                freq = 369.99;
                break;
            // SET NOTE TO G (G4 = 392.00 Hz)
            case 'B': 
                freq = 392.00;
                break;
            // SET NOTE TO G# / Ab (G#4/Ab4 = 415.30 Hz)
            case '7': 
                freq = 415.30;
                break;
            // SET NOTE TO A (A4 = 440.00 Hz)
            case '8': 
                freq = 440.00;
                break;
            // SET NOTE TO A# / Bb (A#4/Bb4 = 466.16 Hz)
            case '9': 
                freq = 466.16;
                break;
            // SET NOTE TO B (B4 = 493.88 Hz)
            case 'C': 
                freq = 493.88;
                break;
            // SCALE UP BY 1 OCTAVE
            case '*':
                freq *= 2;
                continue;
            // SCALE DOWN BY 1 OCTAVE
            case '#':
                freq /= 2;
                continue;

            default:
                continue;
        }

        set_freq(0, freq);
    }
#endif


// #ifdef MIX_TONES
//     set_freq(0, 1000);
//     for(;;) {
//         char key = get_keypress();
//         if (key == 'A')
//             set_freq(0,getfloat());
//         if (key == 'B')
//             set_freq(1,getfloat());
//     }
// #endif

    // demonstrate part 4
// #define TEST_SETRGB
#ifdef TEST_SETRGB
    for(;;) {
        char key = get_keypress();
        if (key == 'A')
            set_freq(0,getfloat());
        if (key == 'B')
            set_freq(1,getfloat());
        if (key == 'D')
            setrgb(getrgb());
    }
#endif

    // Have fun.
    dialer();
}