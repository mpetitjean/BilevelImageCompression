#include <algorithm>
#include "m2f.hpp"

std::vector<unsigned char> M2F(std::vector<unsigned char> image, std::deque<unsigned char> dictionnary)
{	
	/**
    Performs the Move-To-Front transform on a vector 'image' while updating 'dictionnary'.
    A deque is used to allow fast insertions.

    @param 	image: input image
			dictionnary: a deque acting as the dictionnary for the MTF
    @return a vector being the result of the MTF
	*/
	std::vector<unsigned char> result;
	result.reserve(image.size());
	size_t index;
	unsigned char temp;
	for (auto num : image)
	{
		index = std::distance(begin(dictionnary), std::find(begin(dictionnary), end(dictionnary), num));
		result.push_back(index);
		temp = std::move(dictionnary[index]);
		dictionnary.erase(begin(dictionnary) + index);
		dictionnary.push_front(temp);
	}
	return result;
}

std::vector<unsigned char> iM2F(std::vector<unsigned char> image, std::deque<unsigned char> dictionnary)
{
	/**
    Performs the inverse Move-To-Front transform on a vector 'image' while updating 'dictionnary'.
    A deque is used to allow fast insertions

    @param 	image: input transformed image
			dictionnary: a deque acting as the dictionnary for the iMTF
    @return a vector being the result of the iMTF
	*/
	std::vector<unsigned char> result;
	result.reserve(image.size());
	unsigned char temp;
	for (auto num : image)
	{
		result.push_back(dictionnary[num]);
		temp = std::move(dictionnary[num]);
		dictionnary.erase(begin(dictionnary) + num);
		dictionnary.push_front(temp);
	}
	return result;
}