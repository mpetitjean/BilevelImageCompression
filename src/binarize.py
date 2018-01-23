from scipy import misc
import numpy as np
import struct

def weightedAverage(pixel):
    return 0.299*pixel[0] + 0.587*pixel[1] + 0.114*pixel[2]

image = misc.imread('airport.tiff')

binary = np.zeros((image.shape[0], image.shape[1])) 
grey = np.zeros((image.shape[0], image.shape[1])) # init 2D numpy array
# get row number

# FOR RGB
# for rownum in range(len(image)):
# 	for colnum in range(len(image[rownum])):
# 		grey[rownum][colnum] = int(weightedAverage(image[rownum][colnum]))
# 		if (grey[rownum][colnum] > 123):
# 			binary[rownum][colnum] = 1
# 		else:
# 			binary[rownum][colnum] = 0

# FOR GRAY already
for rownum in range(len(image)):
	for colnum in range(len(image[rownum])):
		if (image[rownum][colnum] > 80):
			binary[rownum][colnum] = 1
		else:
			binary[rownum][colnum] = 0

# Convert to 1D array to write properly
binary1D = np.ravel(binary, 'C')
binary1D.astype(np.float32)

# Convert to byte arrays to print
toWrite = struct.pack('<%sf' % binary1D.size, *binary1D);

# Write
file = open('airport_1024x1024.raw', 'wb')
file.write(toWrite)
file.close()
