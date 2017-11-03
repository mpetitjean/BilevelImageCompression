#include <cmath>
#include <iostream>
#include <fstream>

#define WIDTH 	256
#define HEIGHT 	256

void store(float* arrayIn)
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
	float I[WIDTH*HEIGHT];

	for (int w = 0; w < WIDTH; w++)
	{
		for (int h = 0; h < HEIGHT; h++)
		{
			I[w + h*HEIGHT] = 0.5 + 0.5*cos(w*M_PI/32)*cos(h*M_PI/64);
		}
	}
	store(I);

	return 0;
}
