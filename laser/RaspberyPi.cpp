#include "RaspberryPi.h"
#include <bcm2835.h>
//--------------------------------------------------------------------------------
#define PIN RPI_GPIO_P1_15
//--------------------------------------------------------------------------------
bool RaspberryPi::InitializePi()
{
	// If you call this, it will not actually access the GPIO
	// Use for testing
	// bcm2835_set_debug(1);
	if (!bcm2835_init())
		return false;

	// Start SPI operations. Forces RPi SPI0 pins P1-19 (MOSI), P1-21 (MISO), P1-23 (CLK), P1-24 (CE0) and P1-26 (CE1) to alternate function ALT0, which enables those pins for SPI interface. 
	bcm2835_spi_begin();

	// Sets the SPI bit order NOTE: has no effect. Not supported by SPI0. 
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);

	// Sets the SPI data mode Sets the clock polariy and phase
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE1); 

	// Sets the SPI clock divider and therefore the SPI clock speed.
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_32); //About 32 is 8 MHz
	//	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256); //About 32 is 8 MHz

	// Sets the chip select pin(s) When an bcm2835_spi_transfer() is made, the selected pin(s) will be asserted during the transfer.
	bcm2835_spi_chipSelect(BCM2835_SPI_CS1);

	//Sets the chip select pin polarity for a given pin When an bcm2835_spi_transfer() occurs, the currently selected chip select pin(s) will be asserted to the value given by active. 
	//When transfers are not happening, the chip select pin(s) return to the complement (inactive) value.
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, LOW);

	// Set the pin to be an output
	bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_OUTP);

	return true;
}
//--------------------------------------------------------------------------------
void RaspberryPi::ShutdownPi()
{
	bcm2835_spi_end();
	bcm2835_close();
}
//--------------------------------------------------------------------------------
void RaspberryPi::SetLaser(bool on)
{
	bcm2835_gpio_write(PIN, on ? HIGH : LOW);
}
//--------------------------------------------------------------------------------
void RaspberryPi::SetXY(uint16_t x, uint16_t y)
{
	DACCommand(0, 0, x);
	DACCommand(2, 1, y);
}
//--------------------------------------------------------------------------------
void RaspberryPi::SetX(uint16_t x)
{
	DACCommand(3, 0, x);
}
//--------------------------------------------------------------------------------
void RaspberryPi::SetY(uint16_t y)
{
	DACCommand(3, 1, y);
}
//--------------------------------------------------------------------------------
void RaspberryPi::DACCommand(char Command, char Address, uint16_t Data) 
{
	char output[4];
	output[0] = Command & 0x0f;
	output[1] = (Address & 0x0f) << 4 | (char)(Data >> 12);
	output[2] = (char)((Data >> 4) & 0x00ff);
	output[3] = (char)((Data & 0x000f) << 4);
	//	cout << hex << (int)output[0] << ' ' << (int)output[1] << ' ' << (int)output[2] << ' ' << (int)output[3] << endl;

	bcm2835_spi_transfern(output, 4);
}
//--------------------------------------------------------------------------------