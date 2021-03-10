# Underwater Image Enhancement

This project was created by Jonathan Nguyen and Bruno Bombassaro for UCLA's ECE 113D Design Capstone Class. The goal of this project is to implement the algorithm in [Ancuti et. al.](doi.org/10.1109/TIP.2017.2759252) on a platform running C. Specifically we tested our implementation on the STMicroelectronics NUCLEO-H745ZI board. As it currently stands, the RAM limitations of the H7 board prevent our program from running on it. As such, going forward, the algorithm implementation will still be written in C but will no longer be specifc to the H7 board. The only additions to make the code compatible with an embedded platform running C is to implement the specific file IO functions (these are the USB_HOST functions on the H7).

# The Algorithm
The algorithm by Ancuti et. al. was created in order to ammend the common impairments with images taken underwater. For example, the deeper light travels underwater, the more that lower frequency light components become attenuated. For underwater images, this means that the red color channel is much more muted than in a picture taken above water. Additionally, particulate matter present in the water cause a haze like effect, blurring edges and obscuring image details. To address this, the algorithm performs the following steps.

1. White balance
2. Gamma correction on (1)
3. Image sharpening on (1)
4. Calculate Laplacian Weight map for (2) and (3)
5. Calculate Saliency Weight map for (2) and (3)
6. Calculate Saturation Weight map for (2) and (3)
7. Combine the Weight maps
8. Utilize the weight map with fusion techniques to reconstruct the image

# MATLAB Implementation
The main driver file is located in `MATLAB_Implementation/dispProcess.m`. This handles calling all of the helper functions as well as displaying all the intermediate results. As it is a script and not a function, the file name must be modified. Additionally, many of the parameters such as the amount of white balance, gamma, and sharpening / blurring strength are hard coded in. If you want to adjust those, feel free to. These are just the values that worked best for us. 

## Converting Images to C Readable Bitmaps and Vice Versa
We have also implemented a function called `im2bitmap.m` that will convert an image to a text file readable to the C executable. In additon to this, we also have a function called `bitmap2im.m` to display the results of the C program. Simply call these functions with the argument being the file you wish to read / write. Note that the resulting text file of `im2bitmap.m` will be the base name of the input image plus "_bitmap.txt". For example:

```
im2bitmap('underwater.png'); %Creates a bitmap file called "underwater_bitmap.txt"
bitmap2im('underwater_bitmap.png');
```

If one wishes to understand the format of the bitmap, it is as follows:

```
(Number of rows in the image)
(Number of columns in the image)
(Floating point representation of each pixel between [0,1])
```
# C Implementation
Assuming one has the standard C libraries available, the C implementation of this image can be built using any standard C compiler (we built the project using both gcc and Visual Studio). The main limitation may be RAM, so be aware of that if the executable is not working properly. The resulting image of the C executable will be in the same bitmap format as specified in the previous section. The name of the result will be the base file plus the suffix "_corrected.txt". For example, calling `./image_fusion underwater_bitmap.txt` will create a new file called `underwater_bitmap_corrected.txt`.

## Parallelization
Parallelization is largely untested but it should work in theory. It relies on the `pthread.h` library meaning that it will not work on most Windows machines. As such, the code for it is commented out. Parallelization was planned to be used in the image fusion and reading. For reading, we planned on having three text files, each for the red, blue, and green channels and reading them all at once. This was becasue reading text files was the main performance bottleneck. For image processing, we planned on calculating the gamma and sharpened weigths in parallel since they do not depend on each other. In the future, I'll come back to this.
