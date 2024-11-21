// Receive a character via SPI2
char SPI2_Receive_Char(void) {
    while (!(SPI2->SR & SPI_SR_RXNE));  // Wait for RXNE flag (Receive buffer not empty)
    return (char)(SPI2->DR);  // Return received character from the data register
}

void SPI2_Init(void) {
    // Enable clock to SPI2 and GPIOB (or appropriate GPIO port)
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;   // Enable SPI2 clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;  // Enable GPIOB clock (for pins PB13, PB14, PB15)

    // Set PB13, PB14, PB15 to alternate function for SPI2 (SCK, MISO, MOSI)
    GPIOB->MODER |= (0x2 << 2*13) | (0x2 << 2*14) | (0x2 << 2*15);  // Alternate function mode
    GPIOB->AFR[1] |= (0x5 << 4*13) | (0x5 << 4*14) | (0x5 << 4*15); // Set AF5 for SPI2

    // SPI2 configuration
    SPI2->CR1 |= SPI_CR1_MSTR   // Master mode (if SPI2 is the master)
              | SPI_CR1_BR_0   // Baud rate (adjust as needed)
              | SPI_CR1_MSBFirst  // MSB first
              | SPI_CR1_SSI   // Internal slave select
              | SPI_CR1_SPE;  // Enable SPI
}

