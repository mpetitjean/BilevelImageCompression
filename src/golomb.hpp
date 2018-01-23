#ifndef __GOLOMB_HPP_INCLUDED__
#define __GOLOMB_HPP_INCLUDED__

#include <vector>

std::string golomb(std::vector<uint32_t> stream, std::vector<uint32_t> LUT);

std::vector<uint32_t> golomb(std::string filename, size_t size, std::vector<uint32_t> LUT);

std::vector<uint32_t> golomb(std::string golombed, std::vector<uint32_t> LUT);

std::string golomb(std::vector<uint32_t> stream);

std::string golomb(uint32_t value);

uint32_t golomb(std::string value);

std::vector<uint32_t> golomb(std::string filename, size_t size);

std::vector<float> normalize(std::vector<float> P);

std::vector<uint32_t> nbOccurences(std::vector<uint32_t> encoded);

std::vector<uint32_t> createLUT(std::vector<uint32_t> occurences, std::vector<uint32_t> input);

#endif
