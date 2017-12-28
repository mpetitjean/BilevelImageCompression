#include <iostream>
#include <cmath>
#include <random>
#include <fstream>

#define HEIGHT 	256
#define WIDTH 	256


float psnr(float* image_noisy, float* image_ref, float max)
{
	float mse = 0;
	for (int i = 0; i < WIDTH; i++)
	{
		for (int j = 0; j < HEIGHT; j++)
		{
			mse += pow((image_noisy[j+i*WIDTH] - image_ref[j+i*WIDTH]),2);
		}
	}
	mse /= HEIGHT*WIDTH;

	return 10*log10(max*max/mse);
}

float computeMSE(float* image_noisy, float* image_ref)
{
	float mse = 0;
	for (int i = 0; i < WIDTH; i++)
	{
		for (int j = 0; j < HEIGHT; j++)
		{
			mse += pow((image_noisy[j+i*WIDTH] - image_ref[j+i*WIDTH]),2);
		}
	}
	mse /= HEIGHT*WIDTH;

	return mse;
}

void store(float* arrayIn, std::string filename)
{
	std::ofstream outfile;
	outfile.open(filename, std::ios::out | std::ios::binary);

	if (outfile.is_open()) 
	{
		outfile.write(reinterpret_cast<const char*>(arrayIn), HEIGHT*WIDTH*sizeof(float));
	}
	outfile.close();
}

float* createConstantImage(int height, int width, float value)
{

	float* array2D = new float[height*width];
	for (int h = 0; h < height; h++)
	{
	    for (int w = 0; w < width; w++)
	    {
	        array2D[w + h*height] = value;
	    }
	}
	return array2D;
}

float* createUniformRandomImage(int height, int width)
{
	std::default_random_engine generator;
  	std::uniform_real_distribution<float> distribution(0.0,1.0);

	float* array2D = new float[height*width];
	for (int h = 0; h < height; h++)
	{
	    for (int w = 0; w < width; w++)
	    {
	        array2D[w + h*width] = distribution(generator);
	    }
	}
	return array2D;
}

float* createGaussianImage(int height, int width, float mean, float stdev)
{
	std::default_random_engine generator;
  	std::normal_distribution<float> distribution(mean, stdev);

	float* array2D = new float[height*width];
	for (int h = 0; h < height; h++)
	{
	    for (int w = 0; w < width; w++)
	    {
	        array2D[w + h*width] = distribution(generator);
	    }
	}
	return array2D;
}


int main(void)
{	
	printf("____________________________\n\n");
	float* constantImage = createConstantImage(HEIGHT, WIDTH, 0.5);
	store(constantImage, "constant.raw");

	float* uniformRandomImage = createUniformRandomImage(HEIGHT, WIDTH);
	store(uniformRandomImage, "uniformRandom.raw");

	float* gaussianImage = createGaussianImage(HEIGHT, WIDTH, 0.5, 0.288);
	store(gaussianImage, "gaussian.raw");

	float mseU = computeMSE(constantImage, uniformRandomImage);
	float mseG = computeMSE(constantImage, gaussianImage);

	std::cout << "The MSE between the constant image and the uniform realization is " << mseU << std::endl;
	std::cout << "The MSE between the constant image and the gaussian realization is " << mseG << std::endl;

	return 0;
}