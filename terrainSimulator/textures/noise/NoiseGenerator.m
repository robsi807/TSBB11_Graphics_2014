clear all; close all; 

%% Generate uniformly distributed noise

N = 512; % Size (NxN)

% Create three channels with noise
xr = rand(N,N);
xg = rand(N,N);
xb = rand(N,N);

xrgb = zeros(N,N,3);
xrgb(:,:,1) = xr;
xrgb(:,:,2) = xg;
xrgb(:,:,3) = xb;

fileName = 'uniformNoise1.tga';
imwrite(xrgb,fileName);