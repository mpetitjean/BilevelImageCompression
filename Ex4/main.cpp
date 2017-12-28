#include <iostream>
#include <fstream>
#include <random>

#define HEIGHT 	256
#define WIDTH 	256						

void load(std::string filename, float * buffer)
{
	std::ifstream is (filename, std::ifstream::binary);
	
	if(is)
	{
		is.read (reinterpret_cast<char*> (buffer), HEIGHT*WIDTH*sizeof(float));
    	is.close();	
	}
	else
	{
		std::cout << "Error opening file.\n";
	}    
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

void normalize8bpp(float * bufferIn, float * bufferOut)
{
	for (int i = 0; i < HEIGHT*WIDTH; i++)
	{
		bufferOut[i] = bufferIn[i]/255;
	}
}

void addGaussianNoise(float * bufferIn, float * bufferOut, float mean, float stdev)
{
	std::default_random_engine generator;
  	std::normal_distribution<float> distribution(mean, stdev);

	for (int h = 0; h < HEIGHT; h++)
	{
	    for (int w = 0; w < WIDTH; w++)
	    {
	        bufferOut[w + h*WIDTH] = bufferIn[w + h*WIDTH] + distribution(generator);

	        // stay within [0;1]
	        if (bufferOut[w + h*WIDTH] > 1.0)
	        	bufferOut[w + h*WIDTH] = 1.0;
	        else if (bufferOut[w + h*WIDTH] < 0.0)
	        	bufferOut[w + h*WIDTH] = 0.0;
	    }
	}
}

float computePsnr(float* image_noisy, float* image_ref, float max)
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

int main()
{
	std::cout << "_______________________\n\n";

	// load lena 32bpp
	float * lena = new float[HEIGHT*WIDTH];
	load("lena_256x256.raw", lena);

	// Convert to [0;1]
	float * lena01 = new float[HEIGHT*WIDTH];
	normalize8bpp(lena, lena01);

	// Add noise
	float * lenaNoisy1 = new float[HEIGHT*WIDTH];
	float * lenaNoisy2 = new float[HEIGHT*WIDTH];
	float * lenaNoisy3 = new float[HEIGHT*WIDTH];
	float * lenaNoisy4 = new float[HEIGHT*WIDTH];
	float sigma1 = 0.01, sigma2 = 0.05, sigma3 = 0.1, sigma4 = 0.2;
	addGaussianNoise(lena01, lenaNoisy1, 0, sigma1);
	addGaussianNoise(lena01, lenaNoisy2, 0, sigma2);
	addGaussianNoise(lena01, lenaNoisy3, 0, sigma3);
	addGaussianNoise(lena01, lenaNoisy4, 0, sigma4);

	store(lenaNoisy1, "noisy1.raw");
	store(lenaNoisy2, "noisy2.raw");
	store(lenaNoisy3, "noisy3.raw");
	store(lenaNoisy4, "noisy4.raw");
	float psnr1 = computePsnr(lena01, lenaNoisy1, 1.0);
	float psnr2 = computePsnr(lena01, lenaNoisy2, 1.0);
	float psnr3 = computePsnr(lena01, lenaNoisy3, 1.0);
	float psnr4 = computePsnr(lena01, lenaNoisy4, 1.0);
	std::cout << "PSNR is " <<  psnr1 << " dB when stdev is " << sigma1 << std::endl;
	std::cout << "PSNR is " <<  psnr2 << " dB when stdev is " << sigma2 << std::endl;
	std::cout << "PSNR is " <<  psnr3 << " dB when stdev is " << sigma3 << std::endl;
	std::cout << "PSNR is " <<  psnr4 << " dB when stdev is " << sigma4 << std::endl;

	// Free memory
	delete lena;
	delete lena01;
	delete lenaNoisy1;
	delete lenaNoisy2;
	delete lenaNoisy3;
	delete lenaNoisy4;
	return 0;
}