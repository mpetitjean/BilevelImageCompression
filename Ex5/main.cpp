#include <iostream>
#include <fstream>
#include <random>

#define HEIGHT 	256
#define WIDTH 	256
#define KHEIGHT 3
#define KWIDTH	3

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

void storeFloat(float* arrayIn)
{
	std::ofstream outfile;
	outfile.open("outFloat.raw", std::ios::out | std::ios::binary);

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

void addGaussianNoise(float * bufferIn, float * bufferOut)
{
	std::default_random_engine generator;
  	std::normal_distribution<float> distribution(0, 0.024);

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

	        printf("%.2f\n", bufferOut[w + h*WIDTH]);
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

void convolve(float * image, float * kernel, float * result)
{
	int mm, nn, ii, jj;
	int KCenterHeight = KHEIGHT / 2;
	int KCenterWidth = KWIDTH / 2;

	for(int i = 0; i< HEIGHT * WIDTH; i++)
		result[i] = 0;

	for(int i = 0; i < HEIGHT; i++)              // rows
	    for(int j = 0; j < WIDTH; j++)          // columns
	        for(int m = 0; m < KHEIGHT; m++)     // kernel rows
	        {
	            mm = KHEIGHT - 1 - m;      // row index of flipped kernel

	            for(int n = 0; n < KWIDTH; n++) // kernel columns
	            {
	                nn = KWIDTH - 1 - n;  // column index of flipped kernel

	                // index of input signal, used for checking boundary
	                ii = i + (m - KCenterHeight);
	                jj = j + (n - KCenterWidth);

	                // ignore input samples which are out of bound
	                if( ii >= 0 && ii < HEIGHT && jj >= 0 && jj < WIDTH )
	                    result[i * WIDTH + j] += image[ii * WIDTH + jj] * kernel[mm * KWIDTH + nn];
	            }
	        }
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
	float * lenaNoisy = new float[HEIGHT*WIDTH];
	addGaussianNoise(lena01, lenaNoisy);

	storeFloat(lenaNoisy);
	float psnr = computePsnr(lena01, lenaNoisy, 1.0);
	printf("PSNR is %.2f dB\n", psnr); 

	// Free memory
	delete lena;
	delete lena01;
	delete lenaNoisy;
	return 0;
}
