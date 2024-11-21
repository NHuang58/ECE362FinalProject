#ifndef SPI_FUNCTIONS_H
#define SPI_FUNCTIONS_H

void init_lcd_spi(void);
void init_lcd(void);
void test_lcd(void);
void init_spi1_slow(void);
void enable_sdcard(void);
void disable_sdcard(void);
void init_sdcard_io(void);
void sdcard_io_high_speed(void);


void Amir_init_lcd_spi(void);
void Amir_test_lcd(void);
void Amir_init_lcd(void);

#endif