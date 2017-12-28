#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <numeric>
#include <algorithm>
#include <boost/dynamic_bitset.hpp>

#define LENGTH_1D	256 


template <class T>
int store(std::string filename, std::vector<T> image)
{
	std::ofstream file (filename, std::ios::binary);
	if (file)
		file.write(reinterpret_cast<const char*>(image.data()), image.size() * sizeof(T));
	else
		std::cout << "Cannot write into " << filename << std::endl;
	file.close();
	return file.rdstate();
}

template <class T>
int load(std::string filename, std::vector<T>& image)
{	
	std::ifstream file (filename, std::ios::binary);
	if (file)
		file.read(reinterpret_cast<char*>(image.data()), image.size() * sizeof(T));
	else 
		std::cout << "Cannot read " << filename << std::endl;
	file.close();
	return file.rdstate();
}


std::string golomb(uint value)
{
 	std::string buffer;
 	++value;
	to_string(boost::dynamic_bitset<> (2 * (31 - __builtin_clz(value)) + 1, value), buffer);
	return buffer;
}

uint igolomb(std::string value)
{
	return (uint)(std::stol(value, nullptr, 2) - 1);
}

std::vector<int> fromGolomb(std::string filename, int size, std::vector<int> LUT)
{
	std::vector<int> encoded;
	encoded.reserve(size);
	std::ifstream file (filename);
	uint count = 0;
	std::string str = "1";
	if(file)
	{
		while(!file.eof())
		{
			++count;
			if(file.get()=='1')
			{
				for (char c; str.size() != count && file.get(c); )
    				str += c;
    			encoded.push_back(igolomb(str));
    			str = "1";
    			count = 0;
			}
		}
	}
	else
		std::cout << "Cannot read " << filename << std::endl;

	// Mapping thanks to LUT
	std::transform(encoded.begin(), encoded.end(), encoded.begin(),
		[LUT](int value){return std::distance(LUT.begin(), std::find(LUT.begin(), LUT.end(), value));});

	return encoded;
}

std::vector<int> encode_rle(std::vector<char> image)
{
	int sum = 0;
	char bit = 0;
	std::vector<int> output;
	output.reserve(image.size());
	for(unsigned int i=0; i < image.size(); ++i)
	{
		if (image[i] != bit)
		{
			output.push_back(sum);
			sum = 0;
			bit = !bit;
		}
		++sum;
	}
	output.push_back(sum);
	return output;
}

std::vector<float> normalize(std::vector<float> P)
{
	float sum = std::accumulate(P.begin(), P.end(), 0.);
	std::transform(P.begin(), P.end(), P.begin(), [sum](float val){return val/sum;});
	return P;
}

std::vector<float> nbOccurences(std::vector<int> encoded)
{
	std::vector<float> occ(*std::max_element(encoded.begin(), encoded.end()) + 1, 0.);
	for (int i : encoded)
		occ[i]++;
	return occ;
}

std::vector<char> decode_rle(std::vector<int> image)
{
	char bit = 0;
	std::vector<char> output;
	output.reserve(image.capacity());
	for (int i : image)
	{
		for( ;i > 0; --i)
		{
			output.push_back(bit);
		}
		bit = !bit;
	}
	return output;
}

int toCSV(std::string filename, std::vector<float> data)
{
	std::ofstream file (filename);
	if (file)
		for (float i : data)
				file << std::to_string(i)<< std::endl;
	else
		std::cout << "Cannot write into " << filename << std::endl;
	file.close();
	return file.rdstate();
}

template <typename T>
std::vector<int> createLUT(const std::vector<T> &v) {

	// initialize original index locations
	std::vector<int> idx(v.size());
	iota(idx.begin(), idx.end(), 0);

	// sort indexes based on comparing values in v
	sort(idx.begin(), idx.end(),
	   [&v](int i1, int i2) {return v[i1] > v[i2];});

	std::vector<int> LUT(v.size());

	// fill LUT
	for (size_t i = 0; i <  v.size(); ++i)
	{
			LUT[idx[i]] = i;
	}

	return LUT;
}

void toGolomb(std::vector<int> runs, std::vector<int> LUT)
{	
	std::ofstream outfile;
	outfile.open("golombed.txt");
	for(auto value : runs)
	{
		outfile << golomb(LUT[value]);
	}
	outfile.close();
}

int main()
{	
	// Read image
	std::vector<float> imagefloat(256*256);
	load("earth_binary_256x256.raw", imagefloat);
	std::vector<char> image(imagefloat.begin(), imagefloat.end());

	// Encode RLE
	std::vector<int> encoded = encode_rle(image);
	
	// Get pdf
	std::vector<float> P = nbOccurences(encoded);
	std::vector<float> Pnorm = normalize(P);
	
	// Create LUT and Golomb according to it
	std::vector<int> LUT = createLUT(P);
	toGolomb(encoded, LUT);

	// Save LUT
	std::ofstream outfile;
	outfile.open("LUT.txt");
	for(auto value : LUT)
	{
		outfile << value << std::endl;
	}
	outfile.close();

	// Decode Golomb with LUT then run length
	std::vector<int> Gdecoded = fromGolomb("golombed.txt", encoded.size(), LUT);
	std::vector<char> decoded = decode_rle(Gdecoded);
	store("decoded.raw", decoded);

	if(decoded == image){
		std::cout << "Decompress operation done well." << std::endl;
	}

	return 0;
}