function sat_weight = calcSaturationWeight(input_img, lum_idx)
%CALCSATURATIONWEIGHT Calculates the saturation weight of an image

if nargin < 2
    lum_idx = 2;
end

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
        
% Saturation Weight calculation
sat_weight = sqrt(1/3*((red - lum).^2 + (green - lum).^2 + (blue - lum).^2));
sat_weight = sat_weight/max(max(sat_weight));

end

