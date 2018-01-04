#include <cmath>
#include <algorithm>
#include <bitset>
#include <set>
#include <deque>
#include <sstream>
#include "io.hpp"
#include "rle.hpp"
#include <map>
#include "arithmetic.hpp"
#include "golomb.hpp"
#include "tre.hpp"
#include "m2f.hpp"
// #include <iostream>
// #include <fstream>
// #include <vector>
// #include <numeric>
#include <map>
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

void decompress(std::string filename, int imSize)
{
	std::cout << "Starting decompression of " << filename << "..." << std::endl;

	// Open input file
	std::ifstream infile(filename);
	std::stringstream buffer;
	buffer << infile.rdbuf();
	infile.close();
	std::string compressed = buffer.str();
	
	// Get compression method
	std::string method = compressed.substr(0,2);
	if (method == "10")
	{
		std::cout << "\nThe file was compressed using RLE+Arith." << std::endl;

		// Parse file
		int sizedata = (int)(std::stol(compressed.substr(2,32), nullptr, 2));
		std::string strRes = compressed.substr(2+32,sizedata);
		int sizeDic = compressed.length() - 34 - sizedata;
		std::string strDic = compressed.substr(2+32+sizedata,sizeDic);

		// Recover symbols and probabilities for decoding
		std::map<unsigned int, unsigned int> dico;
		for (int i = 0; i < sizeDic; i+=64)
		{
			dico[(uint)(std::stol(strDic.substr(i,32), nullptr, 2))] = (uint)(std::stol(strDic.substr(i+32,32), nullptr, 2));
		}

		// Recreate probability intervals
		uint size2;
		std::map<unsigned int, std::pair<uint, uint>> valmap = createIntervalsInt(dico, size2);
		std::vector<unsigned int> rtest = arithmeticDecoderInt(strRes, valmap, size2, 600); 		
		rtest.pop_back();
		std::vector<unsigned char> result = decode_rle(rtest);

		std::string name = filename.substr(0,filename.length()-4);
		store(name + "_decompressed.raw", result);
		std::cout << "Done." << std::endl;
	}
	else if (method == "00")
	{
		std::cout << "\nThe file was compressed using M2F+Arith." << std::endl;

		// Parse file
		int sizedata = (int)(std::stol(compressed.substr(2,32), nullptr, 2));
		int sizeDicM2F = (int)(std::stol(compressed.substr(34,32), nullptr, 2));
		int sizeDic = compressed.length() - (2+32+32) - sizedata - sizeDicM2F;
		std::string strRes = compressed.substr(2+32+32,sizedata);
		std::string strDicM2F = compressed.substr(2+32+sizedata,sizeDicM2F);
		std::string strDic = compressed.substr(2+32+32+sizedata+sizeDicM2F,sizeDic);

		// Recover symbols and probabilities for decoding
		std::map<unsigned int, unsigned int> dico;
		for (int i = 0; i < sizeDic; i+=64)
		{
			dico[(uint)(std::stol(strDic.substr(i,32), nullptr, 2))] = (uint)(std::stol(strDic.substr(i+32,32), nullptr, 2));
		}

		// Recreate probability intervals and decode
		uint size2;
		std::map<unsigned int, std::pair<uint, uint>> valmap = createIntervalsInt(dico, size2);
		std::vector<unsigned int> decodedInt = arithmeticDecoderInt(strRes, valmap, size2, 2); 
		std::vector<unsigned char> decoded(decodedInt.begin(),decodedInt.end());
		decoded.pop_back();
		
		// Recover M2F dictionnary and decode
		std::vector<unsigned int> dictionnary_vec(sizeDicM2F/8);
		for (int i = 0; i < sizeDicM2F/8; i++)
		{	
			dictionnary_vec[i] = (unsigned int)(std::stol(strDicM2F.substr(i*8,8), nullptr, 2));
		}
		std::deque<unsigned char> dictionnary(dictionnary_vec.begin(), dictionnary_vec.end());
		std::vector<unsigned char> result = iM2F(decoded, dictionnary);

		// Store final result
		std::string name = filename.substr(0,filename.length()-4);
		store(name + "_decompressed.raw", result);
		std::cout << "Done." << std::endl;
	}
	else
		std::cerr << "Compression method unrecognized." << std::endl;
}

std::string encodeRLEAth(std::vector<unsigned char> image, int imSize)
{
	// RLE Encoding
	std::vector<unsigned int> run_length = encode_rle(image);
	run_length.push_back(600);

	// Perform arithmetic encoding
	std::map<unsigned int, unsigned int> dico = probabilityInt(run_length);

	uint size;
	std::map<unsigned int, std::pair<uint, uint>> valmap = createIntervalsInt(dico, size);
	std::string res = arithmeticEncoderInt(valmap, run_length, size);

	// Encode symbols and their prob
	std::string dic;
	for (auto val : dico)
	{
		dic += std::bitset<sizeof(uint)*8>(val.first).to_string();
		dic += std::bitset<sizeof(uint)*8>(val.second).to_string();
	}

	// Generate header for the file	
	std::string header = "10"; //0b10 = 2 means RLE+arith 
	header += std::bitset<sizeof(uint)*8>(res.length()).to_string();

	// Concatenation
	std::string compressed = header + res + dic;
	float ratio = ((float) imSize)*((float) imSize)/((float) compressed.length());	

	std::cout << "Testing RLE with Arithmetic Coder:" << std::endl;
	std::cout << "Total size = "  << compressed.length() <<  " bits. ";
	std::cout << "Compression ratio is " << ratio << "." << std::endl;
 	std::cout << "Header = " << header.length() <<  " bits" << std::endl;
 	std::cout << "Data = " << res.length() <<  " bits" << std::endl;
 	std::cout << "Dictionnaries = " << dic.length() <<  " bits" << std::endl << std::endl;

 	return compressed;
}

