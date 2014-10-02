% MultiBandTest.m 
% A script that tests blending in several frequency bands
%clear all; close all;
%% Load data
load('../generateHeightMap M/blendNoiseData/blendNoiseData1.mat');
perl00 = tempGridArray;
load('../generateHeightMap M/blendNoiseData/blendNoiseData2.mat');
perl01 = tempGridArray;
load('../generateHeightMap M/blendNoiseData/blendNoiseData3.mat');
perl10 = tempGridArray;
load('../generateHeightMap M/blendNoiseData/blendNoiseData4.mat');
perl11 = tempGridArray;

%% Show noise
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
figure(1); colormap gray;
subplot(221); imagesc(im00); title('im00');
subplot(222); imagesc(im01); title('im01');
subplot(223); imagesc(im10); title('im10');
subplot(224); imagesc(im11); title('im11');

%% Merge with no blend
[H,W] = size(im00);
L = 32;
blim = zeros(2*H-L,2*W-L);
blim(1:H-L/2,1:W-L/2) = im00(1:H-L/2,1:W-L/2);
blim(1:H-L/2,W-L/2+1:2*W-L) = im01(1:H-L/2,L/2+1:W);
blim(H-L/2+1:2*H-L,1:W-L/2) = im10(L/2+1:H,1:W-L/2);
blim(H-L/2+1:2*H-L,W-L/2+1:2*W-L) = im11(L/2+1:H,L/2+1:W);
figure(2); colormap gray; imagesc(blim); title('Merged noise');
blim2 = blim;

%% Merge the octaves to three frequency bands
% 7 octaves. 1-2 -> R, 3-4 -> G, 5->7 -> B.
rgb00 = zeros(H,W,3);
rgb01 = rgb00;
rgb10 = rgb00;
rgb11 = rgb00;

rgb00(:,:,1) = perl00(:,:,1) + perl00(:,:,2);
rgb01(:,:,1) = perl01(:,:,1) + perl01(:,:,2);
rgb10(:,:,1) = perl10(:,:,1) + perl10(:,:,2);
rgb11(:,:,1) = perl11(:,:,1) + perl11(:,:,2);

rgb00(:,:,2) = perl00(:,:,3) + perl00(:,:,4);
rgb01(:,:,2) = perl01(:,:,3) + perl01(:,:,4);
rgb10(:,:,2) = perl10(:,:,3) + perl10(:,:,4);
rgb11(:,:,2) = perl11(:,:,3) + perl11(:,:,4);

rgb00(:,:,3) = perl00(:,:,5) + perl00(:,:,6) + perl00(:,:,7);
rgb01(:,:,3) = perl01(:,:,5) + perl01(:,:,6) + perl00(:,:,7);
rgb10(:,:,3) = perl10(:,:,5) + perl10(:,:,6) + perl00(:,:,7);
rgb11(:,:,3) = perl11(:,:,5) + perl11(:,:,6) + perl00(:,:,7);
%%
close all;
%% Blend the separate octaves
[H,W] = size(im00);
wl = 1.0; wm = 1.0; wh = 1.0;
fignum = 10;
blrgb = zeros(2*H-L,2*W-L,3);
blrgb(1:H-L/2,1:W-L/2,:) = rgb00(1:H-L/2,1:W-L/2,:);
blrgb(1:H-L/2,W-L/2+1:2*W-L,:) = rgb01(1:H-L/2,L/2+1:W,:);
blrgb(H-L/2+1:2*H-L,1:W-L/2,:) = rgb10(L/2+1:H,1:W-L/2,:);
blrgb(H-L/2+1:2*H-L,W-L/2+1:2*W-L,:) = rgb11(L/2+1:H,L/2+1:W,:);

% Interpolate corners
grid = cornerBlending(L); 
Hcut = 2*H-L;Wcut = 2*W-L;
rows = Hcut/2-L/2+1:Hcut/2+L/2;
cols = Wcut/2-L/2+1:Wcut/2+L/2;

% Band 1
band = 1;
blrgb(rows,cols,band) = ...
      wl*(grid(:,:,1).*rgb00(H-L+1:H,W-L+1:W,band)...
    + grid(:,:,2).*rgb10(1:L,W-L+1:W,band) ...
    + grid(:,:,3).*rgb01(H-L+1:H,1:L,band) ...
    + grid(:,:,4).*rgb11(1:L,1:L,band));
