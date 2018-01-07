#ifndef __RLE_HPP_INCLUDED__
#define __RLE_HPP_INCLUDED__

#include <vector>

std::vector<uint32_t> encode_rle(std::vector<unsigned char> image);

std::vector<unsigned char> decode_rle(std::vector<uint32_t> image);

#endif
