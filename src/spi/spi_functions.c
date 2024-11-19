#include "stm32f0xx.h"

// Configure SPI and GPIO settings
void init_spi2_slow(){
    // SPI pins:
    // PB12 -> CS (NSS)
    // PB13 -> SCK
    // PB14 -> SDO (MISO)
    // PB15 -> SDI (MOSI)

    GPIOB->MODER &= ~0xff000000;
    GPIOB->MODER |= 0xaa000000;
    GPIOB->AFR[1] &= ~0xffff0000;

    // some more implementation pls

    SPI2->CR1 &= ~SPI_CR1_SPE;
    SPI2->CR1 |= SPI_CR1_MSTR
              | SPI_CR1_BR_2
              | SPI_CR1_BR_1
              | SPI_CR1_BR_0
              | SPI_CR1_SSI
              | SPI_CR1_SSM;

    SPI2->CR2 &= ~SPI_CR2_DS_3;
    SPI2->CR2 |= SPI_CR2_DS_2
              | SPI_CR2_DS_1
              | SPI_CR2_DS_0
              | SPI_CR2_FRXTH;

    SPI2->CR1 |= SPI_CR1_SPE;
}

void enable_sdcard(){
    GPIOB->BSRR |= 0x4;
}

void disable_sdcard(){
    GPIOB->BSRR |= 0x4 << 16;
}

void init_sdcard_io(){
    init_spi2_slow();
    GPIOB->MODER &= ~0x30;
    GPIOB->MODER |= 0x10;
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
