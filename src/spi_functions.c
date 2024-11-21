#include "stm32f0xx.h"
#include "lcd.h"

void Amir_init_lcd_spi(void) {
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
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

    SPI1->CR1 &= ~SPI_CR1_SPE;
    
    SPI1->CR1 |= (SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0);
    
    SPI1->CR2 = SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;
    
    SPI1->CR1 |= SPI_CR1_MSTR;
    SPI1->CR2 |= SPI_CR2_SSOE;
    SPI1->CR1 |= SPI_CR1_SPE;
}

void Amir_init_lcd(void) {
    LCD_Setup();
    LCD_Clear(BLACK);
}

void Amir_test_lcd(void) {
    LCD_DrawString(10, 10, WHITE, BLACK, "Piano Display Test", 16, 0);
    
    LCD_DrawRectangle(10, 30, 230, 70, WHITE);
    
    for(int i = 0; i < 7; i++) {
        LCD_DrawFillRectangle(20 + (i * 30), 80, 45 + (i * 30), 160, WHITE);
        LCD_DrawRectangle(20 + (i * 30), 80, 45 + (i * 30), 160, BLUE);
    }
    
    for(int i = 0; i < 6; i++) {
        if(i != 2) {
            LCD_DrawFillRectangle(40 + (i * 30), 80, 55 + (i * 30), 130, BLACK);
        }
    }
    
    LCD_Circle(120, 200, 20, 1, RED);
    
    LCD_DrawString(10, 220, GREEN, BLACK, "Touch a key to play", 12, 0);
}


void init_lcd_spi(void) {
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
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

    SPI1->CR1 &= ~SPI_CR1_SPE;
    
    SPI1->CR1 |= (SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0);
    
    SPI1->CR2 = SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;
    
    SPI1->CR1 |= SPI_CR1_MSTR;
    SPI1->CR2 |= SPI_CR2_SSOE;
    SPI1->CR1 |= SPI_CR1_SPE;
}

void init_lcd(void) {
    LCD_Setup();
    LCD_Clear(BLACK);
}

void test_lcd(void) {
    LCD_DrawString(10, 10, WHITE, BLACK, "Piano Display Test", 16, 0);
    
    LCD_DrawRectangle(10, 30, 230, 70, WHITE);
    
    for(int i = 0; i < 7; i++) {
        LCD_DrawFillRectangle(20 + (i * 30), 80, 45 + (i * 30), 160, WHITE);
        LCD_DrawRectangle(20 + (i * 30), 80, 45 + (i * 30), 160, BLUE);
    }
    
    for(int i = 0; i < 6; i++) {
        if(i != 2) {
            LCD_DrawFillRectangle(40 + (i * 30), 80, 55 + (i * 30), 130, BLACK);
        }
    }
    
    LCD_Circle(120, 200, 20, 1, RED);
    
    LCD_DrawString(10, 220, GREEN, BLACK, "Touch a key to play", 12, 0);
}

void init_spi1_slow(){
    // SPI pins:
    // PB2 -> CS (NSS)
    // PB3 -> SCK
    // PB4 -> SDO (MISO)
    // PB5 -> SDI (MOSI)
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    GPIOB->MODER &= ~0xff0;
    GPIOB->MODER |= 0xa90;
    GPIOB->AFR[0] &= ~0xfff000;

    // some more implementation pls

    SPI1->CR1 &= ~SPI_CR1_SPE;
    SPI1->CR1 |= SPI_CR1_MSTR
              | SPI_CR1_SSI
              | SPI_CR1_SSM;
    SPI1->CR1 &= ~(SPI_CR1_BR);

    SPI1->CR2 &= ~SPI_CR2_DS_3;
    SPI1->CR2 |= SPI_CR2_DS_2
              | SPI_CR2_DS_1
              | SPI_CR2_DS_0
              | SPI_CR2_FRXTH;

    SPI1->CR1 |= SPI_CR1_SPE;
}

void enable_sdcard(){
    GPIOB->BSRR |= GPIO_BSRR_BR_2;
}

void disable_sdcard(){
    GPIOB->BSRR |= GPIO_BSRR_BS_2;
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

/* void init_lcd_spi(){
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    GPIOB->MODER &= ~0x30c30000;
    GPIOB->MODER |= 0x10410000;

    init_spi1_slow();
    sdcard_io_high_speed();
}
 */