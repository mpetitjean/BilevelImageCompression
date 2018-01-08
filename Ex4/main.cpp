#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <functional>
#include <numeric>
#include <random>
#include <algorithm>

#define ROWS 256
#define COLS 256

int store(std::string filename, std::vector<float> image)
{
	std::ofstream file (filename, std::ios::binary);
	if (file)
	{
		file.write(reinterpret_cast<const char*>(image.data()), image.size() * sizeof(float));
		file.close();
		return 0;
	}
	else
	{
		std::cout << "Cannot write into " << filename;
		file.close();
	} 
		return 1;
}

int load(std::string filename, std::vector<float>& image)
{	
	std::ifstream file (filename, std::ios::binary);
	if (file)
	{
		file.read(reinterpret_cast<char*>(image.data()), image.size() * sizeof(float));
		file.close();
		return 0;
	}
	else 
	{
		std::cout << "Cannot read " << filename;
		file.close();
		return 1;
	}
}

std::vector<float> normalize8bpp(std::vector<float> image)
{
	std::transform(image.begin(), image.end(), image.begin(), [](float val) { return val / 255; });
	return image;
}



template <class T> struct sqminus {
  T operator() (const T& x, const T& y) const {return pow(x-y,2);}
  typedef T first_argument_type;
  typedef T second_argument_type;
  typedef T result_type;
};

float psnr(std::vector<float> image, std::vector<float> ref, float max)
{
	return 10*log10(max*max*image.size()/std::inner_product(image.begin(), image.end(), ref.begin(), 0.0, std::plus<float>(), sqminus<float>()));
}

void normalize(std::vector<float>& image)
{
	for(size_t i = 0; i < image.size(); ++i)
	{
		image[i] = (image[i] < 0.) ? 0. : ((image[i] > 1.) ? 1. : image[i]);
	}
}

std::vector<float> addGaussianNoise(std::vector<float> image, float std = 0.024)
{
	std::default_random_engine gen;
	std::normal_distribution<float> dis(0, std);
	std::transform(image.begin(), image.end(), image.begin(), [&](float val){ return val + dis(gen);});
	return image;
}

int main()
{
	std::vector<float> lena(ROWS*COLS);
	load("lena_256x256.raw", lena);
	std::vector<float> lena8 = normalize8bpp(lena);
	std::vector<float> noisyLena8 = addGaussianNoise(lena8, 0.2);	
	normalize(noisyLena8);
	store("lena8.raw", lena8);
	store("lena8noisy.raw", noisyLena8);
	printf("psnr: %5f dB\n", psnr(lena8, noisyLena8, 1.));
	return 0;
}