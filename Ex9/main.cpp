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

std::vector<float> create_Q()
{
	std::vector<float> buffer{{16, 11, 10, 16, 24, 40, 51, 61, 12, 12, 14, 19, 
		26, 58, 60, 55, 14, 13, 16, 24, 40, 57, 69, 56, 14, 17, 22, 29, 51, 87, 
		80, 62, 18, 22, 37, 56, 68, 109, 103, 77, 24, 35, 55, 64, 81, 104, 113, 
		92, 49, 64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99}};
	return buffer;
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

std::vector<float> approximateBlock(std::vector<float> block, std::vector<float> tcoef, std::vector<float> fcoef, std::vector<float> Q)
{
	std::vector<int> temp(Q.size());
	block = transform(block, tcoef);

	std::transform(block.begin(), block.end(), Q.begin(), temp.begin(), std::divides<float>());
	std::transform(temp.begin(), temp.end(), temp.begin(), round);
	std::transform(temp.begin(), temp.end(), Q.begin(), block.begin(), std::multiplies<float>());

	return transform(block, fcoef);
}

std::vector<float> approximate(std::vector<float> image, std::vector<float> tcoef, std::vector<float> fcoef, std::vector<float> Q)
{
	int block_size = sqrt(Q.size());
	int length = sqrt(image.size());
	std::vector<float> result(image.size());
	int nbframe = length / block_size;
	std::vector<float> block(Q.size());

	for (int step = 0; step < nbframe * nbframe; ++step)
	{
		int off_col = (step % nbframe) * block_size;
		int off_row = step / nbframe * length * block_size;


		for (int i = 0; i < block_size; ++i)
		{
		 	std::copy_n(image.begin() + i * length + off_row + off_col, block_size, block.begin() + i * block_size);
		}

		block = approximateBlock(block, tcoef, fcoef, Q);

		for (int i = 0; i < block_size; ++i)
		{
		 	std::copy_n(block.begin() + i * block_size, block_size, result.begin() + i * length + off_row + off_col);
		}
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
	std::vector<float> Q = create_Q();
	store("Q.raw", Q);

	std::vector<float> DCT_vectors = DCT_vectors_basis(sqrt(Q.size()));
	std::vector<float> IDCT_vectors = transpose(DCT_vectors, sqrt(DCT_vectors.size()));
	
	std::vector<float> lena(LENGTH_1D * LENGTH_1D);
	load("lena_256x256.raw", lena);

	std::vector<float> lenaJPEG = approximate(lena, DCT_vectors, IDCT_vectors, Q);
	store("lenaJPEG.raw", lenaJPEG);
	std::vector<uint8_t> lenaJPEG8bpp = quantize8bpp(lenaJPEG);
	store("lenaJPEG8bpp.raw", lenaJPEG8bpp);
	std::cout << psnr(lenaJPEG, lena) << std::endl;
	return 0;
}
