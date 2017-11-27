#include <iostream>
#include <fstream>
#include <cmath>

#define LENGTH_1D	256
#define BLK_SIZE	8
#define BLK_SIZE2	32

void store(float* arrayIn, std::string filename, int size)
{
	std::ofstream outfile;
	outfile.open(filename, std::ios::out | std::ios::binary);

	if (outfile.is_open()) 
	{
		outfile.write(reinterpret_cast<const char*>(arrayIn), size*size*sizeof(float));
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

void create_coeff(float * coeff_matrix)
{
	for (int k = 0; k < BLK_SIZE; ++k)
	{
		float scale = (k == 0) ? sqrt(.5) : 1.;
		for (int n = 0; n < BLK_SIZE; ++n)
		{	
			coeff_matrix[n + k*BLK_SIZE] = scale * cos(M_PI*k/BLK_SIZE * (n+.5));
		}		
	}
}

void create_coeff32(float * coeff_matrix)
{
	for (int k = 0; k < BLK_SIZE2; ++k)
	{
		float scale = (k == 0) ? sqrt(.5) : 1.;
		for (int n = 0; n < BLK_SIZE2; ++n)
		{	
			coeff_matrix[n + k*BLK_SIZE2] = scale * cos(M_PI*k/BLK_SIZE2 * (n+.5));
		}		
	}
}

void transform(float * image, float * transformed, float * basis)
{
	// Row-wise
	float * temp = new float[BLK_SIZE*BLK_SIZE];
	for (int row = 0; row < BLK_SIZE; ++row)
	{
		for (int elem = 0; elem < BLK_SIZE; ++elem)
		{
			float sum = 0;
			for (int k = 0; k < BLK_SIZE; ++k)
			{
				sum += image[k + row*BLK_SIZE] * basis[k + elem*BLK_SIZE];
			}
			temp[elem + row*BLK_SIZE] = sum * sqrt(2)/sqrt(BLK_SIZE);
		}
	}

	//Column-wise
	for (int row = 0; row < BLK_SIZE; ++row)
	{
		for (int elem = 0; elem < BLK_SIZE; ++elem)
		{
			float sum = 0;
			for (int k = 0; k < BLK_SIZE; ++k)
			{
				sum += temp[row + k*BLK_SIZE] * basis[k + elem*BLK_SIZE];
			}
			transformed[elem + row*BLK_SIZE] = sum * sqrt(2)/sqrt(BLK_SIZE);
		}
	}
	delete temp;
}

void transform32(float * image, float * transformed, float * basis)
{
	// Row-wise
	float * temp = new float[BLK_SIZE2*BLK_SIZE2];
	for (int row = 0; row < BLK_SIZE2; ++row)
	{
		for (int elem = 0; elem < BLK_SIZE2; ++elem)
		{
			float sum = 0;
			for (int k = 0; k < BLK_SIZE2; ++k)
			{
				sum += image[k + row*BLK_SIZE2] * basis[k + elem*BLK_SIZE2];
			}
			temp[elem + row*BLK_SIZE2] = sum * sqrt(2)/sqrt(BLK_SIZE2);
		}
	}

	//Column-wise
	for (int row = 0; row < BLK_SIZE2; ++row)
	{
		for (int elem = 0; elem < BLK_SIZE2; ++elem)
		{
			float sum = 0;
			for (int k = 0; k < BLK_SIZE2; ++k)
			{
				sum += temp[row + k*BLK_SIZE2] * basis[k + elem*BLK_SIZE2];
			}
			transformed[elem + row*BLK_SIZE2] = sum * sqrt(2)/sqrt(BLK_SIZE2);
		}
	}
	delete temp;
}

void DCT1(float * bufferIn, float * bufferOut, float * DCT_basis)
{
	// 8 by 8 blocks of DCT, contiguous
	float * tempIn = new float[BLK_SIZE*BLK_SIZE];
	float * tempOut = new float[BLK_SIZE*BLK_SIZE];
	int col;

	for (int step = 0; step < pow(LENGTH_1D/BLK_SIZE,2); ++step)
	{	
		// fill temp with correct block
		for (int i = 0; i < BLK_SIZE; ++i)
		{
			for (int j = 0; j < BLK_SIZE; ++j)
			{	
				col = (int) (step/32);
				tempIn[j + i*BLK_SIZE] = bufferIn[j + (step%32)*BLK_SIZE + LENGTH_1D*col*BLK_SIZE + i*LENGTH_1D];
			}
		}

		transform(tempIn, tempOut, DCT_basis);

		// fill result image at the correct position
		for (int i = 0; i < BLK_SIZE; ++i)
		{
			for (int j = 0; j < BLK_SIZE; ++j)
			{
				col = (int) (step/32);
				bufferOut[j + (step%32)*BLK_SIZE + LENGTH_1D*col*BLK_SIZE + i*LENGTH_1D] = tempOut[j + i*BLK_SIZE];
			}
		}
	}

	delete tempIn;
	delete tempOut;
}

void DCT2(float * bufferIn, float * bufferOut, float * DCT_basis)
{
	// 32 × 32 interleaved DCT coefficients
	float * tempIn = new float[BLK_SIZE*BLK_SIZE];
	float * tempOut = new float[BLK_SIZE*BLK_SIZE];
	float * tempDCT = new float[LENGTH_1D*LENGTH_1D];
	int col;

	for (int step = 0; step < pow(LENGTH_1D/BLK_SIZE,2); ++step)
	{	
		// fill temp with correct block
		for (int i = 0; i < BLK_SIZE; ++i)
		{
			for (int j = 0; j < BLK_SIZE; ++j)
			{	
				col = (int) (step/32);
				tempIn[j + i*BLK_SIZE] = bufferIn[j + (step%32)*BLK_SIZE + LENGTH_1D*col*BLK_SIZE + i*LENGTH_1D];
			}
		}

		transform(tempIn, tempOut, DCT_basis);

		// fill result image at classic position
		for (int i = 0; i < BLK_SIZE; ++i)
		{
			for (int j = 0; j < BLK_SIZE; ++j)
			{
				col = (int) (step/32);
				tempDCT[j + (step%32)*BLK_SIZE + LENGTH_1D*col*BLK_SIZE + i*LENGTH_1D] = tempOut[j + i*BLK_SIZE];
			}
		}
	}

	// Reorganize to interleaved position
	for (int step = 0; step < BLK_SIZE*BLK_SIZE; ++step)
	{
		for (int row = 0; row < BLK_SIZE2; ++row)
		{
			for (int col = 0; col < BLK_SIZE2; ++col)
			{
				bufferOut[row * LENGTH_1D + col + (step%BLK_SIZE)*BLK_SIZE2 + step/BLK_SIZE*LENGTH_1D*BLK_SIZE2] = 
				tempDCT[(step%BLK_SIZE) + (step / BLK_SIZE) * LENGTH_1D + col * BLK_SIZE + row * LENGTH_1D * BLK_SIZE];
				// if (step == 32 * 32 - 1)
				// 	std::cout << col * 32 + row * 256 * 32 + step%32 + (step/32)*256<< std::endl;
			}
		}
	}


	delete tempIn;
	delete tempOut;
	delete tempDCT;
}

int main()
{
	// Generate DCT 8x8 basis
	float * DCT_basis = new float[BLK_SIZE*BLK_SIZE];
	create_coeff(DCT_basis);

	// Read image
	float * lena = new float[LENGTH_1D*LENGTH_1D];
	load("lena_256x256.raw", lena);

	// Image with 8x8 DCT
	float * DCT88 = new float[LENGTH_1D*LENGTH_1D];
	DCT1(lena, DCT88, DCT_basis);
	store(DCT88, "DCT88.raw", LENGTH_1D);

	// Generate DCT 32x32 basis
	float * DCT_basis32 = new float[BLK_SIZE2*BLK_SIZE2];
	create_coeff32(DCT_basis32);

	float * DCT32 = new float[LENGTH_1D*LENGTH_1D];
	DCT2(lena, DCT32, DCT_basis);
	store(DCT32, "DCT32.raw", LENGTH_1D);

	// int blk = 35;
	// printf("%d\n",(int) (blk/32)*LENGTH_1D + blk%BLK_SIZE2);

	return 0;
}