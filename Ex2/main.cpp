#include <iostream>
#include <vector>
#include <cmath>
#include<fstream>

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
		return 1;
	}
}

int main()
{
	std::vector<float> cst(ROWS*COLS);
	for (int i = 0; i < ROWS; ++i)
	{
		for (int j = 0; j < COLS; ++j)
		{
			cst[i*COLS + j] = .5 + .5 * cos(i * M_PI / 32) * cos(j * M_PI / 64);
		}
	}
	store("cst.raw", cst);
	return 0; 
}