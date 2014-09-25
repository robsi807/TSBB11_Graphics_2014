%% BlendingTest2.m

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