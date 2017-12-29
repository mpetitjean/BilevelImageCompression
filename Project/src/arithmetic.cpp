#include <iostream>
#include <set>
#include <algorithm>
#include <map>
#include "arithmetic.hpp"

std::unordered_map <unsigned int, double> nbOccurences(std::vector<unsigned int> encoded)
{
	/**
    Counts the number of occurences of each value in 'encoded'

    @param 	vector to be processed
    @return a map of which the key is a value of 'encoded' and the corresponding value is its number 
    		of occurences
	*/
	std::unordered_map <unsigned int, double> occ;
	std::for_each(encoded.begin(), encoded.end(), [&occ](unsigned int val)
		{++occ[val];});

	for (auto pair : occ)
	{
		occ[pair.first] /= encoded.size();
	}
	return occ;
}

std::unordered_map<unsigned int, std::pair<double, double>> createIntervals(std::unordered_map <unsigned int, double> occ)
{	
	/**
	Create a map to register the intervals for each symbol in order to implement the arithmetic encoder and decoder

    @param 	the map of occurences as generated by the function 'nbOccurences'
    @return a map containing 1) a symbol and 2) a pair of its corresponding interval
	*/

	// One symbol ⟷ one pair [min, max)
	std::unordered_map<unsigned int, std::pair<double, double>> result;
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

mpf_class arithmeticEncoder(std::unordered_map<unsigned int, std::pair<double, double>> intervalsMap, std::vector<unsigned int> TREd)
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
		range = high - low;
		std::cout << range << std::endl;
	
		high = low + range*intervalsMap[value].second;
		low = low + range*intervalsMap[value].first;
	}
	outbuff = low + (high - low)/2.;
	return outbuff;
}

std::vector<unsigned int> arithmeticDecoder(mpf_class encoded, std::unordered_map<unsigned int, std::pair<double, double>> intervalsMap)
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
	while(decoded.back() != 0);

	return decoded;
}