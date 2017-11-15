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
		return 1;
	}
}

template <class T> struct sqminus {
  T operator() (const T& x, const T& y) const {return pow(x-y,2);}
  typedef T first_argument_type;
  typedef T second_argument_type;
  typedef T result_type;
};

std::vector<float> createConstImage(int rows = ROWS, int cols = COLS, float value = 0.5)
{
	return std::vector<float>(rows*cols, value) ;
}

std::vector<float> createUniformRandomImage(int rows = ROWS, int cols = COLS)
{
	std::default_random_engine gen;
	std::uniform_real_distribution<float> dis(0.0, 1.0);
	std::vector<float> image(rows*cols);
	std::generate(image.begin(), image.end(), [&](){ return dis(gen); });
	return image;
}

std::vector<float> createNormalRandomImage(int rows = ROWS, int cols = COLS, float mean = 0.0, float std = 1.0)
{
	std::default_random_engine gen;
	std::normal_distribution<float> dis(mean, std);
	std::vector<float> image(rows*cols);
	std::generate(image.begin(), image.end(), [&](){ return dis(gen);});
	return image;
}

float psnr(std::vector<float> image, std::vector<float> ref, float max)
{
	return 10*log10(max*max*image.size()/std::inner_product(image.begin(), image.end(), ref.begin(), 0.0, std::plus<float>(), sqminus<float>()));
}

int main()
{
	std::vector<float> constImage = createConstImage();
	std::vector<float> uniformImage = createUniformRandomImage();
	std::vector<float> normalImage = createNormalRandomImage();
	store("constImage.raw", constImage);
	store("uniformImage.raw", uniformImage);
	store("normalImage.raw", normalImage);
	printf("PSNR between constant and uniform random distributed is %.2f dB\n", psnr(constImage, uniformImage, 1.0));

	return 0;
}