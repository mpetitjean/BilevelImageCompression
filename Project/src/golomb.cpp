#include <iostream>
#include <fstream>
#include <boost/dynamic_bitset.hpp>
#include <numeric>
#include <climits>
#include "golomb.hpp"

std::string golomb(std::vector<unsigned int> stream)
{
	std::string output;
	for (auto value: stream)
		output += golomb(value);
	return output;
}

std::string golomb(std::vector<unsigned int> stream, std::vector<uint> LUT)
{
	std::string output;
	std::for_each(stream.begin(), stream.end(),
		[LUT, &output](uint value){output += golomb(std::distance(LUT.begin(), std::find(LUT.begin(), LUT.end(), value)));});
	return output;
}

std::string golomb(uint value)
{
 	std::string buffer;
 	++value;
	to_string(boost::dynamic_bitset<> (2 * (CHAR_BIT*sizeof(value) - __builtin_clz(value)) - 1, value), buffer);
	return buffer;
}

uint golomb(std::string value)
{
	return (uint)(std::stol(value, nullptr, 2) - 1);
}

std::vector<uint> golomb(std::string filename, size_t size)
{
	std::vector<uint> encoded;
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

std::vector<uint> golomb(std::string filename, size_t size, std::vector<uint> LUT)
{
	std::vector<uint> encoded;
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
    			encoded.push_back(LUT[golomb(str)]);
    			str = "1";
    			count = 0;
			}
		}
	}
	else
		std::cout << "Cannot read " << filename << std::endl;
	return encoded;
}

std::vector<float> normalize(std::vector<float> P)
{
	float sum = std::accumulate(P.begin(), P.end(), 0.);
	std::transform(P.begin(), P.end(), P.begin(), [sum](float val){return val/sum;});
	return P;
}

std::vector<unsigned int> nbOccurences(std::vector<uint> encoded)
{
	std::vector<unsigned int> occ(*std::max_element(encoded.begin(), encoded.end()) + 1, 0);
	for (int i : encoded)
		++occ[i];
	return occ;
}

std::vector<unsigned int> createLUT(std::vector<unsigned int> occurences, std::vector<unsigned int> input)
{
	/** OUTDATED
    Creates a LUT for entropy coding, its purpose being to map short
    codewords to probable symbols

    @param vector containing the number of occurences of each run length
    @return vector of which the index indicate to what symbol its value is
    		mapped. Example:
    		vector {2,5,1,3,4} means that the mapping is 2-0,5-1,1-2,3-3,4-4
	*/

	// initialize original index locations
	std::sort(begin(input), end(input));
	size_t size = std::unique(begin(input), end(input)) - begin(input);
	std::vector<unsigned int> idx(occurences.size());
	iota(idx.begin(), idx.end(), 0);

	// sort indexes based on comparing values in occurences
	sort(idx.begin(), idx.end(),
	   [occurences](uint i1, uint i2) {return occurences[i1] > occurences[i2];});

	std::vector<uint> LUT(size);

	// Fill LUT
	for (size_t i = 0; i <  size; ++i)
	{
			LUT[i] = idx[i];
	}
	return LUT;
}