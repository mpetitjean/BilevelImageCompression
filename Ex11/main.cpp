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

void store(float* arrayIn, std::string filename, int size)
{
	std::ofstream outfile;
	outfile.open(filename, std::ios::out | std::ios::binary);

	if (outfile.is_open()) 
	{
		outfile.write(reinterpret_cast<const char*>(arrayIn), size*size*sizeof(float));
	}
	outfile.close();
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
				runs = 1;
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

void decode_rle(int * encoded, float * decoded, int num)
{
	int run;
	float val = 0.0;
	int offset = 0;

	for (int i = 0; i < num; ++i)
	{
		run = encoded[i];

		for (int j = 0; j < run; ++j)
		{
			decoded[j+offset] = val; 
		}

		offset += run;
		val = !val;
	}
}

float * normalizeToPdf(float * occurences)
{
	float * normalized = new float[LENGTH_1D*LENGTH_1D];
	float sum = 0.0;
	// find sum of elements in occurences image
	for (int i = 0; i < LENGTH_1D*LENGTH_1D; ++i)
	{
		sum += occurences[i];
	}
	// normalize each element
	for (int i = 0; i < LENGTH_1D*LENGTH_1D; ++i)
	{
		normalized[i] = occurences[i]/sum;
	}

	return normalized;
}	

int main()
{
	// Read bilevel image
	float * earth = new float[LENGTH_1D*LENGTH_1D];
	load("earth_binary_256x256.raw", earth);	

	// Encode RLE
	int * encoded = new int[LENGTH_1D*LENGTH_1D];
	int size = encode_rle(earth, encoded);
	std::cout << "\nThere were " << size << " runs." << std::endl;
	
	// decode RLE
	float * decoded = new float[LENGTH_1D*LENGTH_1D];
	decode_rle(encoded, decoded, size);
	store(decoded, "earth_decoded.raw", LENGTH_1D);

	delete decoded;
	delete earth;
	delete encoded; 

	return 0;
}