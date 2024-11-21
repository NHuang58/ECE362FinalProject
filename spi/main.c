/**
  ******************************************************************************
  * @file    main.c
  * @author  Weili An, Niraj Menon
  * @date    Feb 7, 2024
  * @brief   ECE 362 Lab 7 student template
  ******************************************************************************
*/

/*******************************************************************************/

// Fill out your username!  Even though we're not using an autotest, 
// it should be a habit to fill out your username in this field now.
const char* username = "huan1811";

/*******************************************************************************/ 

#include "stm32f0xx.h"
#include <stdint.h>
#include "lcd.h"

void internal_clock();

// Uncomment only one of the following to test each step

// #define STEP1
// #define STEP2
// #define STEP3
// #define STEP4
#define SHELL

void init_usart5() {
    // TODO

    // Enable the RCC clocks to GPIOC and GPIOD.
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    RCC->AHBENR |= RCC_AHBENR_GPIODEN;

    //Do all the steps necessary to configure pin PC12 to be routed to USART5_TX.
    GPIOC->MODER  &= ~(0x3 << (2 * 12)); //Clear port 12
    
    GPIOC->MODER  |= (0x2 << (2 * 12)); //Set PC12 to alternate function
    GPIOC->AFR[1] |= (0x2 << 16); //Shift over by 16

    //Do all the steps necessary to configure pin PD2 to be routed to USART5_RX.
    GPIOD->MODER  &= (0x3 << (2 * 2)); //Clear port 2
    
    GPIOD->MODER  |= (0x2 << (2 * 2)); //Set PD2 to alternate function
    GPIOD->AFR[0] |= (0x2 << 8); //Shift over by 8

    //Enable the RCC clock to the USART5 peripheral.
    RCC->APB1ENR |= RCC_APB1ENR_USART5EN;

    //Configure USART5 as follows:
    USART5->CR1 &= ~USART_CR1_UE; //(First, disable it by turning off its UE bit.)
    USART5->CR1 &= ~USART_CR1_M;

    //Set it for one stop bit.
    USART5->CR2 &= ~USART_CR2_STOP; //CR2

    //Set it for no parity control.
    USART5->CR1 &= ~USART_CR1_PCE;

    //Use 16x oversampling.
    USART5->CR1 &= ~USART_CR1_OVER8;

    //Use a baud rate of 115200 (115.2 kbaud). Refer to table 96 of the Family Reference Manual, or simply divide the system clock rate by 115200. 
    USART5->BRR = (0x1A1);

    //Enable the transmitter and the receiver by setting the TE and RE bits.
    USART5->CR1 |= USART_CR1_TE;
    USART5->CR1 |= USART_CR1_RE;

    //Enable the USART.
    USART5->CR1 |= USART_CR1_UE;

    //Finally, you should wait for the TE and RE bits to be acknowledged by checking that TEACK and REACK bits are both set in the ISR. This indicates that the USART is ready to transmit and receive.
    while (! ( (USART5->ISR) & (USART_ISR_TEACK | USART_ISR_REACK))){};
}

#ifdef STEP1
int main(void){
    internal_clock();
    init_usart5();
    for(;;) {
        while (!(USART5->ISR & USART_ISR_RXNE)) { }
        char c = USART5->RDR;
        while(!(USART5->ISR & USART_ISR_TXE)) { }
        USART5->TDR = c;
    }
}
#endif

#ifdef STEP2
#include <stdio.h>

// TODO Resolve the echo and carriage-return problem

int __io_putchar(int c) {
    // TODO

    if (c == '\n') {
        //You must wait for the TXE flag to be set before writing any character to TDR.
        while(!(USART5->ISR & USART_ISR_TXE));
        USART5->TDR = '\r';
    }

    //Same
    while(!(USART5->ISR & USART_ISR_TXE));
    USART5->TDR = c;
    return c;
}

int __io_getchar(void) {
    //Same
    while (!(USART5->ISR & USART_ISR_RXNE));
    char c = USART5->RDR;
    // TODO
    
    if (c == '\r') {
        c = '\n';
    }

    __io_putchar(c);
    // __io_putchar((int)c);
    return c;
}

