#include <iostream>
#include <set>
#include <cmath>
#include <algorithm>
#include <map>
#include "arithmetic.hpp"

std::map <unsigned int, double> probability(std::vector<unsigned int> encoded)
{
	/**
    Counts the probability of each value in 'encoded'

    @param 	vector to be processed
    @return a map of which the key is a value of 'encoded' and the corresponding value is its probabilty
	*/
	std::map <unsigned int, double> occ;
	std::for_each(encoded.begin(), encoded.end(), [&occ](unsigned int val)
		{++occ[val];});

	for (auto pair : occ)
	{
		occ[pair.first] /= encoded.size();
	}
	return occ;
}

std::map <unsigned int, unsigned int> probabilityInt(std::vector<unsigned int> encoded)
{
	/**
    Counts the probability of each value in 'encoded'

    @param 	vector to be processed
    @return a map of which the key is a value of 'encoded' and the corresponding value is its probabilty
    */
	
	std::map <unsigned int, unsigned int> occ;
	std::for_each(encoded.begin(), encoded.end(), [&occ](unsigned int val)
		{++occ[val];});
	return occ;
}

std::map<unsigned int, std::pair<double, double>> createIntervals(std::map <unsigned int, double> occ)
{	
	/**
	Create a map to register the intervals for each symbol in order to implement the arithmetic encoder and decoder

    @param 	the map of occurences as generated by the function 'nbOccurences'
    @return a map containing 1) a symbol and 2) a pair of its corresponding interval
	*/

	// One symbol ⟷ one pair [min, max)
	std::map<unsigned int, std::pair<double, double>> result;
	double high = 0.;
	double low = 0.;

	for (auto p : occ)
	{
		high += p.second;
		result[p.first] = std::pair<double, double>(low, high);
		low = high;
	}

	return result;
}

std::map<unsigned int, std::pair<unsigned int, unsigned int>> createIntervalsInt(std::map <unsigned int, unsigned int> occ, unsigned int &size)
{	
	/**
	Create a map to register the intervals for each symbol in order to implement the arithmetic encoder and decoder

    @param 	the map of occurences as generated by the function 'nbOccurences'
    @return a map containing 1) a symbol and 2) a pair of its corresponding interval
	*/

	// One symbol ⟷ one pair [min, max)
	std::map<unsigned int, std::pair<unsigned int, unsigned int>> result;
	unsigned int high = 0;
	unsigned int low = 0;

	for (auto p : occ)
	{
		high += p.second;
		result[p.first] = std::pair<unsigned int, unsigned int>(low, high);
		low = high;
	}
	size = high;

	return result;
}	

std::string arithmeticEncoderInt(std::map<unsigned int, std::pair<unsigned int, unsigned int>> intervalsMap, std::vector<unsigned int> TREd, unsigned int size)
{
	/**
	Implementation of an arithmetic encoder using arbitrary precise numbers.
	The output is assigned by reference because it is not possible to output a type mpf_t

    @param	intervalsMap: map generated by 'createIntervals'
    		TREd: image to be encoded
    		outbuff: reference to an arbitrary precise number, output of the encoder
	*/
	std::string outbuff;
	unsigned int high = 0xFFFFFFFFU, low = 0, pending_bit = 0;
	unsigned long range;
	for (auto value : TREd)
	{
		range = (long)(high - low) + 1;
		//std::cout << range << std::endl;
	
		high = low + (range*intervalsMap[value].second) / size - 1;
		low = low + (range*intervalsMap[value].first) / size;

		while (1)
		{	
			if (high < 0x80000000U)
			{
				outbuff += "0";
				while (pending_bit--)
					outbuff += "1";
				pending_bit = 0;
				
			}
			else if (low >= 0x80000000U)
			{
				outbuff += "1";
				while (pending_bit--)
					outbuff += "0";
				pending_bit = 0;
				low -=  0x80000000U;
				high -= 0x80000000U;
			}
			else if ( low >= 0x40000000U && high < 0xC0000000U )
			{
				++pending_bit;
				low -= 0x40000000U;
				high -= 0x40000000U;
			} 
			else
				break;
			low <<= 1;
			high <<= 1;
			++high;
		}
		if (value == (uint)-1)
			break;
	}
	++pending_bit;
	if ( low < 0x40000000U)
	{
    	outbuff += "0";
			while (pending_bit--)
				outbuff += "1";
	}
	else
	{
    	outbuff += "1";
			while (pending_bit--)
				outbuff += "0";
	}
	return outbuff;
}

