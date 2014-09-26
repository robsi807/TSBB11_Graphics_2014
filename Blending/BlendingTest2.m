%% BlendingTest2.m
close all;
%%
imLeft = double(imread('../noiseExamples/testNoise1'));
imRight = double(imread('../noiseExamples/testNoise2'));

overlap = 16;
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
[blMid,blMidCut] = BlendHorz(imMid,imLeft,imRight,overlap);
blLeft = BlendHorz(imLeft,black,imMid,overlap);
blRight = BlendHorz(imRight,imMid,black,overlap);
hej = size(blMidCut)
close all;
figure(10); imagesc(blMid); colormap gray;
figure(11); imagesc(blLeft); colormap gray;
figure(12); imagesc(blRight); colormap gray;

size(blLeft(:,W-overlap/2:2*W-1.5*overlap+1))

blTot = [blLeft(:,W-overlap/2:2*W-1.5*overlap-1),...
    blMid(:,W-overlap/2:2*W-1.5*overlap-1),...
    blRight(:,W-overlap/2:2*W-1.5*overlap-1)];

figure(13); imagesc(blTot);colormap gray; title('Complete blend');

%% Test of BlendVert
imMid = double(imread('../noiseExamples/testNoise1'));
imTop = double(imread('../noiseExamples/testNoise2'));
imBottom = double(imread('../noiseExamples/testNoise3'));
black = double(rgb2gray(imread('../noiseExamples/black.png')));
[H,W] = size(imMid);

overlap = 16;
blMid = BlendVert(imMid,imTop,imBottom,overlap);
blTop = BlendVert(imTop,black,imMid,overlap);
blBottom = BlendVert(imBottom,imMid,black,overlap);

figure(15); imagesc(blMid); colormap gray;
figure(16); imagesc(blTop); colormap gray;
figure(17); imagesc(blBottom); colormap gray;

blTot = [blTop(H-overlap/2:2*H-1.5*overlap-1,:);...
    blMid(H-overlap/2:2*H-1.5*overlap-1,:);...
    blBottom(H-overlap/2:2*H-1.5*overlap-1,:)];

figure(18); imagesc(blTot);colormap gray; title('Complete blend');

%% Test of interpolation in all directions



