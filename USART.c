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
