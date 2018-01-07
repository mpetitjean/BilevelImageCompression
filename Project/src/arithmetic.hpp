#ifndef __ARITHMETIC_HPP_INCLUDED__
#define __ARITHMETIC_HPP_INCLUDED__

#include <vector>
#include <map>
#include <gmpxx.h>

std::string arithmeticEncoderInt(std::map<uint32_t, std::pair<uint32_t, uint32_t>> intervalsMap, std::vector<uint32_t> TREd, uint32_t size);

std::map <uint32_t, uint32_t> probabilityInt(std::vector<uint32_t> encoded);

std::map<uint32_t, std::pair<uint32_t, uint32_t>> createIntervalsInt(std::map <uint32_t, uint32_t> occ, uint32_t &size);

std::vector<uint32_t> arithmeticDecoderInt(std::string encoded, std::map<uint32_t, std::pair<uint32_t, uint32_t>> intervalsMap, uint32_t size);

#endif
