% function LinBlend
% Blends two images in a specified direction
% origIm:   image which blending is made on. Size [H W]
% blendIm:  images which to blend with. Should be of size
%           [H W 4] in the order: top, right, bottom, left.

%function outIm = LinBlend(origIm,blendIms,overlap)
% top = blendIms(:,:,1);
% right = blendIms(:,:,2);
% bottom = blendIms(:,:,3);
% left = blendIms(:,:,4);

function outIm = LinBlend(origIm,top,right,bottom,left,overlap)
[H W] = size(origIm);
outIm = origIm(overlap+1:(H-overlap),overlap+1:(W-overlap));
[Hnew,Wnew] = size(outIm);

% Blend top
win = (1:overlap)/2/overlap; % Window and inverted window
invWin = 1-win;
outIm(1:overlap,:) = 0;

% Blend right
outIm(:,Wnew-overlap:end) = 0;

% Blend buttom
outIm(Hnew-overlap:end,:) = 0;

% Blend left
outIm(:,1:overlap) = 0;

end
