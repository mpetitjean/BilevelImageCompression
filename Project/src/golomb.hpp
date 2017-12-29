#ifndef __GOLOMB_HPP_INCLUDED__
#define __GOLOMB_HPP_INCLUDED__

#include <vector>
#include <numeric>

std::string golomb(uint value);

uint golomb(std::string value);

std::vector<int> golomb(std::string filename, int size);

std::vector<float> normalize(std::vector<float> P);

std::vector<float> nbOccurences(std::vector<int> encoded);

template <typename T>
std::vector<int> createLUT(const std::vector<T> &v)
{	
	/**
    Creates a LUT for entropy coding, its purpose being to map short
    codewords to probable symbols

    @param vector containing the number of occurences of each run length
    @return vector of which the values indicate to what symbol its index is
    		mapped. Example:
    		vector {2,0,1,3,4} means that the mapping is 0-1,1-0,2-1,3-3,4-4
	*/

	// initialize original index locations
	std::vector<int> idx(v.size());
	iota(idx.begin(), idx.end(), 0);

	// sort indexes based on comparing values in v
	sort(idx.begin(), idx.end(),
	   [&v](int i1, int i2) {return v[i1] > v[i2];});

	std::vector<int> LUT(v.size());

	// Fill LUT
	for (size_t i = 0; i <  v.size(); ++i)
	{
			LUT[idx[i]] = i;
	}
	return LUT;
}


#endif
