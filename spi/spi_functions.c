#include "stm32f0xx.h"
#include "lcd.h"



// Seven Segment Display (SPI2)
// Uses PB12 (CS), PB13 (SCK), PB15 (MOSI)

// LCD Display (SPI1)
// Uses PA5 (SCK), PA7 (MOSI)
// PB8 (CS), PB11 (RESET), PB14 (DC)

// SD Card (SPI1)
// Can share SPI1 with LCD but needs its own CS pin
// Uses PA5 (SCK), PA7 (MOSI), PA6 (MISO)
// Need to add a new CS pin for SD card, e.g., PB6


void enable_sdcard() {
    GPIOB->BSRR = GPIO_BSRR_BR_9;
}

void disable_sdcard() {
    GPIOB->BSRR = GPIO_BSRR_BS_9;
}

void sdcard_io_high_speed() {
    SPI2->CR1 &= ~SPI_CR1_SPE;
    SPI2->CR1 &= ~(SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0);
    SPI2->CR1 |= SPI_CR1_BR_0;
    SPI2->CR1 |= SPI_CR1_SPE;
}

void init_lcd_spi() {
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    GPIOA->MODER &= ~(GPIO_MODER_MODER5 | GPIO_MODER_MODER7);
    GPIOA->MODER |= (GPIO_MODER_MODER5_1 | GPIO_MODER_MODER7_1);
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL5 | GPIO_AFRL_AFSEL7);
    GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR5 | GPIO_OSPEEDR_OSPEEDR7);

    GPIOB->MODER &= ~(GPIO_MODER_MODER8 | GPIO_MODER_MODER11 | GPIO_MODER_MODER14);
    GPIOB->MODER |= (GPIO_MODER_MODER8_0 | GPIO_MODER_MODER11_0 | GPIO_MODER_MODER14_0);
    GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR8 | GPIO_OSPEEDR_OSPEEDR11 | GPIO_OSPEEDR_OSPEEDR14);
    GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_8 | GPIO_OTYPER_OT_11 | GPIO_OTYPER_OT_14);
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR8 | GPIO_PUPDR_PUPDR11 | GPIO_PUPDR_PUPDR14);
    GPIOB->PUPDR |= (GPIO_PUPDR_PUPDR8_0 | GPIO_PUPDR_PUPDR11_0 | GPIO_PUPDR_PUPDR14_0);

    GPIOB->MODER &= ~(GPIO_MODER_MODER12 | GPIO_MODER_MODER13 | 
                      GPIO_MODER_MODER14 | GPIO_MODER_MODER15);
    GPIOB->MODER |= (GPIO_MODER_MODER12_1 | GPIO_MODER_MODER13_1 | 
                     GPIO_MODER_MODER14_1 | GPIO_MODER_MODER15_1);
    GPIOB->AFR[1] &= ~(GPIO_AFRH_AFSEL12 | GPIO_AFRH_AFSEL13 | 
                       GPIO_AFRH_AFSEL14 | GPIO_AFRH_AFSEL15);

    GPIOB->MODER &= ~GPIO_MODER_MODER9;
    GPIOB->MODER |= GPIO_MODER_MODER9_0;
    disable_sdcard();

    SPI1->CR1 &= ~SPI_CR1_SPE;
    SPI1->CR1 |= (SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0);
    SPI1->CR2 = SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;
    SPI1->CR1 |= SPI_CR1_MSTR;
    SPI1->CR2 |= SPI_CR2_SSOE;
    SPI1->CR1 |= SPI_CR1_SPE;

    SPI2->CR1 &= ~SPI_CR1_SPE;
    SPI2->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM;
    SPI2->CR1 |= (SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0);
    SPI2->CR2 = SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0 | SPI_CR2_FRXTH;
    SPI2->CR1 |= SPI_CR1_SPE;

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