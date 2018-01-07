#include <algorithm>
#include <functional>
#include <iostream>
#include "rle.hpp"


std::vector<uint32_t> encode_rle(std::vector<unsigned char> image)
{
	/**
    Encodes the content of a bilevel vector into its RLE equivalent.
    Assumes 0 as initial value.

    @param vector only containing 1's or 0's
    @return a vector of the length of each run
	*/
	std::vector<uint32_t> output;
	output.reserve(image.size());

	std::vector<unsigned char>::iterator it = begin(image);
	if(*it)
	{
		output.push_back(0);
	}

	while ((it += output.back())!=end(image))
	{	
		output.push_back(distance(it, std::find_if(it, end(image), bind1st(std::not_equal_to<unsigned char>(),*it))));
	}

	return output;
}

std::vector<unsigned char> decode_rle(std::vector<uint32_t> rle)
{	
	/**
    Decodes back the output of the encode_rle function

    @param vector of run lengths
    @return a binary image
	*/
	char bit = 0;
	std::vector<unsigned char> output;
	output.reserve(rle.capacity());
	for (auto i : rle)
	{
		for( ;i > 0; --i)
		{
			output.push_back(bit);
		}
		bit = !bit;
	}
	return output;
}