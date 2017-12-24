#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <numeric>
#include <algorithm>
#include <bitset>
#include <set>
#include <deque>

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

std::vector<float> normalize(std::vector<float> P)
{
	float sum = std::accumulate(P.begin(), P.end(), 0.);
	std::transform(P.begin(), P.end(), P.begin(), [sum](float val){return val/sum;});
	return P;
}

std::vector<float> nbOccurences(std::vector<int> encoded)
{
	std::vector<float> occ(*std::max_element(encoded.begin(), encoded.end()) + 1, 0.);
	for (int i : encoded)
		occ[i]++;
	return occ;
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
			++index;
		}
		else
		{
			std::fill_n(back_inserter(result), image[index], 0);
		}
	}
	return result;
}


int main()
{
	std::vector<float> imagefloat(256*256);
	load("lena_binary_dithered_256x256.raw", imagefloat);
	std::vector<unsigned char> image(imagefloat.begin(), imagefloat.end());
	
	// MTF

	//1) Shrink image in column
	std::vector<unsigned char> shrinked = shrinkColumnTo8bpp(image);

	std::set<unsigned char> dictionnary_set(begin(shrinked), end(shrinked));
	std::deque<unsigned char> dictionnary(dictionnary_set.begin(), dictionnary_set.end());
	
	std::vector<unsigned char> coeff = M2F(shrinked, dictionnary);

	std::vector<unsigned int> run_length = TRE(coeff);
	std::vector<unsigned char> coeff2 = iTRE(run_length);

	std::vector<unsigned char> resolve = iM2F(coeff2, dictionnary);
	std::vector<unsigned char> result = ExpandColumnFrom8bpp(resolve);

	store("result.raw", result);


	return 0;
}