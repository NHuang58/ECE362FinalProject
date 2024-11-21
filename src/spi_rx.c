void SPI1_Init(void) {
    // Enable clock to SPI1 and GPIOA (or appropriate GPIO port)
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;   // Enable SPI1 clock
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  // Enable GPIOA clock (for pins PA5, PA6, PA7)

    // Set PA5, PA6, PA7 to alternate function for SPI (SCK, MISO, MOSI)
    GPIOA->MODER |= (0x2 << 2*5) | (0x2 << 2*6) | (0x2 << 2*7);  // Alternate function mode
    GPIOA->AFR[0] |= (0x5 << 4*5) | (0x5 << 4*6) | (0x5 << 4*7); // Set AF5 for SPI1

    // SPI configuration
    SPI1->CR1 |= SPI_CR1_MSTR   // Master mode
              |  SPI_CR1_BR_0   // Baud rate (adjust as needed)
              |  SPI_CR1_SSI   // Internal slave select
              |  SPI_CR1_SPE;  // Enable SPI
}

char SPI1_Receive_Char(void) {
    while (!(SPI1->SR & SPI_SR_RXNE));  // Wait for RXNE flag (Receive buffer not empty)
    return (char)(SPI1->DR);  // Return received character from the data register
}
