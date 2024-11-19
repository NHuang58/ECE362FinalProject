#include "stm32f0xx.h"

// Configure SPI and GPIO settings
void init_spi1_slow(){
    // SPI pins:
    // PB2 -> CS (NSS)
    // PB3 -> SCK
    // PB4 -> SDO (MISO)
    // PB5 -> SDI (MOSI)

    GPIOB->MODER &= ~0xff0;
    GPIOB->MODER |= 0xa80;
    GPIOB->AFR[0] &= ~0xfff000;

    // some more implementation pls

    SPI1->CR1 &= ~SPI_CR1_SPE;
    SPI1->CR1 |= SPI_CR1_MSTR
              | SPI_CR1_BR_2
              | SPI_CR1_BR_1
              | SPI_CR1_BR_0
              | SPI_CR1_SSI
              | SPI_CR1_SSM;

    SPI1->CR2 &= ~SPI_CR2_DS_3;
    SPI1->CR2 |= SPI_CR2_DS_2
              | SPI_CR2_DS_1
              | SPI_CR2_DS_0
              | SPI_CR2_FRXTH;

    SPI1->CR1 |= SPI_CR1_SPE;
}

void enable_sdcard(){
    GPIOB->BSRR |= 0x4;
}

void disable_sdcard(){
    GPIOB->BSRR |= 0x4 << 16;
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

    init_spi1_slow();
    sdcard_io_high_speed();
}
