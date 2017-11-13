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

int main()
{
	// Compute and show DCT basis functions
	float * DCT_basis = new float[LENGTH_1D*LENGTH_1D];
	create_coeff(DCT_basis);
	store(DCT_basis, "coeff.raw");
	std::cout << "The DC coefficient is " << DCT_basis[0] << std::endl;

	// First base vector - check that all coeff are the same
	float previous_coeff = DCT_basis[0];
	for (int i = 1; i < LENGTH_1D; ++i)
	{
		if (previous_coeff != DCT_basis[i])
		{
			std::cout << "The elements of the first basis funtions are not all equal !";
		}
	}

	// Perform a DCT
	float * lena = new float[LENGTH_1D*LENGTH_1D];
	float * DCT_lena = new float[LENGTH_1D*LENGTH_1D];
	load("lena_256x256.raw", lena);
	transform(lena, DCT_lena, DCT_basis);
	store(DCT_lena, "DCT_lena.raw");

	// Use a threshold on the DCT coeff, various coeff values
	float * threshold1 = new float[LENGTH_1D*LENGTH_1D];
	float * threshold2 = new float[LENGTH_1D*LENGTH_1D];
	float * threshold3 = new float[LENGTH_1D*LENGTH_1D];

	float t1 = 1.0;
	float t2 = 50.0;
	float t3 = 200.0;
	threshold(DCT_lena, threshold1, t1);
	threshold(DCT_lena, threshold2, t2);
	threshold(DCT_lena, threshold3, t3);

	// Perform IDCT
	float * IDCT_basis = new float[LENGTH_1D*LENGTH_1D];
	float * IDCT_lena =  new float[LENGTH_1D*LENGTH_1D];
	transpose(DCT_basis, IDCT_basis);
	transform(DCT_lena, IDCT_lena, IDCT_basis);
	store(IDCT_lena, "IDCT_lena.raw");

	float * IDCT_lena_t1 = new float[LENGTH_1D*LENGTH_1D];
	transform(threshold1, IDCT_lena_t1, IDCT_basis);
	store(IDCT_lena_t1, "IDCT_lena_t1.raw");
	delete threshold1;
	float psnr = computePsnr(IDCT_lena_t1, lena, 255.0);
	std::cout << "The PSNR when the threshold is " << t1 << " is " << psnr << " dB." << std::endl;

	float * IDCT_lena_t2 = new float[LENGTH_1D*LENGTH_1D];
	transform(threshold2, IDCT_lena_t2, IDCT_basis);
	store(IDCT_lena_t2, "IDCT_lena_t2.raw");
	delete threshold2;
	psnr = computePsnr(IDCT_lena_t2, lena, 255.0);
	std::cout << "The PSNR when the threshold is " << t2 << " is " << psnr << " dB." << std::endl;

	float * IDCT_lena_t3 = new float[LENGTH_1D*LENGTH_1D];
	transform(threshold3, IDCT_lena_t3, IDCT_basis);
	store(IDCT_lena_t3, "IDCT_lena_t3.raw");
	delete threshold3;
	psnr = computePsnr(IDCT_lena_t3, lena, 255.0);
	std::cout << "The PSNR when the threshold is " << t3 << " is " << psnr << " dB." << std::endl;

	// Free memory
	delete lena;
	delete DCT_basis;
	delete DCT_lena;
	delete IDCT_basis;
	delete IDCT_lena;
	delete IDCT_lena_t1;
	delete IDCT_lena_t2;
	delete IDCT_lena_t3;

	return 0;
}