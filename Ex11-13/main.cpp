#include <iostream>
#include <fstream>
#include <cmath>

#define LENGTH_1D		256

void load(std::string filename, float * buffer)
{
	std::ifstream is (filename, std::ifstream::binary);
	
	if(is)
	{
		is.read(reinterpret_cast<char*> (buffer), LENGTH_1D*LENGTH_1D*sizeof(float));
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

void storeToCSV(int * arrayIn, std::string filename, int size)
{
	std::ofstream outfile;
	outfile.open(filename, std::ios::out);

	for (int i = 0; i < size; ++i)
	{
		outfile << std::to_string(arrayIn[i]) << std::endl;
	}
	outfile.close();
}

float * normalizeToPdf(int * occurences, int size)
{
	float * normalized = new float[size];
	int sum = 0;
	// find sum of elements in occurences image
	for (int i = 0; i < size+1; ++i)
	{
		sum += occurences[i];
	}

	// normalize each element
	for (int i = 0; i < size; ++i)
	{
		normalized[i] = ((float)occurences[i])/sum;
	}

	return normalized;
}	

int * findOccurences(int * encoded, int * size)
{
	// Find maximum of encoded
	int max = 0; 
	for (int i = 0; i < LENGTH_1D*LENGTH_1D; ++i)
	{
		if (encoded[i] > max)
		{
			max = encoded[i];
		}
	}
	* size = max;

	// Create array and fill it
	int * P = new int[max+1];
	// fill P with zeros
	for (int i = 0; i < max+1; ++i)
	{
		P[i] = 0;
	}

	for (int i = 0; i < LENGTH_1D*LENGTH_1D; ++i)
	{
		P[encoded[i]] ++;

	}
	P[0] = 0;
	return P;
}

float entropy(float * pdf, int size)
{
	float H = 0.0;
	for (int i = 0; i < size; ++i)
	{
		if (pdf[i] != 0)
		{
			H -= pdf[i] * log2(pdf[i]);
		}
	}

	return H;
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

int * createLUT(int * occurences)
{
	
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
			if(infile.get() == '1')
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
	std::cout << "\nThere were " << size << " runs." << std::endl;
	
	// decode RLE
	float * decoded = new float[LENGTH_1D*LENGTH_1D];
	decode_rle(encoded, decoded, size);
	store(decoded, "earth_decoded.raw", LENGTH_1D);

	// Find number of occurences and normalize
	int occ = 0;
	int * P = findOccurences(encoded, &occ);
	storeToCSV(P, "occurences.csv", occ);
	float * pdf = normalizeToPdf(P, occ);

	// Find entropy
	float H = entropy(pdf, occ);
	std::cout << "Entropy is " << H << " bits/run" << std::endl;

	// Encode Exp-Golomb
	//toGolomb(encoded, "golombed.txt", size);
	//fromGolomb("golombed.txt", size);

	delete pdf;
	delete P;
	delete decoded;
	delete earth;
	delete encoded; 

	return 0;
}