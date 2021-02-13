function bal_img = whiteBalance(input_img, factor)
%Given an image matrix, apply a white balance operation to it. The output
%is normalized to be between [0,1]

if nargin < 2
  factor = 1;
end

% Copy the original image and normalize
norm_img = input_img;
avg_red = mean(mean(norm_img(:,:,1)));
avg_green = mean(mean(norm_img(:,:,2)));
avg_blue = mean(mean(norm_img(:,:,3)));

bal_img = zeros(size(input_img));

% Red Channel Shift
bal_img(:,:,1) = norm_img(:,:,1) + ... 
    factor*(avg_green - avg_red)*(1-norm_img(:,:,1)).*norm_img(:,:,2);

% Green Channel Shift (None)
bal_img(:,:,2) = norm_img(:,:,2);

% Blue Channel Shift
bal_img(:,:,3) = norm_img(:,:,3) + ... 
    factor*(avg_green - avg_blue)*(1-norm_img(:,:,3)).*norm_img(:,:,2);

bal_linear = rgb2lin(bal_img);

percentiles = [20,50];
illuminant = illumgray(bal_linear,percentiles);
bal_img = chromadapt(bal_linear,illuminant,'ColorSpace','linear-rgb');