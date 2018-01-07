#include <iostream>
#include <fstream>
#include <boost/dynamic_bitset.hpp>
#include <numeric>
#include <climits>
#include "golomb.hpp"

std::string golomb(std::vector<uint32_t> stream)
{
	std::string output;
	for (auto value: stream)
		output += golomb(value);
	return output;
}

std::string golomb(std::vector<uint32_t> stream, std::vector<uint32_t> LUT)
{
	std::string output;
	std::for_each(stream.begin(), stream.end(),
		[LUT, &output](uint32_t value){output += golomb(std::distance(LUT.begin(), std::find(LUT.begin(), LUT.end(), value)));});
	return output;
}

std::string golomb(uint32_t value)
{
 	std::string buffer;
 	++value;
	to_string(boost::dynamic_bitset<> (2 * (CHAR_BIT*sizeof(value) - __builtin_clz(value)) - 1, value), buffer);
	return buffer;
}

uint32_t golomb(std::string value)
{
	return (uint32_t)(std::stol(value, nullptr, 2) - 1);
}

std::vector<uint32_t> golomb(std::string filename, size_t size)
{
	std::vector<uint32_t> encoded;
	encoded.reserve(size);
	std::ifstream file (filename);
	uint32_t count = 0;
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

std::vector<uint32_t> golomb(std::string filename, size_t size, std::vector<uint32_t> LUT)
{
	std::vector<uint32_t> encoded;
	encoded.reserve(size);
	std::ifstream file (filename);
	uint32_t count = 0;
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

std::vector<uint32_t> nbOccurences(std::vector<uint32_t> encoded)
{
	std::vector<uint32_t> occ(*std::max_element(encoded.begin(), encoded.end()) + 1, 0);
	for (uint32_t i : encoded)
		++occ[i];
	return occ;
}

std::vector<uint32_t> createLUT(std::vector<uint32_t> occurences, std::vector<uint32_t> input)
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
	std::vector<uint32_t> idx(occurences.size());
	iota(idx.begin(), idx.end(), 0);
	// sort indexes based on comparing values in occurences
	std::sort(idx.begin(), idx.end(),
	   [&occurences](uint32_t const& i1, uint32_t const& i2) {return occurences[i1] > occurences[i2];});
	std::vector<uint32_t> LUT(size);

	// Fill LUT
	for (size_t i = 0; i <  size; ++i)
	{
			LUT[i] = idx[i];
	}
	return LUT;
}