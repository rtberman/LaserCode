// spi.c
//
// Example program for bcm2835 library
// Shows how to interface with SPI to transfer a byte to and from an SPI device
//
//B After installing bcm2835, you can build this
// with something like:
// gcc -o test -l rt test.c -l bcm2835
// sudo ./spi
//
// Or you can test it before installing with:
// gcc -o spi -l rt -I ../../src ../../src/bcm2835.c spi.c
// sudo ./spi
//
// Author: Mike McCauley (mikem@open.com.au)
// Copyright (C) 2012 Mike McCauley
// $Id: RF22.h,v 1.21 2012/05/30 01:51:25 mikem Exp $
#include <bcm2835.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdlib.h>
//#include <iomanip>

using namespace std;
#define PIN RPI_GPIO_P1_15

struct COORDINATE
{
	uint16_t x;
	uint16_t y;
	bool on;
};

int LoadFile(const string &file, vector<COORDINATE> &coordinates)
{
	ifstream myFile (file.c_str(), ios::in | ios::binary);
	if (myFile.fail())
	{
		cout << "Input file could not be found" << endl;
		return -1;
	}

	while (1)
	{
		string x, y, b;
		getline(myFile, x, ',');
		if (x.empty())
			break;

		getline(myFile, y, ',');
		if (y.empty())
			break;

		getline(myFile, b);
		if (b.empty())
			break;

		COORDINATE coord;
		int32_t scratch;
		scratch = atoi(x.c_str());
		scratch += 0x7fff;
		coord.x = scratch;
//cout << scratch << '\t' << coord.x;

		scratch = atoi(y.c_str());
		scratch += 0x7fff;
		coord.y = scratch;
//cout << "\t\t" << scratch << '\t' << coord.y << endl;

		coord.on = b=="0";

		coordinates.push_back(coord);
	}

	myFile.close();

	return 0;
}


void DACCommand(char Command, char Address, uint16_t Data) 
{
	char output[4];
	output[0] = Command & 0x0f;
	output[1] = (Address & 0x0f) << 4 | (char)(Data >> 12);
	output[2] = (char)((Data >> 4) & 0x00ff);
	output[3] = (char)((Data & 0x000f) << 4);
//	cout << hex << (int)output[0] << ' ' << (int)output[1] << ' ' << (int)output[2] << ' ' << (int)output[3] << endl;

	bcm2835_spi_transfern(output, 4);
}

void ShowImage(const vector<COORDINATE> &coordinates, uint32_t cycles=0)
{
//	cout << "ShowImage " << coordinates.size() << endl;
	bool wasOn=false;

        // Turn it off
        bcm2835_gpio_write(PIN, LOW);

	uint32_t count=0;
        while (!cycles || count++<cycles)
	{
                for (vector<COORDINATE>::const_iterator p=coordinates.begin();p!=coordinates.end();++p)
                {
			if (p->on != wasOn)
			{
                		bcm2835_gpio_write(PIN, p->on ? HIGH : LOW);
				wasOn = p->on;
			}

//			cout << p->x << '\t' << p->y << endl;
                        DACCommand(0, 0, p->x);
                        DACCommand(2, 1, p->y);
                }
	}
//	cout << "bye";
}



void CycleTest1(char Channel) 
{
       uint16_t number=0;
        while (1)
	{
        while (number < 65400)
	{
                DACCommand(3,Channel,number);
                number+=20;
	}
        while (number > 2)
	{
                DACCommand(3,Channel,number);
                number-=20;
	}
	}

}

void Square(uint16_t length) 
{
	uint16_t stepsize = 100, x, y, i;

	DACCommand(3, 1, 0x0000);

	for (i=0;i<50;++i)
	{
	       // Turn it off
	        bcm2835_gpio_write(PIN, LOW);

		for (x=0;x<length-stepsize-1;x+=stepsize)
			DACCommand(3, 0, x);

	        for (y=0;y<length-stepsize-1;y+=stepsize)
                	DACCommand(3, 1, y);
	       // Turn it on
        	bcm2835_gpio_write(PIN, HIGH);


        	for (;x>stepsize+1;x-=stepsize)
	                DACCommand(3, 0, x);

	        for (;y>stepsize+1;y-=stepsize)
        	        DACCommand(3, 1, y);
	}
}

