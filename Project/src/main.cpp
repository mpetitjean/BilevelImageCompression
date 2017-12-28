#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <numeric>
#include <algorithm>
#include <map>
#include <bitset>
#include <set>
#include <deque>
#include <stdio.h>
#include <gmp.h>

#define LENGTH_1D	256

template <class T>
int store(std::string filename, std::vector<T> image)
{
	std::ofstream file (filename, std::ios::binary);
	if (file)
		file.write(reinterpret_cast<const char*>(image.data()), image.size() * sizeof(T));
	else
		std::cout << "Cannot write into " << filename << std::endl;
	file.close();
	return file.rdstate();
}

template <class T>
int load(std::string filename, std::vector<T>& image)
{	
	std::ifstream file (filename, std::ios::binary);
	if (file)
		file.read(reinterpret_cast<char*>(image.data()), image.size() * sizeof(T));
	else 
		std::cout << "Cannot read " << filename << std::endl;
	file.close();
	return file.rdstate();
}

std::vector<int> encode_rle(std::vector<char> image)
{
	/**
    Encodes the content of a bilevel vector into its RLE equivalent.
    Assumes 0 as initial value.

    @param vector only containing 1's or 0's
    @return a vector of the length of each run
	*/
	int sum = 0;
	char bit = 0;
	std::vector<int> output;
	output.reserve(image.size());
	for(unsigned int i=0; i < image.size(); ++i)
	{
		if (image[i] != bit)
		{
			output.push_back(sum);
			sum = 0;
			bit = !bit;
		}
		++sum;
	}
	output.push_back(sum);
	return output;
}

std::vector<char> decode_rle(std::vector<int> image)
{	
	/**
    Decodes back the output of the encode_rle function

    @param vector of run lengths
    @return a binary image
	*/
	char bit = 0;
	std::vector<char> output;
	output.reserve(image.capacity());
	for (int i : image)
	{
		for( ;i > 0; --i)
		{
			output.push_back(bit);
		}
		bit = !bit;
	}
	return output;
}

int toCSV(std::string filename, std::vector<float> data)
{
	std::ofstream file (filename);
	if (file)
		for (float i : data)
				file << std::to_string(i)<< std::endl;
	else
		std::cout << "Cannot write into " << filename << std::endl;
	file.close();
	return file.rdstate();
}

template <typename T>
std::vector<int> createLUT(const std::vector<T> &v)
{	
	/**
    Creates a LUT for entropy coding, its purpose being to map short
    codewords to probable symbols

    @param vector containing the number of occurences of each run length
    @return vector of which the values indicate to what symbol its index is
    		mapped. Example:
    		vector {2,0,1,3,4} means that the mapping is 0-1,1-0,2-1,3-3,4-4
	*/

	// initialize original index locations
	std::vector<int> idx(v.size());
	iota(idx.begin(), idx.end(), 0);

	// sort indexes based on comparing values in v
	sort(idx.begin(), idx.end(),
	   [&v](int i1, int i2) {return v[i1] > v[i2];});

	std::vector<int> LUT(v.size());

	// Fill LUT
	for (size_t i = 0; i <  v.size(); ++i)
	{
			LUT[idx[i]] = i;
	}
	return LUT;
}

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

std::vector<unsigned char> M2F(std::vector<unsigned char> image, std::deque<unsigned char> dictionnary)
{	
	/**
    Performs the Move-To-Front transform on a vector 'image' while updating 'dictionnary'.
    A deque is used to allow fast insertions

    @param 	image: input image
			dictionnary: a deque acting as the dictionnary for the MTF
    @return a vector being the result of the MTF
	*/
	std::vector<unsigned char> result;
	result.reserve(image.size());
	int index;
	int temp;
	for (auto num : image)
	{
		index = std::distance(begin(dictionnary), std::find(begin(dictionnary), end(dictionnary), num));
		result.push_back(index);
		temp = std::move(dictionnary[index]);
		dictionnary.erase(begin(dictionnary) + index);
		dictionnary.push_front(temp);
	}
	return result;
}

