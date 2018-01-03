#include <cmath>
#include <algorithm>
#include <bitset>
#include <set>
#include <deque>
#include "io.hpp"
#include "rle.hpp"
#include "arithmetic.hpp"
#include "golomb.hpp"
#include "tre.hpp"
#include "m2f.hpp"
// #include <iostream>
// #include <fstream>
// #include <vector>
// #include <numeric>
// #include <map>
// #include <stdio.h>
// #include <gmp.h>

//#define LENGTH_1D	256

std::vector<unsigned char> shrinkColumnTo8bpp(std::vector<unsigned char> binaryImage)
{	
	/**
    Shrinks an image of dimension MxN to a (M/8)xN by grouping 8 bits per 8 bits in column.

    @param vector only containing 1's or 0's
    @return a smaller vector of 8-bit values corresponding to a column-wise shrink
	*/
	int side = sqrt(binaryImage.size());
	std::vector<unsigned char> shrinked(binaryImage.size()/8);
	
	for (int i = 0; i < side; ++i)
	{
		for (int j = 0; j < side; j += 8)
		{
			unsigned char res = 0;
			for (int k = 0; k < 8; ++k)
			{
				res += (binaryImage[(j+k)*side + i] << (7-k));
			}
			shrinked[j/8*side + i] = res;
		}
	}

	return shrinked;
}

std::vector<unsigned char> ExpandColumnFrom8bpp(std::vector<unsigned char> shrinked)
{	
	/**
    Performs the inverse operation as the shrinkColumnTo8bpp function.
    (M/8)xN to MxN

    @param vector of 8-bit values
    @return vector only containing 1's or 0's
	*/
	int side = sqrt(shrinked.size()*8);
	std::vector<unsigned char> binaryImage(shrinked.size()*8);
	std::bitset<8> res;
	
	for (int i = 0; i < side; ++i)
	{
		for (int j = 0; j < side/8; ++j)
		{
			res = shrinked[j*side + i];
			for (int k = 0; k < 8; ++k)
			{	
				binaryImage[(j*8+k)*side + i] = res[7-k];
			}
			
		}
	}

	return binaryImage;
}



int main()
{
	std::vector<float> imagefloat(256*256);
	load("lena_binary_dithered_256x256.raw", imagefloat);
	// load("earth_binary_256x256.raw", imagefloat);
	std::vector<unsigned char> image(imagefloat.begin(), imagefloat.end());
	
	// MTF
	mpf_set_default_prec(70000);
	//1) Shrink image in column
	// std::vector<unsigned char> shrinked = shrinkColumnTo8bpp(image);

	// std::set<unsigned char> dictionnary_set(begin(shrinked), end(shrinked));
	// std::deque<unsigned char> dictionnary(dictionnary_set.begin(), dictionnary_set.end());
	
	// std::vector<unsigned char> coeff = M2F(shrinked, dictionnary);

	// std::vector<unsigned int> run_length = TRE(coeff);
	std::vector<unsigned int> run_length = encode_rle(image);

	run_length.push_back(600);

	std::ofstream outfile ("tre.raw");
	// mp_exp_t exp; 
	for (auto value : run_length)
	outfile << value << std::endl;
	// outfile << res.get_str(exp, 2) << "e" << exp;std::bitset<8>((char)exp);


	// std::cout << exp << std::endl;
	outfile.close();
	std::unordered_map<unsigned int, unsigned int> dico = probabilityInt(run_length);
	uint size;
	std::unordered_map<unsigned int, std::pair<uint, uint>> valmap = createIntervalsInt(dico, size);
	std::string res = arithmeticEncoderInt(valmap, run_length, size);
	std::cout << "Encoded image = " << res << std::endl;
	outfile.open("compressed.raw");
	// mp_exp_t exp; 
	outfile << res;
	// outfile << res.get_str(exp, 2) << "e" << exp;std::bitset<8>((char)exp);
	;


	// std::cout << exp << std::endl;
	outfile.close();
	std::vector<unsigned int> rtest = arithmeticDecoderInt(res, valmap, size); 		
	std::cout << (rtest == run_length) << std::endl;
	outfile.open("resss.raw");
	// mp_exp_t exp; 
	for (auto i = 0; i < rtest.size(); ++i)
	outfile << run_length.at(i) << " " << rtest.at(i) << std::endl;
	// outfile << res.get_str(exp, 2) << "e" << exp;std::bitset<8>((char)exp);
	;


	// std::cout << exp << std::endl;
	outfile.close();
	rtest.pop_back();
	std::vector<unsigned char> result = decode_rle(rtest);
	// std::vector<unsigned char> coeff2 = iTRE(rtest);
	// std::vector<unsigned char> resolve = iM2F(coeff2, dictionnary);
	// std::vector<unsigned char> result = ExpandColumnFrom8bpp(resolve);
	std::cout << (result == image) << std::endl;

	store("result.raw", result);


	return 0;
}