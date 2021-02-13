function rgb_image = bitmap2im(filename)
%IM2BITMAP Given an image file (txt) in the following format 
%   [R1 R2 R3 ... G1 G2 G3 ... B1 B2 B3] with delimiter ',' where all pixel values are
%   written from left to right and top to bottom, display the image

% Open the file
fileID = fopen(filename, 'r');
data = fgetl(fileID);
data = strsplit(data, ','); % Data is separataed by commas

% First 2 entries are the dimensions
ydim = str2double(data(1));
xdim = str2double(data(2));

% Create new image matrix
rgb_image = zeros(ydim, xdim,3);

for channel=1:3
    for i=1:ydim
        for j=1:xdim
            rgb_image(i,j,channel) = str2double(data((i-1)*xdim + j + 1 + (channel-1)*xdim*ydim));
        end
    end
end

rgb_image = uint8(rgb_image);
imshow(rgb_image);

