#include <cmath>
#include <iostream>

int main()
{
	float elem00 = 1/(2*M_PI);
	float elem10 = 1/(2*M_PI)*sqrt(exp(-1));
	float elem11 = 1/(2*M_PI)*exp(-1);

	printf("Kernel:\n\n");
	printf("%.4f %.4f %.4f\n", elem11, elem10, elem11);
	printf("%.4f %.4f %.4f\n", elem10, elem00, elem10);
	printf("%.4f %.4f %.4f\n\n", elem11, elem10, elem11);

	float sum = elem00 + 4*elem10 + 4*elem11;
	printf("Sum of the elements is %.4f\n\n", sum);

	elem00 /= sum;
	elem10 /= sum;
	elem11 /= sum;

	printf("Kernel:\n\n");
	printf("%.4f %.4f %.4f\n", elem11, elem10, elem11);
	printf("%.4f %.4f %.4f\n", elem10, elem00, elem10);
	printf("%.4f %.4f %.4f\n\n", elem11, elem10, elem11);

	sum = elem00 + 4*elem10 + 4*elem11;
	printf("Sum of the elements is %.4f\n", sum);
	return 0;
}