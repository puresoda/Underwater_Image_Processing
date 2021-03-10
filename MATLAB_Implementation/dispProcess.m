% Performs image fusion algorithm and displays all intermediate results
REG_VAL = 0.2;  % Regularization vaue 
NUM_INPUTS = 2;

close all;

% Read and show the original image
original_img = im2double(imread("01.png"));
figure; imshow(original_img); title("Original Image");

% Perform the white balancing technique and show it
white_img = whiteBalance(original_img, 1);
figure; imshow(white_img); title("White Balanced Image");

% Apply gamma corretion
gamma_img = gammaCorrection(white_img, 1.4); 
figure; imshow(gamma_img); title("Gamma Corrected Image");

% Get Laplacian weight on the gamma corrected image
g_laplace = calcLaplacianWeight(gamma_img);
figure; imshow(g_laplace, []); title("Gamma Laplacian Weights");

% Get Saliency weight on the gamma corrected image 
g_saliency = calcSaliencyWeight(gamma_img);
figure; imshow(g_saliency, []); title("Gamma Saliency Weights");

% Get Saturation weight on the gamma corrected image 
g_saturation = calcSaturationWeight(gamma_img);
figure; imshow(g_saturation, []); title("Gamma Saturation Weights");

% Aggregated gamma
g_weight = g_laplace + g_saliency + g_saturation;
figure; imshow(g_weight, []); title("Gamma Aggregate Weight");

% Apply image sharpening
% sharp_img = sharpenImage(white_img, 64, 0.5);
% sharp_img = imadjust(white_img,[]); 
sharp_img = imsharpen(white_img, 'Amount', 3); 

figure; imshow(sharp_img, []); title("Sharpened Mask Image");

% Get Laplacian weight on the sharpened image
s_laplace = calcLaplacianWeight(sharp_img);
figure; imshow(s_laplace, []); title("Sharpened Laplacian Weights");

% Get Saliency weight on the sharpened image 
s_saliency = calcSaliencyWeight(sharp_img);
figure; imshow(s_saliency, []); title("Sharpened Saliency Weights");

% Get Saturation weight on the sharpened image 
s_saturation = calcSaturationWeight(sharp_img);
figure; imshow(s_saturation, []); title("Sharpened Saturation Weights");

% Aggregated sharpened image
s_weight = s_laplace + s_saliency + s_saturation;
figure; imshow(s_weight, []); title("Sharpened Aggregate Weight");

% Normalize the aggreagated weight maps
g_weight = g_weight / max(max(g_weight));
s_weight = s_weight / max(max(s_weight));

g_weight = (g_weight + REG_VAL)./(s_weight + g_weight + NUM_INPUTS * REG_VAL);
s_weight = (s_weight + REG_VAL)./(s_weight + g_weight + NUM_INPUTS * REG_VAL);

% Naive image fusion

% Total weight
t_weight = (g_weight + s_weight);
figure; imshow(t_weight, []); title("Total Aggregate Weight");

reconstructed = t_weight.*white_img;
reconstructed = reconstructed.^0.7;

figure;
imshowpair(original_img, reconstructed, 'montage'); title("Original vs Enhanced Image");