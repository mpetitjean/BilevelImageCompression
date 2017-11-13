#include <iostream>
#include <cmath>

#define LENGTH_1D	256 

void DCT1(float * signal, float * coeff, int length)
{
	for (int k = 0; k < length; ++k) {
    double sum = 0.;
    // if k=0, then scaling factor = 1
    double s = (k == 0) ? sqrt(.5) : 1.;
    for (int n = 0; n < length; ++n)
    {
    	sum += s * x[n] * cos(M_PI * (n + .5) * k / length);
    }
    X[k] = sum * sqrt(2. / length);
  }
}

int main()
{
	// Perform 1D DCT
	float * signal = new float[LENGTH_1D];
	float * DCT_coeff = new float[LENGTH_1D];


	return 0;
}