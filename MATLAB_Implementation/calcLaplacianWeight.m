function lap_weight = calcLaplacianWeight(input_img, lum_idx)
%CALCLAPLACIANWEIGHT Apply Laplacian Filter on each input color channel

if nargin < 2
    lum_idx = 2;
end

% Example 3x3 Laplacian filter is given by 
% 1  1   1
% 1  -8  1
% 1  1   1
% % filt = ones(3);
% % filt(2,2) = -8;

filt = [[1 0 1]; [0 -4 0]; [1 0 1]];

red = input_img(:,:,1);
blue = input_img(:,:,2);
green = input_img(:,:,3);

% luminance_idx determines what formula to use to calculate the luminance
% 1: Standard Relative luminance
% 2: Percieved luminance approximation
% 3: HSP model
switch(lum_idx)
    case 1
        lum = 0.2126 * red + 0.7152 * green + 0.0722 * blue;
    case 2
        lum = 0.299 * red + 0.587 * green + 0.114 * blue;
    case 3
        lum = sqrt(0.299 * red.^2 + 0.587 * green.^2 + 0.114 * blue.^2);
    otherwise
        lum = zeros(size(input_img));
        disp("Invalid luminance value");
end

% Calculate the Laplacian filtered image for luminance channel
lap_weight = abs(conv2(lum, filt, 'same'));
lap_weight = lap_weight/max(max(lap_weight));

end

