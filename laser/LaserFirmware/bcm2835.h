#pragma once
#define BCM2835_GPIO_FSEL_OUTP 1
#define BCM2835_SPI_BIT_ORDER_MSBFIRST 1
#define BCM2835_SPI_CLOCK_DIVIDER_32 1
#define BCM2835_SPI_CS1 1
#define BCM2835_SPI_MODE1 1
#define HIGH 1
#define LOW 0
#define RPI_GPIO_P1_15 1

void bcm2835_spi_end(){}
void bcm2835_close(){}
void bcm2835_gpio_fsel(int, int){}
void bcm2835_gpio_write(int, int){}
void bcm2835_spi_begin(){}
void bcm2835_spi_setBitOrder(int){}
void bcm2835_spi_setDataMode(int){}
void bcm2835_spi_setClockDivider(int){}
void bcm2835_spi_chipSelect(int){}
void bcm2835_spi_setChipSelectPolarity(int, int){}
bool bcm2835_init(){return true;}
void bcm2835_spi_transfern(char*, int){}
