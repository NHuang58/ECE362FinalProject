void init_uart_receiver(void) {
    // Enable USART1 and GPIOA clocks
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;  // Enable USART1 clock
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;      // Enable GPIOA clock

    // Configure PA10 as USART1 RX (Alternate Function)
    GPIOA->MODER &= ~GPIO_MODER_MODE10;           // Clear the current mode for PA10
    GPIOA->MODER |= GPIO_MODER_MODE10_1;          // Set PA10 to alternate function mode
    GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL10;          // Clear the alternate function bits for PA10
    GPIOA->AFR[1] |= 0x01;                       // Set PA10 to USART1_RX (AF1)

    // Set the baud rate (assuming 48 MHz system clock)
    USART1->BRR = 0x683;  // Baud rate 9600

    // Configure USART1: 8 data bits, no parity, 1 stop bit
    USART1->CR1 &= ~USART_CR1_M;       // 8 data bits
    USART1->CR1 &= ~USART_CR1_PS;      // No parity
    USART1->CR1 &= ~USART_CR1_OVER8;   // 16x oversampling mode
    USART1->CR2 &= ~USART_CR2_STOP;    // 1 stop bit

    // Enable USART1 for receiving
    USART1->CR1 |= USART_CR1_RE;      // Enable receiver
    USART1->CR1 |= USART_CR1_UE;      // Enable USART1
}

// Function to receive a character
char receive_char(void) {
    while (!(USART1->SR & USART_SR_RXNE));  // Wait until data is received
    return (char)(USART1->DR);  // Return the received character
}

int main(void) {
    // Initialize USART receiver
    init_uart_receiver();

    while (1) {
        // Receive a character from USART1
        char received_char = receive_char();

        // Process the received character (for example, echo it)
        // Here we can either process it or store it
    }
}
