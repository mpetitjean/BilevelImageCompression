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

std::vector<float> create_kernel()
{
	std::vector<float> kernel(9);
	float elem00 = 1;
	float elem10 = sqrt(exp(-1));
	float elem11 = exp(-1);

	
	float sum = elem00 + 4*elem10 + 4*elem11;
	elem00 /= sum;
	elem10 /= sum;
	elem11 /= sum;

	kernel[0] = elem11;
	kernel[1] = elem10;
	kernel[2] = elem11;
	kernel[3] = elem10;
	kernel[4] = elem00;
	kernel[5] = elem10;
	kernel[6] = elem11;
	kernel[7] = elem10;
	kernel[8] = elem11;

	return kernel;
}

std::vector<float> unsharp_masking(std::vector<float> image, std::vector<float> blurred)
{
	std::vector<float> result(image.size());
	for (size_t i = 0; i < image.size(); ++i)
	{
		result[i] = 2*image[i] - blurred[i]; 

		result[i] = (result[i] < 0.) ? 0. : ((result[i] > 1.) ? 1. : result[i]);
	}
	return result;
}


std::vector<float> blur(std::vector<float> image, std::vector<float> kernel)
{
	int mm, nn, ii, jj;
	int ksize = sqrt(kernel.size());
	int isize = sqrt(image.size());
	int KKCenterRow = ksize / 2;
	int KCenterCol = ksize / 2;
	std::vector<float> result(image.size());

	for(unsigned int i = 0; i< image.size(); ++i)
		result[i] = 0;

	for(int i = 0; i < isize; ++i)              // rows
	    for(int j = 0; j < isize; ++j)           // columns
	        for(int m = 0; m < ksize; ++m)     // kernel rows
	        {
	            mm = ksize - 1 - m;      // row index of flipped kernel

	            for(int n = 0; n < ksize; ++n) // kernel columns
	            {
	                nn = ksize - 1 - n;  // column index of flipped kernel

	                // index of input signal, used for checking boundary
	                ii = i + (m - KKCenterRow);
	                jj = j + (n - KCenterCol);

	                if(ii < 0)
	                    ii=ii+1;
	                if(jj < 0)
	                    jj=jj+1;
	                if(ii >= isize)
	                    ii=ii-1;
	                if(jj >= isize)
	                    jj=jj-1;    
	                if( ii >= 0 && ii < isize && jj >= 0 && jj < isize)
	                    result[i + isize*j] += image[ii + isize*jj] * kernel[mm + ksize*nn];
		            }
	        }
	return result;
}

int main()
{
	std::vector<float> lena(ROWS*COLS);
	load("lena_256x256.raw", lena);
	std::vector<float> lena8 =  normalize8bpp(lena);


	std::vector<float> kernel = create_kernel();


	std::vector<float> blurred  = blur(lena8, kernel);

	printf("PSNR blurred: %.5f dB\n", psnr(lena8, blurred, 1.0));
	store("blurred.raw", blurred);


	std::vector<float> sharped = unsharp_masking(lena8, blurred);
	store("sharped.raw", sharped);
	printf("PSNR sharped: %.5f dB\n", psnr(lena8, sharped, 1.0));
	return 0;
}