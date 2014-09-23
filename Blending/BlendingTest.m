% BlendingTest.m
% Linear blending
close all;
%% Read noise images
imLeft = double(imread('../noiseExamples/testNoise1'));
imRight = double(imread('../noiseExamples/testNoise2'));
figure(1); imagesc(imLeft/255); colormap gray; 
figure(2); imagesc(imRight/255); colormap gray;

% Merge without interpolation
[h w] = size(imLeft);
f = InterpFunc(w,2,'lin');
wLeft = repmat(f,h,1);
wRight = repmat(1-f,h,1);
simpleMerge = imLeft.*wLeft + imRight.*wRight;
figure(3); imagesc(simpleMerge/255); colormap gray;

%% Merge with linear interpolation
f = InterpFunc(w,32,'lin');
wLeft = repmat(f,h,1);
wRight = repmat(1-f,h,1);
figure(4); imagesc(wLeft); colormap gray; title('Linear weithting function');
linMerge = imLeft.*wLeft + imRight.*wRight;
figure(5); imagesc(linMerge/255); colormap gray; title('Linear blended image');

%% Testing of function LinBlend
origIm = double(imread('../noiseExamples/testNoise1'));
top = double(imread('../noiseExamples/testNoise2'));
right = double(imread('../noiseExamples/testNoise3'));
bottom = double(imread('../noiseExamples/testNoise4'));
left = double(imread('../noiseExamples/testNoise1'));

outIm = LinBlend(origIm,top,right,bottom,left,16);
figure(1); imagesc(origIm); colormap gray; title('Original');
figure(2); imagesc(outIm); colormap gray; title('Interpolated');