#include <iostream>
#include <fstream>
#include <cmath>

#define LENGTH_1D	256

void create_coeff(float * coeff_matrix)
{
	for (int k = 0; k < LENGTH_1D; ++k)
	{
		float scale = (k == 0) ? sqrt(.5) : 1.;
		for (int n = 0; n < LENGTH_1D; ++n)
		{	
			coeff_matrix[n + k*LENGTH_1D] = scale * cos(M_PI*k/LENGTH_1D * (n+.5));
		}		
	}
}

void store(float* arrayIn, std::string filename)
{
	std::ofstream outfile;
	outfile.open(filename, std::ios::out | std::ios::binary);

	if (outfile.is_open()) 
	{
		outfile.write(reinterpret_cast<const char*>(arrayIn), LENGTH_1D*LENGTH_1D*sizeof(float));
	}
	outfile.close();
}

void load(std::string filename, float * buffer)
{
	std::ifstream is (filename, std::ifstream::binary);
	
	if(is)
	{
		is.read (reinterpret_cast<char*> (buffer), LENGTH_1D*LENGTH_1D*sizeof(float));
    	is.close();	
	}
	else
	{
		std::cout << "Error opening file.\n";
	}    
}

void transform(float * image, float * transformed, float * basis)
{
	// Row-wise
	float * temp = new float[LENGTH_1D*LENGTH_1D];
	for (int row = 0; row < LENGTH_1D; ++row)
	{
		for (int elem = 0; elem < LENGTH_1D; ++elem)
		{
			float sum = 0;
			for (int k = 0; k < LENGTH_1D; ++k)
			{
				sum += image[k + row*LENGTH_1D] * basis[k + elem*LENGTH_1D];
			}
			temp[elem + row*LENGTH_1D] = sum * sqrt(2)/sqrt(LENGTH_1D);
		}
	}

	//Column-wise
	for (int row = 0; row < LENGTH_1D; ++row)
	{
		for (int elem = 0; elem < LENGTH_1D; ++elem)
		{
			float sum = 0;
			for (int k = 0; k < LENGTH_1D; ++k)
			{
				sum += temp[row + k*LENGTH_1D] * basis[k + elem*LENGTH_1D];
			}
			transformed[elem + row*LENGTH_1D] = sum * sqrt(2)/sqrt(LENGTH_1D);
		}
	}
	delete temp;
}

void threshold(float * bufferIn, float * bufferOut, float th)
{
	for (int i = 0; i < LENGTH_1D; ++i)
	{
		for (int j = 0; j < LENGTH_1D; ++j)
		{
			bufferOut[i + LENGTH_1D*j] = bufferIn[i + LENGTH_1D*j];
			if (abs(bufferOut[i + LENGTH_1D*j]) < th)
			{
				bufferOut[i + LENGTH_1D*j] = 0.;
			}
		}
	}
}

void transpose(float * bufferIn, float * bufferOut)
{
	for (int i = 0; i < LENGTH_1D; ++i)
	{
		for (int j = 0; j < LENGTH_1D; ++j)
		{
			bufferOut[i + LENGTH_1D*j] = bufferIn[j + LENGTH_1D*i];
		}
	}
}

float computePsnr(float* image_noisy, float* image_ref, float max)
{
	float mse = 0;
	for (int i = 0; i < LENGTH_1D; i++)
	{
		for (int j = 0; j < LENGTH_1D; j++)
		{	
			mse += pow((image_noisy[j+i*LENGTH_1D] - image_ref[j+i*LENGTH_1D]),2);
		}
	}
	mse /= LENGTH_1D*LENGTH_1D;

	return 10*log10(max*max/mse);
}

void quantize8bpp(float * bufferIn, float * bufferOut)
{
	// find max and min values
	float min = bufferIn[0];
	float max = bufferIn[0];
	for (int i = 1; i < LENGTH_1D*LENGTH_1D; ++i)
	{
		if( bufferIn[i] > max)
		{
			max = bufferIn[i];
		}
		if (bufferIn[i] < min)
		{
			min = bufferIn[i];
		}
	}

	float ratio = (max - min);
	for (int i = 0; i < LENGTH_1D*LENGTH_1D; ++i)
	{
		bufferOut[i] = (bufferIn[i]-min)/ratio*255.0;
	}
}

int main()
{
	// Compute DCT basis functions
	float * DCT_basis = new float[LENGTH_1D*LENGTH_1D];
	create_coeff(DCT_basis);

	// Perform a DCT
	float * lena = new float[LENGTH_1D*LENGTH_1D];
	float * DCT_lena = new float[LENGTH_1D*LENGTH_1D];
	load("lena_256x256.raw", lena);
	transform(lena, DCT_lena, DCT_basis);
	store(DCT_lena, "DCT_lena.raw");



	return 0;
}