% Band 2
band = 2;
blrgb(rows,cols,band) = ...
      grid(:,:,1).*rgb00(H-L+1:H,W-L+1:W,band)...
    + grid(:,:,2).*rgb10(1:L,W-L+1:W,band) ...
    + grid(:,:,3).*rgb01(H-L+1:H,1:L,band) ...
    + grid(:,:,4).*rgb11(1:L,1:L,band);
% Band 3
band = 3;
blrgb(rows,cols,band) = ...
      wh*(grid(:,:,1).*rgb00(H-L+1:H,W-L+1:W,band)...
    + grid(:,:,2).*rgb10(1:L,W-L+1:W,band) ...
    + grid(:,:,3).*rgb01(H-L+1:H,1:L,band) ...
    + grid(:,:,4).*rgb11(1:L,1:L,band));

% Interpolate 10->00
intVert = [0:1/(L-1):1]';
grid10_00 = repmat(intVert,1,W-L);
rows = Hcut/2-L/2+1:Hcut/2+L/2;
cols = 1:Wcut/2-L/2;

band = 1;
blrgb(rows,cols,band) = wl*(grid10_00.*rgb10(1:L,cols,band) +...
    + (1-grid10_00).*rgb00(H-L+1:H,cols,band));
band = 2;
blrgb(rows,cols,band) = grid10_00.*rgb10(1:L,cols,band) +...
    + (1-grid10_00).*rgb00(H-L+1:H,cols,band);
band = 3;
blrgb(rows,cols,band) = wh*(grid10_00.*rgb10(1:L,cols,band) +...
    + (1-grid10_00).*rgb00(H-L+1:H,cols,band));

% Interpolate 11->01
grid11_01 = grid10_00;
rows = Hcut/2-L/2+1:Hcut/2+L/2;
cols = Wcut/2+L/2+1:Wcut;

band = 1;
blrgb(rows,cols,band) = wl*(grid11_01.*rgb11(1:L,L+1:W,band) ...
    + (1-grid11_01).*rgb01(H-L+1:H,L+1:W,band));
band = 2;
blrgb(rows,cols,band) = grid11_01.*rgb11(1:L,L+1:W,band) ...
    + (1-grid11_01).*rgb01(H-L+1:H,L+1:W,band);
band = 3;
blrgb(rows,cols,band) = wh*(grid11_01.*rgb11(1:L,L+1:W,band) ...
    + (1-grid11_01).*rgb01(H-L+1:H,L+1:W,band));

% Interpolate 01->00
intHorz = [0:1/(L-1):1];
grid01_00 = repmat(intHorz,H-L,1);
rows = 1:H-L;
cols = Wcut/2-L/2+1:Wcut/2+L/2;

band = 1;
blrgb(rows,cols,band) = wl*(grid01_00.*rgb01(rows,1:L,band) ...
    + (1-grid01_00).*rgb00(rows,W-L+1:W,band));
band = 2;
blrgb(rows,cols,band) = grid01_00.*rgb01(rows,1:L,band) ...
    + (1-grid01_00).*rgb00(rows,W-L+1:W,band);
band = 3;
blrgb(rows,cols,band) = wh*(grid01_00.*rgb01(rows,1:L,band) ...
    + (1-grid01_00).*rgb00(rows,W-L+1:W,band));


% Interpolate 11->10
grid11_10 = grid01_00;
rows = Hcut/2+L/2+1:Hcut;
cols = Wcut/2-L/2+1:Wcut/2+L/2;

band = 1;
blrgb(rows,cols,band) = wl*(grid11_10.*rgb11(L+1:W,1:L,band) + ...
    (1-grid11_10).*rgb10(L+1:W,W-L+1:W,band));
band = 2;
blrgb(rows,cols,band) = grid11_10.*rgb11(L+1:W,1:L,band) + ...
    (1-grid11_10).*rgb10(L+1:W,W-L+1:W,band);
band = 3;
blrgb(rows,cols,band) = wh*(grid11_10.*rgb11(L+1:W,1:L,band) + ...
    (1-grid11_10).*rgb10(L+1:W,W-L+1:W,band));

figure(fignum); imagesc(blrgb(:,:,1)+blrgb(:,:,2)+blrgb(:,:,3)); colormap gray;
title('Multi band blend');

%%
diffIm = abs(blim-(blrgb(:,:,1)+blrgb(:,:,2)+blrgb(:,:,3)));
diffIm2 = abs(blim-blim2);
figure(20); imagesc(diffIm); colormap gray; colorbar; title('Diff');
figure(21); imagesc(diffIm2); colormap gray; colorbar; title('Diff2');


