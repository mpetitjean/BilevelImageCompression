#include <iostream>
#include <fstream>

#define LENGTH_1D		256

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
	encoded[num] = runs;

	return num+1;
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

std::string golomb(int input)
{
	int digits;
	input ++;
	int in = input;

	// find number of bits
	for (digits = 0; in > 0; in >>= 1)
	{
		digits ++;
	}

	// Binary representation
	std::string bin;
	for (int i = digits-1; i >= 0; i--)
	{
		bin += std::to_string((input >> i) & 1);
	}

	return std::string(digits-1, '0') + bin;
}

void toGolomb(int * encoded, std::string filename, int size)
{
	std::ofstream outfile;
	outfile.open(filename, std::ios::out);

	for (int i = 0; i < size; ++i)
	{
		outfile << golomb(encoded[i]);
	}

	outfile.close();
}

int * fromGolomb(std::string filename, int size)
{
	std::ifstream infile;
	infile.open(filename);

	int count = 0;
	if(infile.is_open())
	{
		while(!infile.eof())
		{	
			if(file.get() == '1')
			{
				
			}
		}	
		infile.close();
	}
	else
		std::cout << "Unable to open " << filename << std::endl;
}

int main()
{
	std::string res = golomb(4);
	std::cout << res << std::endl;

	// Read bilevel image
	float * earth = new float[LENGTH_1D*LENGTH_1D];
	load("earth_binary_256x256.raw", earth);	

	// Encode RLE
	int * encoded = new int[LENGTH_1D*LENGTH_1D];
	int size = encode_rle(earth, encoded);

	// Encode Exp-Golomb
	toGolomb(encoded, "golombed.txt", size);
	fromGolomb("golombed.txt", size);
	
	delete earth;
	delete encoded; 

	return 0;
}