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
f = InterpFunc(w,16,'lin');
wLeft = repmat(f,h,1);
wRight = repmat(1-f,h,1);
figure(4); imagesc(wLeft); colormap gray; title('Linear weithting function');
linMerge = imLeft.*wLeft + imRight.*wRight;
figure(5); imagesc(linMerge/255); colormap gray; title('Linear blended image');

%% Tesing of function LinBlend with two images
im00 = double(imread('../noiseExamples/testNoise1'));
im01 = double(imread('../noiseExamples/testNoise2'));
black = double(rgb2gray(imread('../noiseExamples/black.png')));
overlap = 16;
[imp00 top00 bot00] = LinBlend(im00,im01,black,black,black,overlap,10);
[imp01 top01 bot01] = LinBlend(im01,black,black,im00,black,overlap,20);
size(imp00)
figure(100); imagesc([imp01;imp00]); colormap gray; 
%%
figure(200); imagesc(top00-bot01); colormap gray;
sum(top00(:)-bot01(:))
%% Merge images
[H W] = size(imp00);
Htot = H; Wtot = 2*W;
imtot = zeros(Htot,Wtot);
imtot(1:end,1:W) = imp00;
imtot(1:H,W+1:end) = imp10;

figure(2); imagesc(imtot); colormap gray; title('Interpolate images');

figure(10); imagesc(im00); colormap gray;
figure(11); imagesc(im10); colormap gray;
figure(12); imagesc(imp00); colormap gray;
figure(13); imagesc(imp10); colormap gray;
%% Testing of function LinBlend
im00 = double(imread('../noiseExamples/testNoise1'));
im01 = double(imread('../noiseExamples/testNoise2'));
im10 = double(imread('../noiseExamples/testNoise3'));
im11 = double(imread('../noiseExamples/testNoise4'));
black = double(rgb2gray(imread('../noiseExamples/black.png')));

overlap = 20;
imp00 = LinBlend(im00,im01,im10,black,black,overlap);
imp10 = LinBlend(im10,im11,black,black,im00,overlap);
imp01 = LinBlend(im01,black,im11,im00,black,overlap);
imp11 = LinBlend(im11,black,black,im10,im01,overlap);

% Merge images
[H W] = size(imp00);
Htot = 2*H; Wtot = 2*W; % Image is doubled in size
imtot = zeros(Htot,Wtot);
imtot(1:H,1:W) = imp01;
imtot(1:H,W+1:end) = imp11;
imtot(H+1:end,W+1:end) = imp10;
imtot(H+1:end,1:W) = imp00;

%figure(1); imagesc(imtot); title('Interpolate images');
figure(2); imagesc(imtot); colormap gray; title('Interpolate images');
%% Bad overlap
overlap = 2;
imp00 = LinBlend(im00,im01,im10,black,black,overlap);
imp10 = LinBlend(im10,im11,black,black,im00,overlap);
imp01 = LinBlend(im01,black,im11,im00,black,overlap);
imp11 = LinBlend(im11,black,black,im10,im01,overlap);

% Merge images
[H W] = size(imp00);
Htot = 2*H; Wtot = 2*W; 
imtot = zeros(Htot,Wtot);
imtot(1:H,1:W) = imp01;
imtot(1:H,W+1:end) = imp11;
imtot(H+1:end,W+1:end) = imp10;
imtot(H+1:end,1:W) = imp00;

figure(3); imagesc(imtot); title('Originals');
figure(4); imagesc(imtot); colormap gray; title('Originals');

%% Show originals
figure(101); imagesc(im00); colormap gray; title('Original 00');
figure(102); imagesc(im01); colormap gray; title('Original 01');
figure(103); imagesc(im11); colormap gray; title('Original 11');
figure(104); imagesc(im10); colormap gray; title('Original 10');
%%











