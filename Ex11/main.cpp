#include <iostream>
#include <fstream>
#include <cmath>

#define LENGTH_1D	256

void load(std::string filename, float * buffer)
{
	std::ifstream is (filename, std::ifstream::binary);
	
	if(is)
	{
		is.read (reinterpret_cast<char*> (buffer), LENGTH_1D*LENGTH_1D*sizeof(float));
    	is.close();	
	}
	else
	{
		std::cout << "Error opening file.\n";
	}    
}

float * encode_rle(float * bufferIn)
{
	for (int i = 0; i < LENGTH_1D; ++i)
	{
		for (int j = 0; j < LENGTH_1D; ++j)
		{
			
		}
	}
}

int main()
{
	// Read bilevel image
	float * earth = new float[LENGTH_1D*LENGTH_1D];
	load("earth_binary_256x256.raw", earth);	

	// Encode RLE

	return 0;
}