std::string encodeM2FAth(std::vector<unsigned char> image, int imSize)
{
	// Perform M2F transform
	std::set<unsigned char> dictionnary_set(begin(image), end(image));
	std::deque<unsigned char> dictionnary(dictionnary_set.begin(), dictionnary_set.end());
	std::vector<unsigned char> transformed = M2F(image, dictionnary);
	transformed.push_back(2);

	// Perform arithmetic encoding
	std::vector<unsigned int> transformedInt(transformed.begin(), transformed.end());
	std::map<unsigned int, unsigned int> dico = probabilityInt(transformedInt);

	uint size;
	std::map<unsigned int, std::pair<uint, uint>> valmap = createIntervalsInt(dico, size);
	std::string res = arithmeticEncoderInt(valmap, transformedInt, size);

	// Encode symbols and their prob
	std::string dic;
	for (auto val : dico)
	{
		dic += std::bitset<sizeof(uint)*8>(val.first).to_string();
		dic += std::bitset<sizeof(uint)*8>(val.second).to_string();
	}
	// Encode M2F dictionnary
	std::string dicM2F;
	for (auto val : dictionnary)
	{
		dicM2F += std::bitset<sizeof(char)*8>(val).to_string();
	}

	// Generate header for the file	
	std::string header = "00"; //0b00 = 0 means M2F+arith 
	header += std::bitset<sizeof(uint)*8>(res.length()).to_string();
	header += std::bitset<sizeof(uint)*8>(dicM2F.length()).to_string();

	// Concatenation
	std::string compressed = header + res + dicM2F + dic;
	float ratio = ((float) imSize)*((float) imSize)/((float) compressed.length());	

	std::cout << "Testing M2F with Arithmetic Coder:"  << std::endl;
	std::cout << "Total size = "  << compressed.length() <<  " bits. ";
	std::cout << "Compression ratio is " << ratio << "." << std::endl;
 	std::cout << "Header = " << header.length() <<  " bits" << std::endl;
 	std::cout << "Data = " << res.length() <<  " bits" << std::endl;
 	std::cout << "Dictionnaries = " << dic.length() + dicM2F.length() <<  " bits" << std::endl << std::endl;

 	return compressed;
}

void compress(std::string filename, int imSize)
{
	std::cout << "Starting compression of " << filename << "..." << std::endl << std::endl;

	// Load image and convert to chars
	std::vector<float> imagefloat(imSize*imSize);
	load(filename, imagefloat);
	std::vector<unsigned char> image(imagefloat.begin(), imagefloat.end());

	// Test several encoding schemes
	std::string RLEAthEncoded = encodeRLEAth(image, imSize);
	std::string M2FAthEncoded = encodeM2FAth(image, imSize);

	// Select best performance
	// TODO
	std::string compressed = M2FAthEncoded;
	
	// Write to file
	std::string name = filename.substr(0,filename.length()-4);
	std::ofstream outfile (name + ".jpp");
	outfile << compressed;
	outfile.close();
}


int main(int argc, char* argv[])
{

	if (argc < 4)
	{
		std::cerr << "Error using " << argv[0] << ". Correct usage is " << argv[0] << "COMPRESS/DECOMPRESS IMAGE_NAME SIZE" << std::endl;
		return 1;
	}

	if(strcmp(argv[1],"COMPRESS") == 0)
	{
		compress(argv[2], std::stoi(argv[3]));
		return 0;
	}
	else if(strcmp(argv[1],"DECOMPRESS") == 0)
	{
		decompress(argv[2], std::stoi(argv[3]));
		return 0;
	}	
	else
	{
		std::cerr << "Error using " << argv[0] << ". Unknown second argument, should be COMPRESS or DECOMPRESS" << std::endl;		
		return 1;
	}

	std::vector<float> imagefloat(256*256);
	// load("airport_1024x1024.raw", imagefloat);
	// load("baboon_512x512.raw", imagefloat);
	load("earth_binary_256x256.raw", imagefloat);
	// load("house_256x256.raw", imagefloat);
	// load("lena_binary_dithered_256x256.raw", imagefloat);
	std::vector<unsigned char> image(imagefloat.begin(), imagefloat.end());
	
	
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
	outfile.close();


	std::map<unsigned int, unsigned int> dico = probabilityInt(run_length);
	uint size;
	std::map<unsigned int, std::pair<uint, uint>> valmap = createIntervalsInt(dico, size);
	std::string res = arithmeticEncoderInt(valmap, run_length, size);
	// std::vector<unsigned int> occ = nbOccurences(run_length);
	// std::vector<unsigned int> LUT = createLUT(occ);
	// std::string res = golomb(run_length, LUT);

	// std::cout << "Encoded image = " << res << std::endl;
	outfile.open("compressed.raw");
	// mp_exp_t exp; 
	outfile << res;
	outfile.close();

	std::vector<unsigned int> rtest = arithmeticDecoderInt(res, valmap, size, 600); 		
	// std::vector<uint> rtest = golomb("compressed.raw", res.size());
	// std::cout << (rtest == run_length) << std::endl;
	outfile.open("resss.raw");
	// mp_exp_t exp; 
	for (auto i = 0; i < rtest.size(); ++i)
	outfile << run_length.at(i) << " " << rtest.at(i) << std::endl;
	// outfile << res.get_str(exp, 2) << "e" << exp;std::bitset<8>((char)exp);


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