std::vector<unsigned char> iM2F(std::vector<unsigned char> image, std::deque<unsigned char> dictionnary)
{
	/**
    Performs the inverse Move-To-Front transform on a vector 'image' while updating 'dictionnary'.
    A deque is used to allow fast insertions

    @param 	image: input transformed image
			dictionnary: a deque acting as the dictionnary for the iMTF
    @return a vector being the result of the iMTF
	*/
	std::vector<unsigned char> result;
	result.reserve(image.size());
	int temp;
	for (auto num : image)
	{
		result.push_back(dictionnary[num]);
		temp = std::move(dictionnary[num]);
		dictionnary.erase(begin(dictionnary) + num);
		dictionnary.push_front(temp);
	}
	return result;
}

std::vector<unsigned int> TRE(std::vector<unsigned char> image)
{
	/**
    Apply a Two-Role-Encoder on a vector 'image'. The number of consecutive zeros 
    must be smaller than 256.

    @param 	input image
    @return a vector begin the result of the TRE
	*/
	std::vector<unsigned int> result;
	result.reserve(image.size());
	unsigned int index = 0;
	unsigned int count = 1;
	while (index < image.size())
	{
		if (image[index])
		{
			result.push_back(image[index] + 256);
			++index;
		}
		else
		{
			while (!image[index + count])
			{
				++count;
			}
			result.push_back(count);
			if (count > 255)
			{
				std::cout << "Too many consecutive zeros!" << std::endl;
			}
			index += count;
			count = 1;
		}
	}
	return result;
}

std::vector<unsigned char> iTRE(std::vector<unsigned int> image)
{
	/**
    Apply a Two-Role-Decoder on a vector 'image', inverse operation of TRE

    @param 	input TRE'd image
    @return a vector begin the result of the iTRE
	*/
	std::vector<unsigned char> result;
	result.reserve(image.size());
	unsigned int index = 0;
	while (index < image.size())
	{
		if (image[index] > 255)
		{
			result.push_back(image[index] - 256);
		}
		else
		{
			std::fill_n(back_inserter(result), image[index], 0);
		}
		++index;
	}
	return result;
}

std::map <unsigned int, double> nbOccurences(std::vector<unsigned int> encoded)
{
	/**
    Counts the number of occurences of each value in 'encoded'

    @param 	vector to be processed
    @return a map of which the key is a value of 'encoded' and the corresponding value is its number 
    		of occurences
	*/
	std::map <unsigned int, double> occ;
	std::set <unsigned int> values(begin(encoded), end(encoded));

	// Fill map with encoded values
	std::for_each(values.begin(), values.end(), [&occ, encoded](unsigned int val)
		{occ[val] = std::count(encoded.begin(), encoded.end(), val);});

	// normalize
	double sum = std::accumulate(occ.begin(), occ.end(), 0., [](double sum, std::pair<unsigned int, double> p)
		{return sum + p.second;});
	
	for (auto pair : occ)
	{
		occ[pair.first] /= sum;
	}
	return occ;
}

std::map<unsigned int, std::pair<double, double>> createIntervals(std::map <unsigned int, double> occ)
{	
	/**
	Create a map to register the intervals for each symbol in order to implement the arithmetic encoder and decoder

    @param 	the map of occurences as generated by the function 'nbOccurences'
    @return a map containing 1) a symbol and 2) a pair of its corresponding interval
	*/

	// One symbol ⟷ one pair [min, max)
	std::map<unsigned int, std::pair<double, double>> result;
	double high = 0.;
	double low = 0.;

	for (auto p : occ)
	{
		high += p.second;
		result[p.first] = std::pair<double, double>(low, high);
		low = high;
	}

	return result;
}	

void arithmeticEncoder(std::map<unsigned int, std::pair<double, double>> intervalsMap, std::vector<unsigned int> TREd, mpf_t& outbuff)
{
	/**
	Implementation of an arithmetic encoder using arbitrary precise numbers.
	The output is assigned by reference because it is not possible to output a type mpf_t

    @param	intervalsMap: map generated by 'createIntervals'
    		TREd: image to be encoded
    		outbuff: reference to an arbitrary precise number, output of the encoder
	*/
	mpf_t high, low, range;
	mpf_init_set_d(high, 1.);
	mpf_init_set_d(low, 0.);
	mpf_init(range);
	// mpf_init(temp);
	for (auto value : TREd)
	{
		mpf_sub(range, high, low);
		std::cout << range << std::endl;
		mpf_set_d(outbuff, intervalsMap[value].second);
		mpf_mul(outbuff, outbuff, range);
		mpf_add(high, low, outbuff);
		// high = low + range*intervalsMap[value].second;
		mpf_set_d(outbuff, intervalsMap[value].first);
		mpf_mul(outbuff, outbuff, range);
		mpf_add(low, low, outbuff);
		// low = low + range*intervalsMap[value].first;
	}
	mpf_sub(outbuff, high, low);
	mpf_div_ui(outbuff, outbuff ,2);
	mpf_add(outbuff, outbuff, low);
	mpf_clears(high, low, range, NULL);
	// mpf_set(outbuff, temp);
}

