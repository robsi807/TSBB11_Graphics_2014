
% im00 = double(imread('../noiseExamples/testNoise1'));
% im01 = double(imread('../noiseExamples/testNoise2'));
% im10 = double(imread('../noiseExamples/testNoise3'));
% im11 = double(imread('../noiseExamples/testNoise4'));
%% Load data
load('../generateHeightMap M/blendNoiseData/blendNoiseData1.mat');
perl00 = tempGridArray;
load('../generateHeightMap M/blendNoiseData/blendNoiseData2.mat');
perl01 = tempGridArray;
load('../generateHeightMap M/blendNoiseData/blendNoiseData3.mat');
perl10 = tempGridArray;
load('../generateHeightMap M/blendNoiseData/blendNoiseData4.mat');
perl11 = tempGridArray;

%% Merge noise
[H,W,D] = size(perl00);
im00 = zeros(H,W);
im01 = im00;
im10 = im00;
im11 = im00;
for d = 1:D-1
    im00 = im00+perl00(:,:,d);
    im01 = im01+perl01(:,:,d);
    im10 = im10+perl10(:,:,d);
    im11 = im11+perl11(:,:,d);
end
%% 
[H,W] = size(im00);
L = 32;
blim = zeros(2*H-L,2*W-L);
blim(1:H-L/2,1:W-L/2) = im00(1:H-L/2,1:W-L/2);
blim(1:H-L/2,W-L/2+1:2*W-L) = im01(1:H-L/2,L/2+1:W);
blim(H-L/2+1:2*H-L,1:W-L/2) = im10(L/2+1:H,1:W-L/2);
blim(H-L/2+1:2*H-L,W-L/2+1:2*W-L) = im11(L/2+1:H,L/2+1:W);

%figure(3); imagesc(blim); colormap gray;

%% Interpolate corners
grid = cornerBlending(L);
figure(1); imagesc(grid(:,:,1:3)); 
Hcut = 2*H-L;Wcut = 2*W-L;
rows = Hcut/2-L/2+1:Hcut/2+L/2;
cols = Wcut/2-L/2+1:Wcut/2+L/2;
blim(rows,cols) = ...
      grid(:,:,1).*im00(H-L+1:H,W-L+1:W)...
    + grid(:,:,2).*im10(1:L,W-L+1:W) ...
    + grid(:,:,3).*im01(H-L+1:H,1:L) ...
    + grid(:,:,4).*im11(1:L,1:L);

figure(4); imagesc(blim); colormap gray;

% Interpolate 10->00
intVert = [0:1/(L-1):1]';
grid10_00 = repmat(intVert,1,W-L);
rows = Hcut/2-L/2+1:Hcut/2+L/2;
cols = 1:Wcut/2-L/2;
blim(rows,cols) = grid10_00.*im10(1:L,cols) + (1-grid10_00).*im00(H-L+1:H,cols);

figure(4); imagesc(blim); colormap gray;

% Interpolate 11->01
grid11_01 = grid10_00;
rows = Hcut/2-L/2+1:Hcut/2+L/2;
cols = Wcut/2+L/2+1:Wcut;
blim(rows,cols) = grid11_01.*im11(1:L,L+1:W) + (1-grid11_01).*im01(H-L+1:H,L+1:W);

figure(4); imagesc(blim); colormap gray;

% Interpolate 01->00
intHorz = [0:1/(L-1):1];
grid01_00 = repmat(intHorz,H-L,1);
rows = 1:H-L;
cols = Wcut/2-L/2+1:Wcut/2+L/2;
blim(rows,cols) = grid01_00.*im01(rows,1:L) + (1-grid01_00).*im00(rows,W-L+1:W);

figure(4); imagesc(blim); colormap gray;

% Interpolate 11->10
grid11_10 = grid01_00;
rows = Hcut/2+L/2+1:Hcut;
cols = Wcut/2-L/2+1:Wcut/2+L/2;
blim(rows,cols) = grid11_10.*im11(L+1:W,1:L) + (1-grid11_10).*im10(L+1:W,W-L+1:W);

figure(4); imagesc(blim); colormap gray; title('Linear blend');
