#ifndef __M2F_HPP_INCLUDED__
#define __M2F_HPP_INCLUDED__

#include <deque>
#include <vector>

std::vector<unsigned char> M2F(std::vector<unsigned char> image, std::deque<unsigned char> dictionnary);

std::vector<unsigned char> iM2F(std::vector<unsigned char> image, std::deque<unsigned char> dictionnary);

#endif