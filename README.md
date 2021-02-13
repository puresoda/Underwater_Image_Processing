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
To be done later. I'm tired.

# C Implementation