int main() {
    internal_clock();
    init_usart5();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    printf("Enter your name: ");
    char name[80];
    fgets(name, 80, stdin);
    printf("Your name is %s", name);
    printf("Type any characters.\n");
    for(;;) {
        char c = getchar();
        putchar(c);
    }
}
#endif

#ifdef STEP3
#include <stdio.h>
#include "fifo.h"
#include "tty.h"
int __io_putchar(int c) {
    // TODO Copy from your STEP2

    if (c == '\n') {
        //You must wait for the TXE flag to be set before writing any character to TDR.
        while(!(USART5->ISR & USART_ISR_TXE));
        USART5->TDR = '\r';
    }

    //Same
    while(!(USART5->ISR & USART_ISR_TXE));
    USART5->TDR = c;
    return c;
}

int __io_getchar(void) {
    // TODO
    return line_buffer_getchar();
}

int main() {
    internal_clock();
    init_usart5();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    printf("Enter your name: ");
    char name[80];
    fgets(name, 80, stdin);
    printf("Your name is %s", name);
    printf("Type any characters.\n");
    for(;;) {
        char c = getchar();
        putchar(c);
    }
}
#endif

// #ifdef STEP4

#include <stdio.h>
#include "fifo.h"
#include "tty.h"

// TODO DMA data structures
#define FIFOSIZE 16
char serfifo[FIFOSIZE];
int seroffset = 0;

void enable_tty_interrupt(void) {
    // TODO
    RCC->AHBENR |= RCC_AHBENR_DMA2EN;
    DMA2->CSELR |= DMA2_CSELR_CH2_USART5_RX;
    
    DMA2_Channel2->CCR &= ~DMA_CCR_EN;

    //Raise an interrupt every time the receive data register becomes not empty
    USART5->CR1 |= USART_CR1_RXNEIE; 

    //Remember to set the proper bit in the NVIC ISER
    NVIC->ISER[0] |= (1 << USART3_8_IRQn);
    RCC->AHBENR |= RCC_AHBENR_DMA2EN; //Enable DMA2
    USART5->CR3 |= (USART_CR3_DMAR); //Enable DMAR

    //Trigger a DMA operation every time the receive data register becomes not empty
    DMA2->CSELR |= DMA2_CSELR_CH2_USART5_RX;

    //CMAR should be set to the address of serfifo.
    DMA2_Channel2->CMAR = (uint32_t) serfifo;

    //CPAR should be set to the address of the USART5->RDR.
    DMA2_Channel2->CPAR = (uint32_t) &USART5->RDR;

    //CNDTR should be set to FIFOSIZE.
    DMA2_Channel2->CNDTR = FIFOSIZE;

    //The DIRection of copying should be from peripheral to memory.
    DMA2_Channel2->CCR &= ~DMA_CCR_DIR;

    //Neither the total-completion nor the half-transfer interrupt should be enabled.
    // DMA2_Channel2->CCR = DMA_CCR_HTIE;
    DMA2_Channel2->CCR |= DMA_CCR_TCIE;

    //Both the MSIZE and the PSIZE should be set for 8 bits.
    DMA2_Channel2->CCR &= ~DMA_CCR_MSIZE;
    DMA2_Channel2->CCR &= ~DMA_CCR_PSIZE;

    //MINC should be set to increment the CMAR.
    DMA2_Channel2->CCR |= DMA_CCR_MINC;

    //PINC should not be set so that CPAR always points at the USART5->RDR.
    DMA2_Channel2->CCR &= ~DMA_CCR_PINC;

    //Enable CIRCular transfers.
    DMA2_Channel2->CCR |= DMA_CCR_CIRC;

    //Do not enable MEM2MEM transfers.
    DMA2_Channel2->CCR &= ~(DMA_CCR_MEM2MEM);

    //Set the channel priority Level to highest.
    // DMA2_Channel2->CCR |= DMA_CCR_PL;
    DMA2_Channel2->CCR |= (1 );

    //Finally, make sure that the channel is enabled for operation.
    DMA2_Channel2->CCR |= DMA_CCR_EN;
}