std::vector<unsigned int> arithmeticDecoder(mpf_t encoded, std::map<unsigned int, std::pair<double, double>> intervalsMap, size_t size)
{
	/**
	Implementation of an arithmetic decoder using arbitrary precise numbers.

    @param	encoded: arbitrary precise number, output of the encoder
    		intervalsMap: map generated by 'createIntervals'
    		size: size of the image
	@return the decoded vector
	*/

	mpf_t high, low, range, temp;
	mpf_init_set_d(high, 1.);
	mpf_init_set_d(low, 0.);
	mpf_inits(range, temp, NULL);
	std::vector<unsigned int> decoded;

	do
	{
		mpf_sub(range, high, low);
		mpf_sub(temp, encoded, low);
		mpf_div(temp, temp, range);
		// temp = (encoded - low)/range;

		for (auto val : intervalsMap)
		{
			if (mpf_cmp_d(temp, val.second.first) >= 0 && mpf_cmp_d(temp, val.second.second) < 0) //if (temp >= val.second.first && temp < val.second.second)
			{
				decoded.push_back(val.first);
				// std::cout << val.first << std::endl;
				break;
			}
		}
		mpf_set_d(temp, intervalsMap[decoded.back()].second);
		mpf_mul(temp, temp, range);
		mpf_add(high, low, temp);
		// high = low + range*intervalsMap[decoded.back()].second;
		mpf_set_d(temp, intervalsMap[decoded.back()].first);
		mpf_mul(temp, temp, range);
		mpf_add(low, low, temp);
		// low = low + range*intervalsMap[decoded.back()].first;
		// std::cout << "left: " << decoded.size() << std::endl;

	}
	while(decoded.back() != 0);
	mpf_clears(high, low, range, temp, NULL);

	return decoded;
}


int main()
{
	std::vector<float> imagefloat(256*256);
	load("earth_binary_256x256.raw", imagefloat);
	std::vector<unsigned char> image(imagefloat.begin(), imagefloat.end());
	
	// MTF
	mpf_set_default_prec(25000);
	//1) Shrink image in column
	std::vector<unsigned char> shrinked = shrinkColumnTo8bpp(image);

	std::set<unsigned char> dictionnary_set(begin(shrinked), end(shrinked));
	std::deque<unsigned char> dictionnary(dictionnary_set.begin(), dictionnary_set.end());
	
	std::vector<unsigned char> coeff = M2F(shrinked, dictionnary);

	std::vector<unsigned int> run_length = TRE(coeff);
	std::cout << run_length.size() << std::endl;
	// for (auto val : run_length)
	// 	std::cout << val << std::endl;
	// std::vector<unsigned int> test = {1, 2, 3, 4, 3, 3, 1, 5, 0};
	run_length.push_back(0);
	std::map<unsigned int, double> dico = nbOccurences(run_length);
	std::map<unsigned int, std::pair<double, double>> valmap = createIntervals(dico);
	std::cout << valmap.size() << std::endl;
	mpf_t res;
	mpf_init(res);
	arithmeticEncoder(valmap, run_length, res);
	std::cout << "Encoded image = " << res << std::endl;
	FILE* outfile;
	outfile = fopen("compressed.raw","w");
	mpf_out_str(outfile, 2, 0, res);
	fclose(outfile);

	// mpf_out_str()
	std::vector<unsigned int> rtest = arithmeticDecoder(res, valmap, run_length.size()); 		
	mpf_clear(res);
	rtest.pop_back();
	std::cout << "Decoded image = ";
	for(auto i : rtest)
		std::cout << i << ", ";
	std::cout << std::endl;
	

	std::vector<unsigned char> coeff2 = iTRE(rtest);

	std::vector<unsigned char> resolve = iM2F(coeff2, dictionnary);
	std::vector<unsigned char> result = ExpandColumnFrom8bpp(resolve);

	
	

	// for (auto val : rtest)
	// 	std::cout << val << std::endl;

	store("result.raw", result);


	return 0;
}