void Circle(uint16_t radius)
{
        float theta;
	uint16_t i;

	for (i=0;i<50;++i)
        {
		for (theta = 0.0;theta < 360.0; theta +=0.5)
		{
			DACCommand(3, 0, (uint16_t)(0x7fff + ((float)radius * cos(theta / 180.0 * 3.1415926539))));
                        DACCommand(3, 1, (uint16_t)(0x7fff + ((float)radius * sin(theta / 180.0 * 3.1415926539))));
		}
        }
}


int main(int argc, char **argv)
{
	// If you call this, it will not actually access the GPIO
	// Use for testing
	// bcm2835_set_debug(1);
	if (!bcm2835_init())
		return 1;

	bcm2835_spi_begin();
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST); // The default
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE1); // The default
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_32); //About 32 is 8 MHz
//	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256); //About 32 is 8 MHz
	bcm2835_spi_chipSelect(BCM2835_SPI_CS1); // The default
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, LOW); // the default

	// Set the pin to be an output
	bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_OUTP);

	vector<COORDINATE> coordinates1, coordinates2, coordinates3, coordinates4;
	vector<COORDINATE> coordinates5, coordinates6, coordinates7, coordinates8;
	vector<COORDINATE> coordinates9, coordinates10, coordinates11, coordinates12;
       LoadFile("out0.csv", coordinates1);
       LoadFile("out1.csv", coordinates2);
       LoadFile("out2.csv", coordinates3);
       LoadFile("out3.csv", coordinates4);
      LoadFile("out4.csv", coordinates5);
      LoadFile("out5.csv", coordinates6);
      LoadFile("out6.csv", coordinates7);
      LoadFile("out7.csv", coordinates8);
      LoadFile("out8.csv", coordinates9);
      LoadFile("out9.csv", coordinates10);
      LoadFile("out10.csv", coordinates11);
      LoadFile("out11.csv", coordinates12);

	uint32_t cycles=1;
	while (1)
{   
 	ShowImage(coordinates1, cycles);
      ShowImage(coordinates2, cycles);
       ShowImage(coordinates3, cycles);
       ShowImage(coordinates4, cycles);
       ShowImage(coordinates5, cycles);
       ShowImage(coordinates6, cycles);
       ShowImage(coordinates7, cycles);
       ShowImage(coordinates8, cycles);
       ShowImage(coordinates9, cycles);
       ShowImage(coordinates10, cycles);
       ShowImage(coordinates11, cycles);
}
//	DACCommand(3,0,0x7fff);
//	DACCommand(3,1,0x7fff);

/*
while (1)
{
//	Square(0x1fff);
	Circle(0x0fff);
}*/
//	CycleTest1(0);


/*
	
// Set to 0
//	uint32_t data = 0x03f00000;
//	uint32_t data=0xffaa00ff;
//	uint32_t data = 0x03000000;
	uint32_t data = 0x00000003;
	bcm2835_spi_transfern((char *)&data, 4);
	bcm2835_delay(1000);
	data = 0xf0ffff03;
	bcm2835_spi_transfern((char *)&data, 4);
	bcm2835_delay(1000);
	data = 0xf0ff0703; //0307fff0
	bcm2835_spi_transfern((char *)&data, 4);
*/
/*while (1)
{
	bcm2835_spi_transfern((char *)&data, 4);
}
	bcm2835_delay(2000);
// Set to +Vdd
	data = 0x03fffff0;
	bcm2835_spi_transfern((char *)&data, 4);
	bcm2835_delay(2000);

	data = 0x03f7fff0;
	bcm2835_spi_transfern((char *)&data, 4);
	bcm2835_delay(2000);
//}*/
	bcm2835_spi_end();
	bcm2835_close();
	return 0;
}