// Works like line_buffer_getchar(), but does not check or clear ORE nor wait on new characters in USART
char interrupt_getchar() {
    // TODO

    while(fifo_newline(&input_fifo) == 0) {
        asm volatile ("wfi"); // wait for an interrupt
        // while (!(u->ISR & USART_ISR_RXNE))
        //     ;
        // insert_echo_char(u->RDR);
    }

    // Return a character from the line buffer.
    char ch = fifo_remove(&input_fifo);
    return ch;
}

int __io_putchar(int c) {
        // TODO

    if (c == '\n') {
        //You must wait for the TXE flag to be set before writing any character to TDR.
        while(!(USART5->ISR & USART_ISR_TXE));
        USART5->TDR = '\r';
    }

    //Same
    while(!(USART5->ISR & USART_ISR_TXE));
    USART5->TDR = c;
    return c;
    // return ();
}

int __io_getchar(void) {
    // TODO Use interrupt_getchar() instead of line_buffer_getchar()
    return interrupt_getchar();
}

// TODO Copy the content for the USART5 ISR here
void USART3_8_IRQHandler(void) {
    
    while(DMA2_Channel2->CNDTR != sizeof serfifo - seroffset) {
        if (!fifo_full(&input_fifo))
            insert_echo_char(serfifo[seroffset]);
        seroffset = (seroffset + 1) % sizeof serfifo;
    }
}
// TODO Remember to look up for the proper name of the ISR function


// int main() {
//     internal_clock();
//     init_usart5();
//     //Added
//     enable_tty_interrupt();

//     setbuf(stdin,0); // These turn off buffering; more efficient, but makes it hard to explain why first 1023 characters not dispalyed
//     setbuf(stdout,0);
//     setbuf(stderr,0);
//     printf("Enter your name: "); // Types name but shouldn't echo the characters; USE CTRL-J to finish
//     char name[80];
//     fgets(name, 80, stdin);
//     printf("Your name is %s", name);
//     printf("Type any characters.\n"); // After, will type TWO instead of ONE
//     for(;;) {
//         char c = getchar();
//         putchar(c);
//     }
// }
// #endif

#ifdef SHELL
#include "commands.h"
#include <stdio.h>
void init_spi1_slow(){
    // SPI pins:
    // PB2 -> CS (NSS)
    // PB3 -> SCK
    // PB4 -> SDO (MISO)
    // PB5 -> SDI (MOSI)
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    GPIOB->MODER &= ~0xff0;
    GPIOB->MODER |= 0xa90;
    GPIOB->AFR[0] &= ~0xfff000;

    // some more implementation pls

    SPI1->CR1 &= ~SPI_CR1_SPE;
    SPI1->CR1 |= SPI_CR1_MSTR
              | SPI_CR1_SSI
              | SPI_CR1_SSM;
    SPI1->CR1 &= ~(SPI_CR1_BR);

    SPI1->CR2 &= ~SPI_CR2_DS_3;
    SPI1->CR2 |= SPI_CR2_DS_2
              | SPI_CR2_DS_1
              | SPI_CR2_DS_0
              | SPI_CR2_FRXTH;

    SPI1->CR1 |= SPI_CR1_SPE;
}

void enable_sdcard(){
    GPIOB->BSRR |= GPIO_BSRR_BR_2;
}

void disable_sdcard(){
    GPIOB->BSRR |= GPIO_BSRR_BS_2;
}

void init_sdcard_io(){
    init_spi1_slow();
    GPIOB->MODER &= ~0x30;
    GPIOB->MODER |= 0x10;
    disable_sdcard();
}

void sdcard_io_high_speed(){
    SPI1->CR1 &= ~SPI_CR1_SPE;

    SPI1->CR1 &= ~SPI_CR1_BR_2
              & ~SPI_CR1_BR_1
              & ~SPI_CR1_BR_0;
    SPI1->CR1 |= SPI_CR1_BR_0; // 48MHz / 4 (001)

    SPI1->CR1 |= SPI_CR1_SPE;
}

