#ifndef __IO_HPP_INCLUDED__
#define __IO_HPP_INCLUDED__

#include <vector>
#include <iostream>
#include <fstream>
#include <iterator>

template <class T>
void store(std::string filename, std::vector<T> image)
{
	std::ofstream file (filename, std::ios::binary);
	if (file)
		file.write(reinterpret_cast<const char*>(image.data()), image.size() * sizeof(T));
	else
		std::cerr << "Cannot write into " << filename << ", errno: " << file.rdstate() << std::endl;
	file.close();
}

template <class T>
std::vector<T> load(std::string filename)
{	
	std::vector<T> output;
	std::ifstream file (filename, std::ios::binary | std::ios::ate);
	if (file)
	{
		size_t size;
		if ((size = file.tellg()) % sizeof(T))
			std::cerr << "File ans type sizes mismatch"<< std::endl;
		else
		{
			output.resize(size/sizeof(T));
			file.seekg( 0, std::ios_base::beg);
			file.read(reinterpret_cast<char*>(output.data()), size);
		}
	}
	else 
		std::cerr << "Cannot read " << filename << ", errno: " << file.rdstate() << std::endl;
	file.close();
	return output;
}

template <class T>
int toCSV(std::string filename, std::vector<T> data)
{
	std::ofstream file (filename);
	if (file)
		for (T i : data)
				file << std::to_string(i)<< std::endl;
	else
		std::cout << "Cannot write into " << filename << std::endl;
	file.close();
	return file.rdstate();
}

#endif
