#include <cmath>
#include <algorithm>
#include <bitset>
#include <set>
#include <deque>
#include <sstream>
#include <map>
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
// #include <stdio.h>
// #include <gmp.h>

//#define LENGTH_1D	256

std::vector<unsigned char> transpose(std::vector<unsigned char> image, size_t imSize)
{
	for(size_t i = 0; i < imSize; ++i)
	    for(size_t j = i+1; j < imSize; ++j)
	        std::swap(image[imSize*i + j], image[imSize*j + i]);
	return image;
}

std::vector<unsigned char> transpose(std::vector<unsigned char> image)
{	size_t imSize = sqrt(image.size());
	for(size_t i = 0; i < imSize; ++i)
	    for(size_t j = i+1; j < imSize; ++j)
	        std::swap(image[imSize*i + j], image[imSize*j + i]);
	return image;
}

std::vector<unsigned char> shrinkColumnTo8bpp(std::vector<unsigned char> binaryImage)
{	
	/**
    Shrinks an image of dimension MxN to a (M/8)xN by grouping 8 bits per 8 bits in column.

    @param vector only containing 1's or 0's
    @return a smaller vector of 8-bit values corresponding to a column-wise shrink
	*/
	size_t side = sqrt(binaryImage.size());
	std::vector<unsigned char> shrinked(binaryImage.size()/8);
	for (size_t i = 0; i < side; ++i)
	{
		for (size_t j = 0; j < side; j += 8)
		{
			unsigned char res = 0;
			for (size_t k = 0; k < 8; ++k)
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
	size_t side = sqrt(shrinked.size()*8);
	std::vector<unsigned char> binaryImage(shrinked.size()*8);
	std::bitset<8> res;
	
	for (size_t i = 0; i < side; ++i)
	{
		for (size_t j = 0; j < side/8; ++j)
		{
			res = shrinked[j*side + i];
			for (size_t k = 0; k < 8; ++k)
			{	
				binaryImage[(j*8+k)*side + i] = res[7-k];
			}
			
		}
	}

	return binaryImage;
}

int decompress(std::string filename)
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
	std::vector<unsigned char> result;
	char transposed = compressed[2];
	if (method == "10")
	{
		std::cout << "\nThe file was compressed using RLE+Arith." << std::endl;

		// Parse file
		uint32_t  sizedata = (uint32_t)(std::stol(compressed.substr(3,32), nullptr, 2));
		std::string strRes = compressed.substr(3+32,sizedata);
		uint32_t  sizeDic = compressed.length() - 35 - sizedata;
		std::string strDic = compressed.substr(3+32+sizedata,sizeDic);

		// Recover symbols and probabilities for decoding
		std::map<uint32_t, uint32_t> dico;
		for (uint32_t i = 0; i < sizeDic; i+=64)
		{
			dico[(uint32_t)(std::stol(strDic.substr(i,32), nullptr, 2))] = (uint32_t)(std::stol(strDic.substr(i+32,32), nullptr, 2));
		}

		// Recreate probability intervals
		uint32_t size2;
		std::map<uint32_t, std::pair<uint32_t, uint32_t>> valmap = createIntervalsInt(dico, size2);
		std::vector<uint32_t> rtest = arithmeticDecoderInt(strRes, valmap, size2); 		
		rtest.pop_back();
		result = decode_rle(rtest);
	}
	else if (method == "00")
	{
		std::cout << "\nThe file was compressed using M2F+Arith." << std::endl;

		// Parse file
		uint32_t  sizedata = (uint32_t)(std::stol(compressed.substr(3,32), nullptr, 2));
		uint32_t  sizeDicM2F = (uint32_t)(std::stol(compressed.substr(35,32), nullptr, 2));
		uint32_t  sizeDic = compressed.length() - (3+32+32) - sizedata - sizeDicM2F;
		std::string strRes = compressed.substr(3+32+32,sizedata);
		std::string strDicM2F = compressed.substr(3+32+32+sizedata,sizeDicM2F);
		std::string strDic = compressed.substr(3+32+32+sizedata+sizeDicM2F,sizeDic);

		// Recover symbols and probabilities for decoding
		std::map<uint32_t, uint32_t> dico;
		for (uint32_t i = 0; i < sizeDic; i+=64)
		{
			dico[(uint32_t)(std::stol(strDic.substr(i,32), nullptr, 2))] = (uint32_t)(std::stol(strDic.substr(i+32,32), nullptr, 2));
		}

		// Recreate probability intervals and decode
		uint32_t size2;
		std::map<uint32_t, std::pair<uint32_t, uint32_t>> valmap = createIntervalsInt(dico, size2);
		std::vector<uint32_t> decodedInt = arithmeticDecoderInt(strRes, valmap, size2); 
		decodedInt.pop_back();
		std::vector<unsigned char> decoded(decodedInt.begin(),decodedInt.end());
		
		// Recover M2F dictionnary and decode
		std::vector<uint32_t> dictionnary_vec(sizeDicM2F/8);
		for (uint32_t i = 0; i < sizeDicM2F/8; i++)
		{	
			dictionnary_vec[i] = (uint32_t)(std::stol(strDicM2F.substr(i*8,8), nullptr, 2));
		}
		std::deque<unsigned char> dictionnary(dictionnary_vec.begin(), dictionnary_vec.end());
		result = iM2F(decoded, dictionnary);		
	}
	else if (method == "01")
	{
		std::cout << "\nThe file was compressed using 8 + M2F + TRE + Arith." << std::endl;

		uint32_t sizedata = (uint32_t)(std::stol(compressed.substr(3,32), nullptr, 2));
		uint32_t sizeDicM2F = (uint32_t)(std::stol(compressed.substr(35,32), nullptr, 2));
		uint32_t sizeDic = compressed.length() - (3+32+32) - sizedata - sizeDicM2F;
		std::string strRes = compressed.substr(3+32+32,sizedata);
		std::string strDicM2F = compressed.substr(3+32+32+sizedata,sizeDicM2F);
		std::string strDic = compressed.substr(3+32+32+sizedata+sizeDicM2F,sizeDic);

		// Recover symbols and probabilities for decoding
		std::map<uint32_t, uint32_t> dico;
		for (uint32_t i = 0; i < sizeDic; i+=64)
		{
			dico[(uint32_t)(std::stol(strDic.substr(i,32), nullptr, 2))] = (uint32_t)(std::stol(strDic.substr(i+32,32), nullptr, 2));
		}

		// Recreate probability intervals and decode
		uint32_t size2;
		std::map<uint32_t, std::pair<uint32_t, uint32_t>> valmap = createIntervalsInt(dico, size2);
		std::vector<uint32_t> decodedInt = arithmeticDecoderInt(strRes, valmap, size2); 
		decodedInt.pop_back();
		// Invert TRE
		std::vector<unsigned char> decoded = iTRE(decodedInt);
		// Recover M2F dictionnary and decode
		std::vector<uint32_t> dictionnary_vec(sizeDicM2F/8);
		for (uint32_t i = 0; i < sizeDicM2F/8; i++)
		{	
			dictionnary_vec[i] = (uint32_t)(std::stol(strDicM2F.substr(i*8,8), nullptr, 2));
		}
		std::deque<unsigned char> dictionnary(dictionnary_vec.begin(), dictionnary_vec.end());
		std::vector<unsigned char> shrinked = iM2F(decoded, dictionnary);
		// Expand
		result = ExpandColumnFrom8bpp(shrinked);
	}
	else if (method == "11")
	{
		std::cout << "\nThe file was compressed using RLE+ExpGolomb." << std::endl;

		// Parse file
		uint32_t  sizedata = (uint32_t)(std::stol(compressed.substr(3,32), nullptr, 2));
		std::string strRes = compressed.substr(3+32,sizedata);
		uint32_t sizeDic = compressed.length() - 35 - sizedata;
		std::string strDic = compressed.substr(3+32+sizedata,sizeDic);

		// Recover LUT
		std::vector<uint32_t> LUT;
		LUT.reserve(sizeDic/32);
		for (uint32_t i = 0; i < sizeDic; i+=32)
		{
			LUT.push_back((uint32_t)(std::stol(strDic.substr(i,32), nullptr, 2)));
		}

		// Inverse golomb and RLE
		std::ofstream outfile("golombed.temp");
		outfile << strRes;
		outfile.close();
		std::vector<uint32_t> decoded = golomb("golombed.temp", sizedata, LUT);
		result = decode_rle(decoded);
	}
	else
	{
		std::cerr << "Compression method unrecognized." << std::endl;
		return 3;
	}
	// If fine, store the result
	std::string name = filename.substr(0,filename.length()-4);
	result = transposed=='0' ? result : transpose(result);
	std::vector<float> imagefloat(begin(result), end(result));
	store(name + "_decompressed.raw", imagefloat);
	std::cout << "Output: " << name + "_decompressed.raw" << std::endl;
	return 0;
}

std::string encodeRLEAth(std::vector<unsigned char> image, size_t imSize, char transposed)
{
	// RLE Encoding
	std::vector<uint32_t> run_length = encode_rle(image);
	run_length.push_back(-1);

	// Perform arithmetic encoding
	std::map<uint32_t, uint32_t> dico = probabilityInt(run_length);

	uint32_t size;
	std::map<uint32_t, std::pair<uint32_t, uint32_t>> valmap = createIntervalsInt(dico, size);
	std::string res = arithmeticEncoderInt(valmap, run_length, size);

	// Encode symbols and their prob
	std::string dic;
	for (auto val : dico)
	{
		dic += std::bitset<sizeof(uint32_t)*8>(val.first).to_string();
		dic += std::bitset<sizeof(uint32_t)*8>(val.second).to_string();
	}

	// Generate header for the file	
	std::string header = "10"; //0b10 = 2 means RLE+arith 
	header += transposed;
	header += std::bitset<sizeof(uint32_t)*8>(res.length()).to_string();
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

std::string encodeM2FAth(std::vector<unsigned char> image, size_t imSize, char transposed)
{
	// Perform M2F transform
	std::set<unsigned char> dictionnary_set(begin(image), end(image));
	std::deque<unsigned char> dictionnary(dictionnary_set.begin(), dictionnary_set.end());
	std::vector<unsigned char> transformed = M2F(image, dictionnary);

	// Perform arithmetic encoding
	std::vector<uint32_t> transformedInt(transformed.begin(), transformed.end());
	transformedInt.push_back(-1);
	std::map<uint32_t, uint32_t> dico = probabilityInt(transformedInt);

	uint32_t size;
	std::map<uint32_t, std::pair<uint32_t, uint32_t>> valmap = createIntervalsInt(dico, size);
	std::string res = arithmeticEncoderInt(valmap, transformedInt, size);

	// Encode symbols and their prob
	std::string dic;
	for (auto val : dico)
	{
		dic += std::bitset<sizeof(uint32_t)*8>(val.first).to_string();
		dic += std::bitset<sizeof(uint32_t)*8>(val.second).to_string();
	}
	// Encode M2F dictionnary
	std::string dicM2F;
	for (auto val : dictionnary)
	{
		dicM2F += std::bitset<sizeof(char)*8>(val).to_string();
	}

	// Generate header for the file	
	std::string header = "00"; //0b00 = 0 means M2F+arith
	header += transposed;
	header += std::bitset<sizeof(uint32_t)*8>(res.length()).to_string();
	header += std::bitset<sizeof(uint32_t)*8>(dicM2F.length()).to_string();
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

std::string encode8M2FTREAth(std::vector<unsigned char> image, size_t imSize, char transposed)
{
	std::vector<unsigned char> shrinked = shrinkColumnTo8bpp(image);
	// Perform M2F transform
	std::set<unsigned char> dictionnary_set(begin(shrinked), end(shrinked));
	std::deque<unsigned char> dictionnary(dictionnary_set.begin(), dictionnary_set.end());
	std::vector<unsigned char> transformed = M2F(shrinked, dictionnary);
	std::vector<uint32_t> treEd = TRE(transformed);
	if (treEd.back() == (uint32_t) -1)
	{
		return "-1";
	}
	treEd.push_back(-1);
	// Perform arithmetic encoding
	std::map<uint32_t, uint32_t> dico = probabilityInt(treEd);

	uint32_t size;
	std::map<uint32_t, std::pair<uint32_t, uint32_t>> valmap = createIntervalsInt(dico, size);
	std::string res = arithmeticEncoderInt(valmap, treEd, size);

	// Encode symbols and their prob
	std::string dic;
	for (auto val : dico)
	{
		dic += std::bitset<sizeof(uint32_t)*8>(val.first).to_string();
		dic += std::bitset<sizeof(uint32_t)*8>(val.second).to_string();
	}
	// Encode M2F dictionnary
	std::string dicM2F;
	for (auto val : dictionnary)
	{
		dicM2F += std::bitset<sizeof(char)*8>(val).to_string();
	}

	// Generate header for the file	
	std::string header = "01"; //0b01 = 1 means 8+M2F+TRE+arith
	header += transposed;
	header += std::bitset<sizeof(uint32_t)*8>(res.length()).to_string();
	header += std::bitset<sizeof(uint32_t)*8>(dicM2F.length()).to_string();
	// Concatenation
	std::string compressed = header + res + dicM2F + dic;
	float ratio = ((float) imSize)*((float) imSize)/((float) compressed.length());	

	std::cout << "Testing 8 + M2F + TRE + with Arithmetic Coder:"  << std::endl;
	std::cout << "Total size = "  << compressed.length() <<  " bits. ";
	std::cout << "Compression ratio is " << ratio << "." << std::endl;
 	std::cout << "Header = " << header.length() <<  " bits" << std::endl;
 	std::cout << "Data = " << res.length() <<  " bits" << std::endl;
 	std::cout << "Dictionnaries = " << dic.length() + dicM2F.length() <<  " bits" << std::endl << std::endl;

 	return compressed;
}


std::string encodeRLEGb(std::vector<unsigned char> image, size_t imSize, char transposed)
{
	// RLE Encoding
	std::vector<uint32_t> run_length = encode_rle(image);

	// Perform Golomb encoding
	std::vector<uint32_t> occ = nbOccurences(run_length);
	std::vector<uint32_t> LUT = createLUT(occ, run_length);
	std::string res = golomb(run_length, LUT);

	// Generate header for the file	
	std::string header = "11"; //0b11 = 3 means RLE+golomb
	header += transposed;
	header += std::bitset<sizeof(uint32_t)*8>(res.length()).to_string();
	// Encode LUT
	std::string dic;
	for (auto val : LUT)
	{
		dic += std::bitset<sizeof(int)*8>(val).to_string();
	}

	// Concatenation
	std::string compressed = header + res + dic;
	float ratio = ((float) imSize)*((float) imSize)/((float) compressed.length());	

	std::cout << "Testing RLE with Exp-Golomb Coder:" << std::endl;
	std::cout << "Total size = "  << compressed.length() <<  " bits. ";
	std::cout << "Compression ratio is " << ratio << "." << std::endl;
 	std::cout << "Header = " << header.length() <<  " bits" << std::endl;
 	std::cout << "Data = " << res.length() <<  " bits" << std::endl;
 	std::cout << "Dictionnaries = " << dic.length() <<  " bits" << std::endl << std::endl;

 	return compressed;
}

void compress(std::string filename)
{
	std::cout << std::endl << "Starting compression of " << filename << "..." << std::endl << std::endl;
	std::cout << "Trying a horizontal scanning order..." << std::endl;

	// Load image and convert to chars
	std::vector<float> imagefloat = load<float>(filename);
	size_t imSize = sqrt(imagefloat.size());
	std::vector<unsigned char> image(imagefloat.begin(), imagefloat.end());
	std::vector<unsigned char> imageT = transpose(image, imSize);

	// Test several encoding schemes
	std::string RLEAthEncoded = encodeRLEAth(image, imSize, '0');
	std::string M2FAthEncoded = encodeM2FAth(image, imSize, '0');
	std::string RLEGbEncoded = encodeRLEGb(image, imSize, '0');
	std::string M2FTREAthEncoded = encode8M2FTREAth(image, imSize, '0');

	// Same but transposed
	std::cout << "Trying a vertical scanning order..." << std::endl;
	std::string RLEAthEncodedT = encodeRLEAth(imageT, imSize, '1');
	std::string M2FAthEncodedT = encodeM2FAth(imageT, imSize, '1');
	std::string RLEGbEncodedT = encodeRLEGb(imageT, imSize, '1');
	std::string M2FTREAthEncodedT = encode8M2FTREAth(imageT, imSize, '1');
	// Select best performance
	size_t min = -1;
	uint32_t minInd = 0;
	std::vector<std::string> vecMeth = {RLEAthEncoded, M2FAthEncoded, RLEGbEncoded,
		M2FTREAthEncoded,RLEAthEncodedT, M2FAthEncodedT, RLEGbEncodedT,M2FTREAthEncodedT};
	for (uint32_t i = 0; i < vecMeth.size(); ++i)
	 {
	 	if (vecMeth[i].size() <= min && vecMeth[i] != "-1")
	 	{
	 		min = vecMeth[i].size();
	 		minInd = i;
	 	}
	 } 
	std::string compressed = vecMeth[minInd];

	// Output chosen method
	std::cout << "The best method is ";
	switch(minInd)
	{
		case 0:
			std::cout << "RLE-Ath in horizontal scanning." << std::endl;
			break;
		case 1:
			std::cout << "MTF-Ath in horizontal scanning." << std::endl;
			break;
		case 2:
			std::cout << "RLE-Gb in horizontal scanning." << std::endl;
			break;
		case 3:
			std::cout << "Benzid in horizontal scanning." << std::endl;
			break;
		case 4:
			std::cout << "RLE-Ath in vertical scanning." << std::endl;
			break;
		case 5:
			std::cout << "MTF-Ath in vertical scanning." << std::endl;
			break;
		case 6:
			std::cout << "RLE-Gb in vertical scanning." << std::endl;
			break;
		case 7:
			std::cout << "Benzid in vertical scanning." << std::endl;
			break;
	}
	
	// Write to file
	std::string name = filename.substr(0,filename.length()-4);
	std::cout << "Output: " + name + ".jpp\n" << std::endl;
	std::ofstream outfile (name + ".jpp");
	outfile << compressed;
	outfile.close();
}


int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cerr << "Error using " << argv[0] << ". Correct usage is " << argv[0] << " IMAGE_NAME(.raw/.jpp)" << std::endl;
		return 1;
	}

	else if(std::string(argv[1]).substr(std::string(argv[1]).length()-4) == ".raw")
	{
		compress(argv[1]);
		return 0;
	}
	else if(std::string(argv[1]).substr(std::string(argv[1]).length()-4) == ".jpp")
	{
		return decompress(argv[1]);
		
	}
	else if (std::string(argv[1]) == "help")
	{
		std::cout << std::endl << "This is a bilevel image compressor. To use it, type:" << std::endl;
		std::cout << "./main imagename" << std::endl;
		std::cout << std::endl << "The file should be either a .raw file, which will be compressed, or a " 
				  << ".jpp file, which will be decompressed." << std::endl;
	}
	else
	{
		std::cerr << "Error using " << argv[0] << ". Unknown second argument, should be IMAGE_NAME.raw (compress) or IMAGE_NAME.jpp (decompress))" << std::endl;		
		return 2;
	}
}