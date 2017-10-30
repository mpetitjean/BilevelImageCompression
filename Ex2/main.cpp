#include <math.h>
#include <iostream>
#include <fstream>

#define WIDTH 	256
#define HEIGHT 	256

void store(float arrayIn[][HEIGHT])
{
	std::ofstream outfile;
	outfile.open("out.raw", std::ios::out | std::ios::binary);

	if (outfile.is_open()) 
	{
		outfile.write(reinterpret_cast<const char*>(arrayIn), HEIGHT*WIDTH*sizeof(float));
	}
	outfile.close();
}


int main(void)
{
	float I[WIDTH][HEIGHT];

	for (int i = 0; i < WIDTH; i++)
	{
		for (int j = 0; j < HEIGHT; j++)
		{
			I[i][j] = 0.5 + 0.5*cos(i*M_PI/32)*cos(j*M_PI/64);
		}
	}
	store(I);

	return 0;
}