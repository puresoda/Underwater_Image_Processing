function rgb_image = im2bitmap(filename)
%IM2BITMAP Given an image file, write each RGB value to a text file
%   The output will be a flattend version of the image in the following
%   format [R1 R2 R3 ... G1 G2 G3 ... B1 B2 B3] where all pixel values are
%   written from left to right and top to bottom

% Strip the file extension
base_file = filename(1:end-4);

% Read the file 
rgb_image = imread(filename);
y_dim = size(rgb_image, 1);
x_dim = size(rgb_image, 2);

% Loop through image and write each RGB value
fileID = fopen([base_file, '_bitmap.txt'],'w');
fprintf(fileID, '%d,', y_dim);
fprintf(fileID, '%d,', x_dim);

for k=1:3
    for i=1:y_dim
        for j=1:x_dim
            fprintf(fileID, '%d,', rgb_image(i,j,k));
        end
    end
end

fclose(fileID);
end