void init_lcd_spi(){
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    GPIOB->MODER &= ~0x30c30000;
    GPIOB->MODER |= 0x10410000;

    init_spi1_slow();
    sdcard_io_high_speed();
}

void drawStaff(){
    LCD_DrawFillRectangle(0,0,239,319,0xffffff);
    LCD_DrawFillRectangle(70,40,75,280,0x0);
    LCD_DrawFillRectangle(100,40,105,280,0x0);
    LCD_DrawFillRectangle(130,40,135,280,0x0);
    LCD_DrawFillRectangle(160,40,165,280,0x0);
    LCD_DrawFillRectangle(190,40,195,280,0x0);
}

void drawNote(char note){
    drawStaff();
    int x1 = 80;
    int x2 = 95;
    int x3 = 30;
    int x4 = 90;

    int shift;
    int sharp;

    switch (note) {
        case 'f': shift = 0;
        sharp = 0;
        break;
        case 'F': shift = 0;
        sharp = 1;
        break;
        case 'g': shift = 15;
        sharp = 0;
        break;
        case 'G': shift = 15;
        sharp = 1;
        break;
        case 'a': shift = 30;
        break;
        case 'A': shift = 30;
        sharp = 1;
        break;
        case 'b': shift = 45;
        sharp = 0;
        break;
        case 'c': shift = 60;
        sharp = 0;
        break;
        case 'C': shift = 60;
        sharp = 1;
        break;
        case 'd': shift = 75;
        sharp = 0;
        break;
        case 'D': shift = 75;
        sharp = 1;
        break;
        case 'e': shift = 90;
        sharp = 0;
        break;
        default: shift = 150;
        sharp = 1;
        break;
    }

    x1 += shift;
    x2 += shift;
    x3 += shift;
    x4 += shift;
    
    LCD_DrawFillRectangle(x1,95,x2,110,0x0);
    LCD_DrawFillRectangle(x3,90,x4,95,0x0);
    LCD_DrawFillTriangle(x4,90,x4,95,x2,95,0);

    if(sharp){
        LCD_DrawFillRectangle(x1,65,x1 + 30,70,0xff);
        LCD_DrawFillRectangle(x1,75,x1 + 30,80,0xff);
        LCD_DrawFillRectangle(x1+5,55,x1 + 10,85,0xff);
        LCD_DrawFillRectangle(x1 + 15, 55,x1 + 20,85,0xff);
    }

    LCD_DrawChar(30, 200, 0, 0xffffff, 'a', 16, 1);
}

char USART2_Receive(void) {
    while (!(USART2->ISR & USART_ISR_RXNE)); // Wait until RX buffer is not empty
    return (char)USART2->RDR; // Return received character
}

void USART2_Init(uint32_t baud_rate) {
    // Enable the clock for GPIOA and USART2
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;   // Enable GPIOA clock
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN; // Enable USART2 clock

    // Configure PA2 (TX) and PA3 (RX) as alternate function
    GPIOA->MODER &= ~(GPIO_MODER_MODER2 | GPIO_MODER_MODER3); // Clear mode
    GPIOA->MODER |= GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1; // Set to AF
    GPIOA->AFR[0] |= (1 << GPIO_AFRL_AFSEL2_Pos) | (1 << GPIO_AFRL_AFSEL3_Pos); // Set AF1 for PA2 and PA3

    // Configure USART2
    USART2->BRR = SystemCoreClock / baud_rate; // Set baud rate
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE; // Enable TX, RX, and USART

    // Wait for USART to be ready
    while (!(USART2->ISR & USART_ISR_TEACK)); // Wait for TX ready
    while (!(USART2->ISR & USART_ISR_REACK)); // Wait for RX ready
}


int main() {
    internal_clock();
    // init_usart5();
    // enable_tty_interrupt();
    // setbuf(stdin,0);
    // setbuf(stdout,0);
    // setbuf(stderr,0);
    // init_lcd_spi();
    LCD_Setup();
    USART2_Init(4800);

    while(1){
        drawNote(USART2_Receive());
    }
}
#endif
