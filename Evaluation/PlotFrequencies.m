% Frequency calculations
p00 = patch_0_0;
N = length(p00);
figure(1); imagesc(p00); colormap gray; title('Raw height map');

p00ft = log(abs(fftshift(fft2(p00-mean(mean(p00)),N,N)))+1);

figure(2); imagesc(p00ft); title('FFT of height map'); colorbar; colormap gray;

%%
p00 = patch_0_1;
N = length(p00);
figure(3); imagesc(p00); colormap gray; title('Raw height map');

p00ft = log(abs(fftshift(fft2(p00,N,N)))+1);

figure(4); imagesc(p00ft); title('FFT of height map');colormap gray;colorbar;
%%
p00 = patch_0_2;
N = length(p00);
figure(5); imagesc(p00); colormap gray; title('Raw height map');

p00ft = log(abs(fftshift(fft2(p00,N,N)))+1);colormap gray;

figure(6); imagesc(p00ft); title('FFT of height map'); colormap gray;colorbar;
%%
p00 = patch_1_0;
N = length(p00);
figure(7); imagesc(p00); colormap gray; title('Raw height map');

p00ft = log(abs(fftshift(fft2(ifftshift(p00),N,N)))+1);

figure(8); imagesc(p00ft); title('FFT of height map');colormap gray;colorbar;

%% Mask
figure(10);
cx=256;cy=256;ix=512;iy=512;r=80;
[x,y]=meshgrid(-(cx-1):(ix-cx),-(cy-1):(iy-cy));
c_mask=((x.^2+y.^2)<=r^2);
imagesc(c_mask) 

%%
p00 = vp00;%patch_0_0;
N = length(p00);
figure(11); imagesc(p00); colormap gray; title('Raw height map');

p00ft = c_mask.*fftshift(fft2(p00,N,N));

%figure(12); imagesc(abs(p00ft)) ; title('FFT of height map'); colorbar;colormap gray;

p00filt = ifft2(ifftshift(p00ft));
figure(13); imagesc(real(p00filt)); colormap gray; title('Raw height map');


