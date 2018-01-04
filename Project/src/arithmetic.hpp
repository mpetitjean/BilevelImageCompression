#ifndef __ARITHMETIC_HPP_INCLUDED__
#define __ARITHMETIC_HPP_INCLUDED__

#include <vector>
#include <map>
#include <gmpxx.h>


std::map <unsigned int, double> probability(std::vector<unsigned int> encoded);

std::map<unsigned int, std::pair<double, double>> createIntervals(std::map <unsigned int, double> occ);

mpf_class arithmeticEncoder(std::map<unsigned int, std::pair<double, double>> intervalsMap, std::vector<unsigned int> TREd);

std::vector<unsigned int> arithmeticDecoder(mpf_class encoded, std::map<unsigned int, std::pair<double, double>> intervalsMap);

std::string arithmeticEncoderInt(std::map<unsigned int, std::pair<unsigned int, unsigned int>> intervalsMap, std::vector<unsigned int> TREd, unsigned int size);

std::map <unsigned int, unsigned int> probabilityInt(std::vector<unsigned int> encoded);

std::map<unsigned int, std::pair<unsigned int, unsigned int>> createIntervalsInt(std::map <unsigned int, unsigned int> occ, unsigned int &size);

std::vector<unsigned int> arithmeticDecoderInt(std::string encoded, std::map<unsigned int, std::pair<unsigned int, unsigned int>> intervalsMap, unsigned int size);

#endif
