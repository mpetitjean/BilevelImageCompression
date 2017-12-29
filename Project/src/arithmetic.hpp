#ifndef __ARITHMETIC_HPP_INCLUDED__
#define __ARITHMETIC_HPP_INCLUDED__

#include <vector>
#include <unordered_map>
#include <gmpxx.h>


std::unordered_map <unsigned int, double> nbOccurences(std::vector<unsigned int> encoded);

std::unordered_map<unsigned int, std::pair<double, double>> createIntervals(std::unordered_map <unsigned int, double> occ);

mpf_class arithmeticEncoder(std::unordered_map<unsigned int, std::pair<double, double>> intervalsMap, std::vector<unsigned int> TREd);

std::vector<unsigned int> arithmeticDecoder(mpf_class encoded, std::unordered_map<unsigned int, std::pair<double, double>> intervalsMap);

#endif
