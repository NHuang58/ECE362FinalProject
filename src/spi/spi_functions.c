#include "stm32f0xx.h"

void init_spi2_slow(){
    // SPI pins:
    // PB12 -> CS (NSS)
    // PB13 -> SCK
    // PB14 -> SDO (MISO)
    // PB15 -> SDI (MOSI)
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

    GPIOB->MODER &= ~0xff000000;
    GPIOB->MODER |= 0xaa000000;
    GPIOB->AFR[1] &= ~0xffff0000;

    // some more implementation pls

    SPI2->CR1 &= ~SPI_CR1_SPE;
    SPI2->CR1 |= SPI_CR1_MSTR
              | SPI_CR1_SSI
              | SPI_CR1_SSM;
    SPI2->CR1 &= ~(SPI_CR1_BR);

    SPI2->CR2 &= ~SPI_CR2_DS_3;
    SPI2->CR2 |= SPI_CR2_DS_2
              | SPI_CR2_DS_1
              | SPI_CR2_DS_0
              | SPI_CR2_FRXTH;

    SPI2->CR1 |= SPI_CR1_SPE;
}

void enable_sdcard(){
    GPIOB->BSRR |= GPIO_BSRR_BR_11;
}

void disable_sdcard(){
    GPIOB->BSRR |= GPIO_BSRR_BS_11;
}

void init_sdcard_io(){
    init_SPI2_slow();
    GPIOB->MODER &= ~0x3 << 11;
    GPIOB->MODER |= 0x1 << 11;
    disable_sdcard();
}

void sdcard_io_high_speed(){
    SPI2->CR1 &= ~SPI_CR1_SPE;

    SPI2->CR1 &= ~SPI_CR1_BR_2
              & ~SPI_CR1_BR_1
              & ~SPI_CR1_BR_0;
    SPI2->CR1 |= SPI_CR1_BR_0; // 48MHz / 4 (001)

    SPI2->CR1 |= SPI_CR1_SPE;
}

void init_lcd_spi(){
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    GPIOB->MODER &= ~0x30000000;
    GPIOB->MODER |= 0x10000000;

    init_SPI2_slow();
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