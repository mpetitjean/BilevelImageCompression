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
	// initialize original index locations
	std::vector<int> idx(v.size());
	iota(idx.begin(), idx.end(), 0);

	// sort indexes based on comparing values in v
	sort(idx.begin(), idx.end(),
	   [&v](int i1, int i2) {return v[i1] > v[i2];});

	std::vector<int> LUT(v.size());

	for (size_t i = 0; i <  v.size(); ++i)
	{
			LUT[idx[i]] = i;
	}
	return LUT;
}

std::vector<unsigned char> shrinkColumnTo8bpp(std::vector<unsigned char> binaryImage)
{
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

	// for (auto val : result)
	// {
	// 	std::cout << val.first << " : (" << val.second.first << ", " << val.second.second << ")" << std::endl;
	// }

	return result;
}	

void arithmeticEncoder(std::map<unsigned int, std::pair<double, double>> intervalsMap, std::vector<unsigned int> TREd, mpf_t& outbuff)
{
	mpf_t high, low, range, temp;
	mpf_init_set_d(high, 1.);
	mpf_init_set_d(low, 0.);
	mpf_init(temp);
	for (auto value : TREd)
	{
		mpf_sub(range, high, low);
		std::cout << range << std::endl;
		mpf_set_d(temp, intervalsMap[value].second);
		mpf_mul(temp, temp, range);
		mpf_add(high, low, temp);
		// high = low + range*intervalsMap[value].second;
		mpf_set_d(temp, intervalsMap[value].first);
		mpf_mul(temp, temp, range);
		mpf_add(low, low, temp);

		// low = low + range*intervalsMap[value].first;
	}
	mpf_sub(temp, high, low);
	mpf_div_ui(temp, temp ,2);
	mpf_add(temp, temp, low);
	mpf_set(outbuff, temp);
}

std::vector<unsigned int> arithmeticDecoder(mpf_t encoded, std::map<unsigned int, std::pair<double, double>> intervalsMap)
{
	// IMPORTANT: '0' is EOF character
	// http://marknelson.us/2014/10/19/data-compression-with-arithmetic-coding/

	mpf_t high, low, range, temp;
	mpf_init_set_d(high, 1.);
	mpf_init_set_d(low, 0.);
	mpf_init(temp);
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
				std::cout << val.first << std::endl;
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

	}
	while(decoded.back() != 0);

	return decoded;
}


int main()
{
	std::vector<float> imagefloat(256*256);
	load("earth_binary_256x256.raw", imagefloat);
	std::vector<unsigned char> image(imagefloat.begin(), imagefloat.end());
	
	// MTF
	__float128 a = 1.0;
	
	//1) Shrink image in column
	std::vector<unsigned char> shrinked = shrinkColumnTo8bpp(image);

	std::set<unsigned char> dictionnary_set(begin(shrinked), end(shrinked));
	std::deque<unsigned char> dictionnary(dictionnary_set.begin(), dictionnary_set.end());
	
	std::vector<unsigned char> coeff = M2F(shrinked, dictionnary);

	std::vector<unsigned int> run_length = TRE(coeff);

	// for (auto val : run_length)
	// 	std::cout << val << std::endl;

	std::map<unsigned int, double> dico = nbOccurences(run_length);
	run_length.push_back(0);
	std::map<unsigned int, std::pair<double, double>> valmap = createIntervals(dico);
	mpf_t res;
	arithmeticEncoder(valmap, run_length, res);
	std::cout << "Encoded image = " << res << std::endl;

	std::vector<unsigned int> rtest = arithmeticDecoder(res, valmap); 		
	rtest.pop_back();

	std::vector<unsigned char> coeff2 = iTRE(rtest);

	std::vector<unsigned char> resolve = iM2F(coeff2, dictionnary);
	std::vector<unsigned char> result = ExpandColumnFrom8bpp(resolve);

	// std::vector<unsigned int> test = {1, 2, 3, 4, 3, 3, 1, 5, 0};

	

	// for (auto val : rtest)
	// 	std::cout << val << std::endl;

	store("result.raw", result);


	return 0;
}