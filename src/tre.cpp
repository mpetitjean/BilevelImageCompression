#include <algorithm>
#include <iostream>
#include "tre.hpp"

std::vector<uint32_t> TRE(std::vector<unsigned char> image)
{
	/**
    Apply a Two-Role-Encoder on a vector 'image'. The number of consecutive zeros 
    must be smaller than 256.

    @param 	input image
    @return a vector begin the result of the TRE
	*/
	std::vector<uint32_t> result;
	result.reserve(image.size());
	std::vector<unsigned char>::iterator it = begin(image);
	while (it < end(image))
	{
		if (*it)
		{
			result.push_back((*it) << 24);
			++it;
		}
		else
		{	
			result.push_back(distance(it,std::find_if(it, end(image), bind1st(std::not_equal_to<unsigned char>(),*it))));
			if (result.back() >= 0xFFFFFFU)
			{
				std::cerr << "TRE: Too many consecutive zeros! Method excluded." << std::endl;
				result.push_back(-1);
				return result;
			}
			it += result.back();
		}
	}
	return result;
}

std::vector<unsigned char> iTRE(std::vector<uint32_t> image)
{
	/**
    Apply a Two-Role-Decoder on a vector 'image', inverse operation of TRE

    @param 	input TRE'd image
    @return a vector begin the result of the iTRE
	*/
	std::vector<unsigned char> result;
	result.reserve(image.size());
	size_t index = 0;
	while (index < image.size())
	{
		if (image[index] > 0xFFFFFFU)
		{
			result.push_back(image[index] >> 24);
		}
		else
		{
			std::fill_n(back_inserter(result), image[index], 0);
		}
		++index;
	}
	return result;
}