mpf_class arithmeticEncoder(std::map<unsigned int, std::pair<double, double>> intervalsMap, std::vector<unsigned int> TREd)
{
	/**
	Implementation of an arithmetic encoder using arbitrary precise numbers.
	The output is assigned by reference because it is not possible to output a type mpf_t

    @param	intervalsMap: map generated by 'createIntervals'
    		TREd: image to be encoded
    		outbuff: reference to an arbitrary precise number, output of the encoder
	*/
	mpf_class high = 1., low = 0., range, outbuff;
	
	for (auto value : TREd)
	{
		range = high - low + 1;
		// std::cout << range << std::endl;
	
		high = low + range*intervalsMap[value].second;
		low = low + range*intervalsMap[value].first;
		// std::cout << high-low << std::endl;
	}
	outbuff = low + (high - low)/2.;

	// Truncate precision
	range = high-low;

	mp_exp_t exp; 
	range.get_str(exp, 10);
	float res = -exp*log2(10);
	//std::cout << (int) res << std::endl;

	// int prec = (int) res;


	// std::cout << "Before truncation: " << outbuff << std::endl;
	mpf_class temp = outbuff;
	outbuff.set_prec((int) res);
	// std::cout << "After truncation: " << outbuff << std::endl;

	return outbuff;
}

std::vector<unsigned int> arithmeticDecoderInt(std::string encoded, std::map<unsigned int, std::pair<unsigned int, unsigned int>> intervalsMap, unsigned int size)
{
	/**
	Implementation of an arithmetic decoder using arbitrary precise numbers.
	Number 0 is defined as EOF.

    @param	encoded: arbitrary precise number, output of the encoder
    		intervalsMap: map generated by 'createIntervals'
	@return the decoded vector
	*/
	// std::vector<unsigned int> decoded(2);
	std::map<unsigned int, unsigned int> inverseIntervalsMap;
	for (auto val: intervalsMap)
		inverseIntervalsMap[val.second.first] = val.first;
	unsigned int high = 0xFFFFFFFFU, low = 0, value;
	unsigned long range, temp;
	std::vector<unsigned int> decoded;
	// printf("there\n");
	value = std::stoul(encoded.substr(0, 32), nullptr, 2);
	// std::cout << value << std::endl;
	encoded.erase(0, 32);

	do
	{
		range = (long)(high - low) + 1;
		temp = ((long)(value - low + 1) * size - 1) / range;
		// std::cout << temp << std::endl;
		decoded.push_back((*(--inverseIntervalsMap.upper_bound(temp))).second);
		high = low + (range*intervalsMap[decoded.back()].second) / size - 1;
		low = low + (range*intervalsMap[decoded.back()].first) / size;
		while (1)
		{
			if (high < 0x80000000U)
			{}
			else if (low >= 0x80000000U)
			{
				low -=  0x80000000U;
				high -= 0x80000000U;
				value -= 0x80000000U;
			}
			else if ( low >= 0x40000000U && high < 0xC0000000U )
			{
				value -= 0x40000000U;
				low -= 0x40000000U;
				high -= 0x40000000U;
			} 
			else
				break;
			low <<= 1;
			high <<= 1;
			++high;
			value <<= 1;
			// printf("lol\n");
			value += encoded.empty() ? 1 : std::stoul(encoded.substr(0, 1), nullptr, 2);
			encoded.erase(0, 1);
		}
		// std::cout << decoded.back() << std::endl;
	}
	while(decoded.back() != (uint) 600);

	return decoded;
}

std::vector<unsigned int> arithmeticDecoder(mpf_class encoded, std::map<unsigned int, std::pair<double, double>> intervalsMap)
{
	/**
	Implementation of an arithmetic decoder using arbitrary precise numbers.
	Number 0 is defined as EOF.

    @param	encoded: arbitrary precise number, output of the encoder
    		intervalsMap: map generated by 'createIntervals'
	@return the decoded vector
	*/
	std::map<double, unsigned int> inverseIntervalsMap;
	for (auto val: intervalsMap)
	{
		inverseIntervalsMap[val.second.first] = val.first;
	}
	mpf_class high = 1., low = 0., range, temp;
	std::vector<unsigned int> decoded;

	do
	{
		range = high - low;
		temp = (encoded - low) / range;

		decoded.push_back((*(--inverseIntervalsMap.upper_bound(temp.get_d()))).second);
		high = low + range*intervalsMap[decoded.back()].second;
		low = low + range*intervalsMap[decoded.back()].first;
	}
	while(decoded.back() != (uint)-1);

	return decoded;
}