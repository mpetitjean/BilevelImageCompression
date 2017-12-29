#include <iostream>
#include <fstream>
#include <boost/dynamic_bitset.hpp>
#include "golomb.hpp"

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