#ifndef __GOLOMB_HPP_INCLUDED__
#define __GOLOMB_HPP_INCLUDED__

#include <vector>

std::string golomb(std::vector<unsigned int> stream, std::vector<uint> LUT);

std::vector<uint> golomb(std::string filename, size_t size, std::vector<uint> LUT);

std::string golomb(std::vector<unsigned int> stream);

std::string golomb(uint value);

uint golomb(std::string value);

std::vector<uint> golomb(std::string filename, size_t size);

std::vector<float> normalize(std::vector<float> P);

std::vector<unsigned int> nbOccurences(std::vector<uint> encoded);

std::vector<unsigned int> createLUT(std::vector<unsigned int> occurences, std::vector<unsigned int> input);

#endif
