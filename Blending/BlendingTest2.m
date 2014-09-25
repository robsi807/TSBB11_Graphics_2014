%% BlendingTest2.m
close all;
%%
imLeft = double(imread('../noiseExamples/testNoise1'));
imRight = double(imread('../noiseExamples/testNoise2'));

overlap = 16;
[multiBlend,linBlend] = BlendIms(imLeft,imRight,overlap);
figure(1); imagesc(linBlend); colormap gray;
figure(2); imagesc(multiBlend); colormap gray;
%% Using apple and melon
imLeft = double(rgb2gray(imread('apple.png')));
imRight = double(rgb2gray(imread('Watermelon.png')));

size(imLeft)
size(imRight)

overlap = 256;
[multiBlend,linBlend] = BlendIms(imLeft,imRight,overlap);
figure(1); imagesc(linBlend); colormap gray;
figure(2); imagesc(multiBlend); colormap gray;

%% Test of BlendHorz
imMid = double(imread('../noiseExamples/testNoise1'));
imLeft = double(imread('../noiseExamples/testNoise2'));
imRight = double(imread('../noiseExamples/testNoise3'));
black = double(rgb2gray(imread('../noiseExamples/black.png')));
[H,W] = size(imMid);
overlap = 16;
blMid = BlendHorz(imMid,imLeft,imRight,overlap);
blLeft = BlendHorz(imLeft,black,imMid,overlap);
blRight = BlendHorz(imRight,imMid,black,overlap);
close all; 
figure(10); imagesc(blMid); colormap gray;
figure(11); imagesc(blLeft); colormap gray;
figure(12); imagesc(blRight); colormap gray;

size(blLeft(:,W-overlap/2:2*W-1.5*overlap+1))

blTot = [blLeft(:,W-overlap/2:2*W-1.5*overlap-1),...
    blMid(:,W-overlap/2:2*W-1.5*overlap-1),...
    blRight(:,W-overlap/2:2*W-1.5*overlap-1)];

figure(13); imagesc(blTot);colormap gray; title('Complete blend');