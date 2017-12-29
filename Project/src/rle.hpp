#ifndef __RLE_HPP_INCLUDED__
#define __RLE_HPP_INCLUDED__

#include <vector>

std::vector<unsigned int> encode_rle(std::vector<unsigned char> image);

std::vector<unsigned char> decode_rle(std::vector<unsigned int> image);

#endif
