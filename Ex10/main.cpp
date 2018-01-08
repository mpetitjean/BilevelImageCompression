	#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <numeric>
#include <algorithm>

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

template <class T> struct sqminus {
  T operator() (const T& x, const T& y) const {return pow(x-y,2);}
  typedef T first_argument_type;
  typedef T second_argument_type;
  typedef T result_type;
};

float psnr(std::vector<float> image, std::vector<float> ref, float max = 255)
{
	return 10*log10(max*max*image.size()/std::inner_product(image.begin(), image.end(), ref.begin(), 0.0,
	 std::plus<float>(), sqminus<float>()));
}

std::vector<float> DCT_vectors_basis(int length = LENGTH_1D)
{
	std::vector<float> DCT_vectors(length * length);
	for (int k = 0; k < length; ++k)
	{
	    double s = (k == 0) ? sqrt(.5) : 1.;
	    for (int n = 0; n < length; ++n)
	    {
	    	DCT_vectors[k * length + n]= s * cos(M_PI * (n + .5) * k / length);
	    }
	}
  return DCT_vectors;
}

std::vector<float> transpose(std::vector<float> image, int length)
{
	for(int i = 0; i < length; ++i)
	    for(int j = i+1; j < length; ++j)
	        std::swap(image[length*i + j], image[length*j + i]);
	return image;
}

std::vector<float> transform1D(std::vector<float> image, std::vector<float> basis, int length)
{
	std::vector<float> image_DCT(length * length);
	for (int row = 0; row < length; ++row)
	{
		for (int col = 0; col < length; ++col)
		{
			image_DCT[row * length + col] = std::inner_product(image.begin() + row * length, 
				image.begin() + row * length + length, basis.begin() + col * length, 0.) * sqrt(2) / sqrt(length);

		}
		
	}
	return image_DCT;
}

std::vector<float> transform(std::vector<float> image, std::vector<float> basis)
{
	int length = sqrt(image.size());
	std::vector<float> temp = transform1D(image, basis, length);
	std::vector<float> transformed(image.size());
	for (int row = 0; row < length; ++row)
	{
		for (int col = 0; col < length; ++col)
		{
			float sum = 0;
			for (int k = 0; k < length; ++k)
			{
				sum += temp[row + k*length] * basis[k + col*length];
			}
			transformed[col + row*length] = sum * sqrt(2) / sqrt(length);
		}
	}
	return transformed;
}

std::vector<float> threshold(std::vector<float> image, float value = 1e-10)
{
	std::replace_if (image.begin(), image.end(), [&value](float i){return abs(i) < value;}, 0);
	return image;
}

std::vector<float> approximateBlock(std::vector<float> block, std::vector<float> tcoef, int block_size)
{
	std::vector<float> temp(block_size * block_size);
	return transform(block, tcoef);
}

std::vector<float> approximate(std::vector<float> image, std::vector<float> tcoef, int block_size = 8, bool intervaled=0)
{
	int length = sqrt(image.size());
	std::vector<float> result(image.size());
	std::vector<float> temp(image.size());
	int nbframe = length / block_size;
	std::vector<float> block(block_size * block_size);

	for (int step = 0; step < nbframe * nbframe; ++step)
	{
		int off_col = (step % nbframe) * block_size;
		int off_row = step / nbframe * length * block_size;


		for (int i = 0; i < block_size; ++i)
		{
		 	std::copy_n(image.begin() + i * length + off_row + off_col, block_size, block.begin() + i * block_size);
		}

		block = approximateBlock(block, tcoef, block_size);

		for (int i = 0; i < block_size; ++i)
		{
		 	std::copy_n(block.begin() + i * block_size, block_size, result.begin() + i * length + off_row + off_col);
		}
	}
	if(intervaled)
	{
		for (int step = 0; step < block_size*block_size; ++step)
		{
			for (int row = 0; row < nbframe; ++row)
			{
				for (int col = 0; col < nbframe; ++col)
				{
					temp[row * length + col + (step%block_size)*nbframe + step/block_size*length*nbframe] = 
					result[(step%block_size) + (step / block_size) * length + col * block_size + row * length * block_size];
				}
			}
		}
		return temp;
	}

	return result;

}

std::vector<uint8_t> quantize8bpp(std::vector<float> image)
{
	std::vector<uint8_t> nImage(image.size());
	std::transform(image.begin(), image.end(), nImage.begin(), [](float pixel){return (uint8_t) std::max(0., std::min(255., round(pixel)));});
	return nImage;
}

int main()
{
	std::vector<float> DCT_vectors = DCT_vectors_basis(8);

	std::vector<float> lena(LENGTH_1D * LENGTH_1D);
	load("lena_256x256.raw", lena);

	std::vector<float> lenaJPEG = approximate(lena, DCT_vectors, 8, 1);
	store("lenaJPEG.raw", lenaJPEG);
	return 0;
}
