import numpy as np
from PIL import Image

def im2bitmap(filename):

    # Open the image file and convert it into a numpy array
    image = Image.open(filename)
    data = asarray(image)

    ydim = data.shape[0]
    xdim = data.shape[1]
    num_channels= data.shape[2]

    # Write the pixels to a text file
    f = open(filename + "_bitmap.txt", "w")

    for channel in range(num_channels):
        for i in range(xdim):
            for j in range(ydim):
                f.write('%d,' % data[i,j,channel])

def bitmap2im(filename):

    data = np.genfromtxt(filename)

    image = np.zeros((data[0], data[1], 3))

    for channel in range(3):
        for i in range(data[0]):
            for j in range(data[1]):
                image[i,j,channel] = image(j + i*data[1] + channel*data[0]*data[1])

    
            

