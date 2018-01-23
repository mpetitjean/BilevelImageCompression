# JPP: Bilevel Images Codec
Encoder/Decoder tool for  lossless bilevel image compression (only power-of-two square images).

The source code is in the folder 'src'. To compile the program, simply type 'make' (note that the provided Makefile requires g++ and Boost). This code was tested with g++ 5.4 and g++ 7.2. Once the code is compiled, an executable called 'convert' will be available.
To compress a .raw file, run './convert filename'. You can look at the output to check the various Compression Ratios of the implemented methods. The compressed file will be written in .jpp format.
To decompress a .jpp file, run './convert filename'.

The test images that were used to benchmark the different methods in the report are provided in the folder 'Test images'.
