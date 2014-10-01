
im00 = double(imread('../noiseExamples/testNoise1'));
im01 = double(imread('../noiseExamples/testNoise2'));
im10 = double(imread('../noiseExamples/testNoise3'));
im11 = double(imread('../noiseExamples/testNoise4'));

grid = cornerBlending(16);
blendIm = im00.*grid(:,:,1) + im01.*grid(:,:,2) ...
    + im10.*grid(:,:,3) + im11.*grid(:,:,4);

imagesc(grid); colormap gray;

%% 
[H,W] = size(im00);
L = 16;
blim = zeros(2*H-L,2*W-L);
blim(1:H-L/2,1:W-L/2) = im00(1:H-L/2,1:W-L/2);
blim(1:H-L/2,W-L/2+1:2*W-L) = im01(1:H-L/2,L/2+1:W);
blim(H-L/2+1:2*H-L,1:W-L/2) = im10(L/2+1:H,1:W-L/2);
blim(H-L/2+1:2*H-L,W-L/2+1:2*W-L) = im11(L/2+1:H,L/2+1:W);

figure(2); imagesc(blim); colormap gray;

grid = cornerBlending(L);
figure(1); imagesc(grid(:,:,1:3)); 
Hcut = 2*H-L;
Wcut = 2*W-L;
rows = Hcut/2-L/2+1:Hcut/2+L/2;
cols = Wcut/2-L/2+1:Wcut/2+L/2;
blim(rows,cols) = ...
      grid(:,:,1).*im00(H-L+1:H,W-L+1:W)...
    + grid(:,:,2).*im10(1:L,W-L+1:W) ...
    + grid(:,:,3).*im01(H-L+1:H,1:L) ...
    + grid(:,:,4).*im11(1:L,1:L);

figure(3); imagesc(blim); colormap gray;