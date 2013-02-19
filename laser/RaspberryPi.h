#ifndef RASPI
#define RASPI
#include <stdint.h>

class RaspberryPi
{
	void DACCommand(char Command, char Address, uint16_t Data);
public:
	bool InitializePi();
	void ShutdownPi();
	void SetLaser(bool on);
	void SetXY(uint16_t x, uint16_t y);
	void SetX(uint16_t x);
	void SetY(uint16_t y);
};

#endif