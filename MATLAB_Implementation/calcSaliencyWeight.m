function sal_weight = calcSaliencyWeight(input_img)
%CALCSALIENCYWEIGHT Calculates the saliency weight of the image

% Default standard deviation of the gaussian filter is 0.5
blur_img = imgaussfilt(input_img);

% Convert the blurred image into LAB color space
cform = makecform('srgb2lab');
lab = applycform(blur_img, cform);

% Extract each channel
l = lab(:,:,1);
a = lab(:,:,2);
b = lab(:,:,3);

% Calculate the mean of each channel
lm = mean(mean(l));
am = mean(mean(a));
bm = mean(mean(b));

% Calculate the actual saliency weight
sal_weight = (l-lm).^2 + (a-am).^2 + (b-bm).^2;
sal_weight = sal_weight/max(max(sal_weight));


