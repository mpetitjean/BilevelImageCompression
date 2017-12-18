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
std::vector<int> createLUT(const std::vector<T> &v) {

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




int main()
{
	std::vector<float> imagefloat(256*256);
	load("earth_binary_256x256.raw", imagefloat);
	std::vector<char> image(imagefloat.begin(), imagefloat.end());
	std::vector<char> out(image.size());
	std::vector<char> out2(image.size());
	
	
	for(auto i = 0; i < sqrt(image.size()); ++i)
	{
		out[i*sqrt(image.size())] = image[i*sqrt(image.size())];
		for(auto j = 1; j < sqrt(image.size()); ++j)
		{
			out[i*sqrt(image.size()) + j] = (image[i*sqrt(image.size()) + j -1] != image[i*sqrt(image.size()) + j]);
		}
	}
	for(auto i = 0; i < sqrt(image.size()); ++i)
	{
		out2[i] = out[i];
		for(auto j = 1; j < sqrt(image.size()); ++j)
		{
			out2[j*sqrt(image.size()) + i] = (out[(j-1)*sqrt(image.size()) + i] != out[j*sqrt(image.size()) + i]);
		}
	}
	std::ofstream outfile;
	outfile.open("out.raw");
	for(auto value : out2)
	{
		outfile << value;
	}
	outfile.close();
	int block_size = 16;
	int length = sqrt(out2.size());
	std::vector<int> result;
	result.reserve(out2.size());
	int nbframe = length / block_size;
	std::vector<char> block(block_size * block_size);
	for (int step = 0; step < nbframe * nbframe; ++step)
	{
		int off_col = (step % nbframe) * block_size;
		int off_row = step / nbframe * length * block_size;


		for (int i = 0; i < block_size; ++i)
		{
		 	std::copy_n(image.begin() + i * length + off_row + off_col, block_size, block.begin() + i * block_size);
		}
		std::vector<int> ttt = encode_rle(block);
		result.insert(result.end(), ttt.begin(), ttt.end());
	}
	std::vector<int> encoded = result;//encode_rle(out2);
	std::vector<float> P = nbOccurences(encoded);
	std::vector<float> Pnorm = normalize(P);
	toCSV("test.csv", P);
	
	outfile.open("test.raw");
	for(auto value : encoded)
	{
		outfile << golomb(value);
	}
	outfile.close();
	std::vector<int> encoded2 = golomb("test.raw", encoded.size());
	std::vector<int> LUT = createLUT(P);
	
	outfile.open("testttt.raw");
	for(auto value : encoded)
	{
		outfile << golomb(LUT[value]);
	}
	outfile.close();
	outfile.open("LUT.txt");
	for(auto value : LUT)
	{
		outfile << value << std::endl;
	}
	outfile.close();
	std::vector<int> encoded3 = golomb("testttt.raw", encoded.size());
	std::transform(encoded3.begin(), encoded3.end(), encoded3.begin(),
		[LUT](int value){return std::distance(LUT.begin(), std::find(LUT.begin(), LUT.end(), value));});
	std::cout << (encoded3 == encoded && encoded2 == encoded) << std::endl;
	

	return 0;
}