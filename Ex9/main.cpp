#include <iostream>
#include <fstream>
#include <cmath>

#define LENGTH_1D	256
#define BLK_SIZE	8

void create_Q(float * buffer)
{
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

void transpose(float * bufferIn, float * bufferOut)
{
	for (int i = 0; i < BLK_SIZE; ++i)
	{
		for (int j = 0; j < BLK_SIZE; ++j)
		{
			bufferOut[i + BLK_SIZE*j] = bufferIn[j + BLK_SIZE*i];
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

void approximateBlock(float * bufferIn, float * bufferOut, float * Q, float * DCT_basis, float * IDCT_basis)
{
	// Takes a 8x8 block as input, does DCT -> Q -> IQ -> IDCT

	// perform DCT
	float * temp = new float[BLK_SIZE*BLK_SIZE];
	transform(bufferIn, temp, DCT_basis);

	// perform Q
	int * temp2 = new int[BLK_SIZE*BLK_SIZE];
	for (int i = 0; i < BLK_SIZE; ++i)
	{
		for (int j = 0; j < BLK_SIZE; ++j)
		{
			temp2[j + i*BLK_SIZE] = (int) (temp[j+i*BLK_SIZE]/Q[j+i*BLK_SIZE]);
		}
	}

	// perform IQ
	for (int i = 0; i < BLK_SIZE; ++i)
	{
		for (int j = 0; j < BLK_SIZE; ++j)
		{
			temp[j + i*BLK_SIZE] = (float) (temp2[j+i*BLK_SIZE]*Q[j+i*BLK_SIZE]);
		}
	}

	// perform IDCT
	transform(temp, bufferOut, IDCT_basis);

	delete temp;
	delete temp2;
}

void approximate(float * bufferIn, float * bufferOut, float * Q, float * DCT_basis, float * IDCT_basis)
{
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

		approximateBlock(tempIn, tempOut, Q, DCT_basis, IDCT_basis);

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
	// Generate and store Q
	float * Q = new float[BLK_SIZE*BLK_SIZE];
	create_Q(Q);
	store(Q, "Q.raw", BLK_SIZE);

	// Generate DCT and IDCT basis
	float * DCT_basis = new float[BLK_SIZE*BLK_SIZE];
	create_coeff(DCT_basis);
	float * IDCT_basis = new float[BLK_SIZE*BLK_SIZE];
	transpose(DCT_basis, IDCT_basis);

	// Read image
	float * lena = new float[LENGTH_1D*LENGTH_1D];
	load("lena_256x256.raw", lena);

	// Apply the approximate function
	float * lenaJPEG = new float[LENGTH_1D*LENGTH_1D];
	approximate(lena, lenaJPEG, Q, DCT_basis, IDCT_basis);
	store(lenaJPEG, "lenaJPEG.raw", LENGTH_1D);

	// Clip to [0;255]
	float * lenaJPEG8bpp = new float[LENGTH_1D*LENGTH_1D];
	quantize8bpp(lenaJPEG, lenaJPEG8bpp);
	store(lenaJPEG8bpp, "lenaJPEG8bpp.raw", LENGTH_1D);

	// Compute PSNR
	float psnr = computePsnr(lena, lenaJPEG8bpp, 255.0);
	std::cout << "PSNR after 50% JPEG approximation is " << psnr << " dB" << std::endl;


	return 0;
}