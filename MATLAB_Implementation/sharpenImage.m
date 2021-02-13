function sharp_img = sharpenImage(input_img, num_bins, std_dev)
%SHARPENIMAGE Performs sharpening of an image using histogram normalization
%and Gaussian filtering

if nargin < 2
  num_bins = 64;
  std_dev = 0.5;
elseif nargin < 3
  std_dev = 0.5;
end

% Perform Gaussian filtering (std dev default is 0.5 and filter size is 3 x 3)
filt_img = imgaussfilt(input_img, std_dev);

% Histogram normalization to increase contrast
sharp_img = histeq(input_img - filt_img, num_bins);

% Additional processing
sharp_img = (sharp_img + input_img)/2;

for i=1:3
    sharp_img(:,:,i) = sharp_img(:,:,i) / (max(max(sharp_img(:,:,i))));
end
end