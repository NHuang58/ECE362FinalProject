#include "stm32f0xx.h"


void SPI_Init_Slave(void) {
    // Enable SPI2 and GPIOB clock (Assuming SPI2 is used on GPIOB)
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    // Configure SPI2 pins: PB14 -> SCK, PB15 -> MISO, PB13 -> MOSI
    GPIOB->MODER &= ~(GPIO_MODER_MODER13_Msk | GPIO_MODER_MODER14_Msk | GPIO_MODER_MODER15_Msk);
    GPIOB->MODER |= (GPIO_MODER_MODER13_1 | GPIO_MODER_MODER14_1 | GPIO_MODER_MODER15_1); // Alternate function
    GPIOB->AFR[1] |= (0x06 << (4*1)) | (0x06 << (4*2)) | (0x06 << (4*3)); // Set AF6 for SPI

    // Configure SPI in Slave mode, full-duplex, 8-bit, SCK idle low, MSB first
    SPI2->CR1 &= ~SPI_CR1_SPE;  // Disable SPI
    SPI2->CR1 |= SPI_CR1_SSI | SPI_CR1_SSM | SPI_CR1_SPE;  // Slave mode, Software slave management, Enable SPI
}

// Receive data using SPI
uint8_t SPI_Receive_Char(void) {
    while (!(SPI2->SR & SPI_SR_RXNE));  // Wait for RXNE (Receive buffer not empty)
    return SPI2->DR;  // Read data from SPI data register
}
