#include <algorithm>
#include <iostream>
#include "tre.hpp"

std::vector<unsigned int> TRE(std::vector<unsigned char> image)
{
	/**
    Apply a Two-Role-Encoder on a vector 'image'. The number of consecutive zeros 
    must be smaller than 256.

    @param 	input image
    @return a vector begin the result of the TRE
	*/
	std::vector<unsigned int> result;
	result.reserve(image.size());
	std::vector<unsigned char>::iterator it = begin(image);
	while (it < end(image))
	{
		if (*it)
		{
			result.push_back(*it + 256);
			++it;
		}
		else
		{	
			result.push_back(distance(it,std::find_if(it, end(image), bind1st(std::not_equal_to<unsigned char>(),*it))));
			if (result.back() > 255)
			{
				std::cout << "Too many consecutive zeros!" << std::endl;
			}
			it += result.back();
		}
	}
	return result;
}

std::vector<unsigned char> iTRE(std::vector<unsigned int> image)
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
		if (image[index] > 255)
		{
			result.push_back(image[index] - 256);
		}
		else
		{
			std::fill_n(back_inserter(result), image[index], 0);
		}
		++index;
	}
	return result;
}
