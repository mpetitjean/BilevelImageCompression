#include <iostream>
#include <fstream>
#include <boost/dynamic_bitset.hpp>
#include <numeric>
#include <climits>
#include "golomb.hpp"

std::string golomb(std::vector<uint32_t> stream)
{
	/**
   	Converts a vector of integers to a string of its Golomb codes

    @param 	vector to be processed
    @return string of golomb codes
	*/
	
	std::string output;
	for (auto value: stream)
		output += golomb(value);
	return output;
}


std::string golomb(std::vector<uint32_t> stream, std::vector<uint32_t> LUT)
{
	/**
   	Converts a vector of integers to a string of its Golomb codes, taking into account a LUT that
   	maps the shortest codewords to most probable symbols. See the createLUT function.

    @param 	vector to be processed
    		vector representing the LUT
    @return string of golomb codes
	*/
	std::string output;
	std::for_each(stream.begin(), stream.end(),
		[LUT, &output](uint32_t value){output += golomb(std::distance(LUT.begin(), std::find(LUT.begin(), LUT.end(), value)));});
	return output;
}

std::string golomb(uint32_t value)
{
	/**
   	Converts an unisgned integer to a string of its Golomb code

    @param 	unsigned integer value to be coded
    @return string of golomb code
	*/	
 	std::string buffer;
 	++value;
	to_string(boost::dynamic_bitset<> (2 * (CHAR_BIT*sizeof(value) - __builtin_clz(value)) - 1, value), buffer);
	return buffer;
}

uint32_t golomb(std::string value)
{
	/**
   	Inverse Golomb coding 

    @param 	string of the golomb code
    @return converted integer value
	*/

	return (uint32_t)(std::stol(value, nullptr, 2) - 1);
}

std::vector<uint32_t> golomb(std::string filename, size_t size)
{
	/**
   	Opens the file "filename", reads the content and converts each Golomb code to its
	corresponding integer.    	 

    @param 	name of the file to be processed
    		size of the data, ie number of golomb codes (must be called as reference to be used as output)
    @return vector of decoded values
	*/
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
	/**
   	Opens the file "filename", reads the content and converts each Golomb code to its
	corresponding integer, taking into account the mapping of the LUT.

    @param 	name of the file to be processed
    		size of the data, ie number of golomb codes (must be called as reference to be used as output)
    		vector representing the LUT
    @return vector of decoded values
	*/
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
	/**
   	Counts the number of occurences of each individual values in a vector

    @param 	vector to be processed
    @return vector where the index i represents a value, and the corresponding element occ[i] is its number
    			of occurences in the initial vector 
	*/
	std::vector<uint32_t> occ(*std::max_element(encoded.begin(), encoded.end()) + 1, 0);
	for (uint32_t i : encoded)
		++occ[i];
	return occ;
}

std::vector<uint32_t> createLUT(std::vector<uint32_t> occurences, std::vector<uint32_t> input)
{
	/**
    Creates a LUT for entropy coding, its purpose being to map short
    codewords to probable symbols

    @param  vector containing the number of occurences of each run length
    @return vector of which the index indicates to what symbol its value is
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