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
std::vector<int> createLUT(const std::vector<T> &v)
{
	// initialize original index locations
	std::vector<int> idx(v.size());
	iota(idx.begin(), idx.end(), 0);

	// sort indexes based on comparing values in v
	sort(idx.begin(), idx.end(),
	   [&v](int i1, int i2) {return v[i1] > v[i2];});

	std::vector<int> LUT(v.size());

	for (size_t i = 0; i <  v.size(); ++i)
	{
			LUT[idx[i]] = i;
	}
	return LUT;
}

std::vector<char> shrinkColumnTo8bpp(std::vector<char> binaryImage)
{
	int side = binaryImage.size();
	std::vector<char> shrinked(side/8);
	
	for (int i = 0; i < (int) sqrt(side); ++i)
	{
		for (int j = 0; j < (int) sqrt(side)/8; j += 8)
		{
			char res = 0;
			for (int k = 0; k < 8; ++k)
			{	
				// if (i == 1 && j == 8)
				// {
				// 	std::cout << (j+k)*sqrt(side) + i << std::endl;
				// }
				res += (binaryImage[(j+k)*sqrt(side) + i] << (7-k));
			}
			shrinked[j*sqrt(side) + i] = res;
		}
	}

	return shrinked;
}


int main()
{
	std::vector<float> imagefloat(256*256);
	load("earth_binary_256x256.raw", imagefloat);
	std::vector<char> image(imagefloat.begin(), imagefloat.end());
	
	// MTF

	//1) Shrink image in column
	std::vector<char> shrinked = shrinkColumnTo8bpp(image);
	store("shrinked.raw", shrinked);


	return 0;
}