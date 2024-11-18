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

}
