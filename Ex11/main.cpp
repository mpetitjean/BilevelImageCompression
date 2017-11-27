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

int encode_rle(float * bufferIn, int * encoded)
{
	int runs = 0;
	int num = 0;
	float previous = bufferIn[0];

	for (int i = 0; i < LENGTH_1D; ++i)
	{
		for (int j = 0; j < LENGTH_1D; ++j)
		{
			if (bufferIn[j + i*LENGTH_1D] != previous)
			{
				previous = bufferIn[j + i*LENGTH_1D];
				encoded[num] = runs;
				runs = 0;
				num ++;
			}
			else
			{
				runs ++;
			}
		}
	}

	return num;
}

int main()
{
	// Read bilevel image
	float * earth = new float[LENGTH_1D*LENGTH_1D];
	load("earth_binary_256x256.raw", earth);	

	// Encode RLE
	int * encoded = new int[LENGTH_1D*LENGTH_1D];
	int size = encode_rle(earth, encoded);

	std::cout << size << std::endl;
	

	return 0;
}