// laser.c
//
// After installing bcm2835, you can build this
// with something like:
// gcc -o laser -l rt laser.c -l bcm2835
// sudo ./spi

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include "RaspberryPi.h"
//#include <iomanip>
//--------------------------------------------------------------------------------
using namespace std;
//--------------------------------------------------------------------------------
RaspberryPi pi;
//--------------------------------------------------------------------------------
struct COORDINATE
{
	uint16_t x;
	uint16_t y;
	bool on;
};
//--------------------------------------------------------------------------------
bool LoadFile(const string &file, vector<COORDINATE> &coordinates)
{
	ifstream myFile (file.c_str(), ios::in | ios::binary);
	if (myFile.fail())
	{
		cout << "Input file could not be found" << endl;
		return false;
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

	return true;
}
//--------------------------------------------------------------------------------
void ShowImage(const vector<COORDINATE> &coordinates, uint32_t cycles=0)
{
	//	cout << "ShowImage " << coordinates.size() << endl;
	bool wasOn=false;

	// Turn it off
	pi.SetLaser(false);

	uint32_t count=0;
	while (!cycles || count++<cycles)
	{
		for (vector<COORDINATE>::const_iterator p=coordinates.begin();p!=coordinates.end();++p)
		{
			if (p->on != wasOn)
			{
				pi.SetLaser(p->on);
				wasOn = p->on;
			}

			//			cout << p->x << '\t' << p->y << endl;
			pi.SetXY(p->x, p->y);
		}
	}
	//	cout << "bye";
}
//--------------------------------------------------------------------------------
void Square(uint16_t length) 
{
	uint16_t stepsize = 100, x, y, i;

	pi.SetY(0x0000);

	for (i=0;i<50;++i)
	{
		for (x=0;x<length-stepsize-1;x+=stepsize)
			pi.SetX(x);

		for (y=0;y<length-stepsize-1;y+=stepsize)
			pi.SetY(y);

		for (;x>stepsize+1;x-=stepsize)
			pi.SetX(x);

		for (;y>stepsize+1;y-=stepsize)
			pi.SetY(y);
	}
}
//--------------------------------------------------------------------------------
void Circle(uint16_t radius)
{
	float theta;
	uint16_t i;

	for (i=0;i<50;++i)
	{
		for (theta = 0.0;theta < 360.0; theta +=0.5)
		{
			pi.SetXY((uint16_t)(0x7fff + ((float)radius * cos(theta / 180.0 * 3.1415926539))), 
				(uint16_t)(0x7fff + ((float)radius * sin(theta / 180.0 * 3.1415926539))));
		}
	}
}
//--------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	// Set up all the appropriate registers on the Pi.
	if (!pi.InitializePi())
		return -1;

	vector<vector<COORDINATE> > coordinates;
	const string filenamePrefix="out";
	uint16_t i=0;
	while (1)
	{
		ostringstream str;
		str << filenamePrefix << i++ << ".csv";
		vector<COORDINATE> temp;
		if (!LoadFile(str.str(), temp))
			break;
		
		coordinates.push_back(temp);
	}

	const uint32_t cycles=1;
	while (1)
	{   
		for (vector<vector<COORDINATE> >::const_iterator p=coordinates.begin();p!=coordinates.end();++p)
			ShowImage(*p, cycles);
	}

	/*
	while (1)
	{
	//	Square(0x1fff);
	Circle(0x0fff);
	}*/
	//	CycleTest1(0);

	pi.ShutdownPi();
	return 0;
}
//--------------------------------------------------------------------------------
