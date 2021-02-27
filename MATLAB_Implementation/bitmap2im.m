function rgb_image = bitmap2im(filename)
%IM2BITMAP Given an image file (txt) in the following format 
%   [R1 R2 R3 ... G1 G2 G3 ... B1 B2 B3] with delimiter '\n' where all pixel values are
%   written from left to right and top to bottom, display the image

% Open the file
fileID = fopen(filename, 'r');

% First 2 entries are the dimensions
data = fscanf(fileID, '%f');
num_row = data(1);
num_col = data(2);

% Create new image matrix
rgb_image = zeros(num_row, num_col,1);

for channel=1:3
    for i=1:num_row
        for j=1:num_col
            rgb_image(i,j,channel) = data((i-1)*num_col + j + 1 + num_col*num_row*(channel-1));
        end
    end
end

rgb_image = rgb_image;
imshow(rgb_image);

