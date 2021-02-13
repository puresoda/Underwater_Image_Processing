function corr_img = gammaCorrection(input_img, gamma)
%Applies gamma correction to an image
% Gamma should be greater than 1 in our application to darken the image

if nargin < 2
    gamma = 1.5;
end

% Apply gamma correction
corr_img = input_img.^(gamma);

% Clip the values between 0 and 1
corr_img = min(max(corr_img, 0), 1);

end

