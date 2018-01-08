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

void normalize(std::vector<float>& image)
{
	for(size_t i = 0; i < image.size(); ++i)
	{
		image[i] = (image[i] < 0.) ? 0. : ((image[i] > 1.) ? 1. : image[i]);
	}

}

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

std::vector<float> createNormalRandomImage(int rows = ROWS, int cols = COLS, float mean = 0.5, float std = 0.29)
{
	std::default_random_engine gen;
	std::normal_distribution<float> dis(mean, std);
	std::vector<float> image(rows*cols);
	std::generate(image.begin(), image.end(), [&](){ return dis(gen);});
	return image;
}

float mse(std::vector<float> image, std::vector<float> ref)
{
	return std::inner_product(image.begin(), image.end(), ref.begin(), 0.0, std::plus<float>(), sqminus<float>())/image.size();
}

int main()
{
	std::vector<float> constImage = createConstImage();
	std::vector<float> uniformImage = createUniformRandomImage();
	std::vector<float> normalImage = createNormalRandomImage();
	//normalize(normalImage);
	store("constImage.raw", constImage);
	store("uniformImage.raw", uniformImage);
	store("normalImage.raw", normalImage);
	printf("MSE between constant and uniform random distributed is %.5f\n", mse(constImage, normalImage));

	return 0;
}