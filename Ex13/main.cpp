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

uint golomb(std::string value)
{
	return (uint)(std::stol(value, nullptr, 2) - 1);
}

std::vector<int> golomb(std::string filename, int size)
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
    			encoded.push_back(golomb(str));
    			str = "1";
    			count = 0;
			}
		}
	}
	else
		std::cout << "Cannot read " << filename << std::endl;
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
std::vector<size_t> sort_indexes(const std::vector<T> &v) {

  // initialize original index locations
  std::vector<size_t> idx(v.size());
  iota(idx.begin(), idx.end(), 0);

  // sort indexes based on comparing values in v
  sort(idx.begin(), idx.end(),
       [&v](size_t i1, size_t i2) {return v[i1] < v[i2];});

  return idx;
}




int main()
{
	std::vector<float> imagefloat(256*256);
	load("earth_binary_256x256.raw", imagefloat);
	std::vector<char> image(imagefloat.begin(), imagefloat.end());
	std::vector<int> encoded = encode_rle(image);
	std::vector<float> P = nbOccurences(encoded);
	std::vector<float> Pnorm = normalize(P);
	toCSV("test.csv", P);
	std::ofstream outfile;
	outfile.open("test.raw");
	for(auto value : encoded)
	{
		outfile << golomb(value);
	}
	outfile.close();
	std::vector<int> encoded2 = golomb("test.raw", encoded.size());
	// std::cout << "start" << std::endl;
	// for (auto i: sort_indexes(P)) {
 //  		std::cout << i << std::endl;
	// }
	return 0;
}