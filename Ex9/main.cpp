#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <numeric>
#include <algorithm>

#define LENGTH_1D	256 



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

template <class T> struct sqminus {
  T operator() (const T& x, const T& y) const {return pow(x-y,2);}
  typedef T first_argument_type;
  typedef T second_argument_type;
  typedef T result_type;
};

float psnr(std::vector<float> image, std::vector<float> ref, float max)
{
	return 10*log10(max*max*image.size()/std::inner_product(image.begin(), image.end(), ref.begin(), 0.0,
	 std::plus<float>(), sqminus<float>()));
}

std::vector<float> create_Q(float * buffer)
{
	std::vector<float> buffer(64);
	buffer[0] = 16;
	buffer[1] = 11;
	buffer[2] = 10;
	buffer[3] = 16;	
	buffer[4] = 24;
	buffer[5] = 40;
	buffer[6] = 51;
	buffer[7] = 61;
	buffer[8] = 12;
	buffer[9] = 12;
	buffer[10] = 14;
	buffer[11] = 19;
	buffer[12] = 26;
	buffer[13] = 58;
	buffer[14] = 60;
	buffer[15] = 55;
	buffer[16] = 14;
	buffer[17] = 13;
	buffer[18] = 16;
	buffer[19] = 24;
	buffer[20] = 40;
	buffer[21] = 57;
	buffer[22] = 69;
	buffer[23] = 56;
	buffer[24] = 14;
	buffer[25] = 17;
	buffer[26] = 22;
	buffer[27] = 29;
	buffer[28] = 51;
	buffer[29] = 87;
	buffer[30] = 80;
	buffer[31] = 62;
	buffer[32] = 18;
	buffer[33] = 22;
	buffer[34] = 37;
	buffer[35] = 56;
	buffer[36] = 68;
	buffer[37] = 109;
	buffer[38] = 103;
	buffer[39] = 77;
	buffer[40] = 24;
	buffer[41] = 35;
	buffer[42] = 55;
	buffer[43] = 64;
	buffer[44] = 81;
	buffer[45] = 104;
	buffer[46] = 113;
	buffer[47] = 92;
	buffer[48] = 49;
	buffer[49] = 64;
	buffer[50] = 78;
	buffer[51] = 87;
	buffer[52] = 103;
	buffer[53] = 121;
	buffer[54] = 120;
	buffer[55] = 101;
	buffer[56] = 72;
	buffer[57] = 92;
	buffer[58] = 95;
	buffer[59] = 98;
	buffer[60] = 112;
	buffer[61] = 100;
	buffer[62] = 103;
	buffer[63] = 99;

	// it hurts
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
	return temp;
}

std::vector<float> threshold(std::vector<float> image, float value = 1e-10)
{
	std::replace_if (image.begin(), image.end(), [&value](float i){return abs(i) < value;}, 0);
	return image;
}

int main()
{
	// Perform 1D DCT
	std::vector<float> signal(LENGTH_1D);
	std::vector<float> DCT_vectors = DCT_vectors_basis();
	store("DCT_vectors.raw", DCT_vectors);
	std::cout << "DC coef: " << DCT_vectors[0] << std::endl;
	
	std::vector<float> lena(LENGTH_1D * LENGTH_1D);
	load("lena_256x256.raw", lena);
	std::vector<float> lena_DCT = transform(lena, DCT_vectors);
	store("lena_DCT.raw", lena_DCT);

	std::vector<float> IDCT_vectors = transpose(DCT_vectors, sqrt(DCT_vectors.size()));
	std::vector<float> lena_new = transform(lena_DCT, IDCT_vectors);
	store("lena_new.raw", lena_new);

	lena_DCT = threshold(lena_DCT, 0);
	std::vector<float> lena_new_t = transform(lena_DCT, IDCT_vectors);
	store("lena_new_t.raw", lena_new_t);

	std::cout << psnr(lena_new_t, lena, 255) << std::endl;

	return 0;
}
