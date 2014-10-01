% function LinBlend
% Blends two images in a specified direction
% origIm:   image which blending is made on. Size [H W]
% blendIm:  images which to blend with. Should be of size
%           [H W 4] in the order: top, right, bottom, left.

function [outIm blendedTop blendedBottom] = LinBlend(origIm,top,right,bottom,left,overlap,i)
[H W] = size(origIm);
Hblend = 2*H-2*overlap;
Wblend = 2*W-2*overlap;
blend1D = InterpFunc(Hblend,overlap,'lin')';

% Blend top
blend2Dtop = repmat(blend1D,1,W);
figure(i+1); imagesc(blend2Dtop); colormap gray; title('Top blending')

% Append zeros images to match the size of the window
middleExt = [zeros(Hblend-H,W);origIm];
topExt = [top;zeros(Hblend-H,W)];
blendedTop = middleExt.*blend2Dtop + topExt.*(1-blend2Dtop);
scaleTop = blendedTop(Hblend-H+1:end,:);
figure(i+2); imagesc(blendedTop); colormap gray; title('Blended top image');

% Blend bottom
blend2Dbottom = (1-blend2Dtop);
figure(i+5); imagesc(blend2Dbottom); colormap gray; title('Bottom blending')

middleExt = [origIm;zeros(Hblend-H,W)];
bottomExt = [zeros(Hblend-H,W);bottom];
blendedBottom = middleExt.*blend2Dbottom + bottomExt.*(1-blend2Dbottom);
scaleBottom = blendedBottom(1:H,:);
figure(i+6); imagesc(blendedBottom); colormap gray; title('Blended bottom image');

mergeIm = 0.5*(scaleTop+scaleBottom);
figure(i+9); imagesc(mergeIm); colormap gray; title('Merged complete image');

outIm = mergeIm(overlap+1:end-overlap,:);
end
% 
% [H W] = size(origIm);
% Hblend = 2*H-2*overlap;
% Wblend = 2*W-2*overlap;
% blend1D = InterpFunc(Hblend,overlap,'lin')';
% 
% % Blend top
% blend2Dtop = repmat(blend1D,1,W);
% figure(i+1); imagesc(blend2Dtop); colormap gray; title('Top blending')
% 
% % Append zeros images to match the size of the window
% middleExt = [zeros(Hblend-H,W);origIm];
% topExt = [top;zeros(Hblend-H,W)];
% blendedTop = middleExt.*blend2Dtop + topExt.*(1-blend2Dtop);
% scaleTop = blendedTop(Hblend-H+1:end,:);
% figure(i+2); imagesc(blendedTop); colormap gray; title('Blended top image');
% 
% % Blend right
% blend2Dright = (1-blend2Dtop');
% figure(i+3); imagesc(blend2Dright); colormap gray; title('Right blending');
% 
% middleExt = [origIm,zeros(H,Wblend-W)];
% rightExt = [zeros(H,Wblend-W),right];
% blendedRight = middleExt.*blend2Dright + rightExt.*(1-blend2Dright);
% scaleRight = blendedRight(:,1:W);
% figure(i+4); imagesc(blendedRight); colormap gray; title('Blended right image');
% 
% % Blend bottom
% blend2Dbottom = (1-blend2Dtop);
% figure(i+5); imagesc(blend2Dbottom); colormap gray; title('Bottom blending')
% 
% middleExt = [origIm;zeros(Hblend-H,W)];
% bottomExt = [zeros(Hblend-H,W);bottom];
% blendedBottom = middleExt.*blend2Dbottom + bottomExt.*(1-blend2Dbottom);
% scaleBottom = blendedBottom(1:H,:);
% figure(i+6); imagesc(blendedBottom); colormap gray; title('Blended bottom image');
% 
% % Blend left
% blend2Dleft = 1-blend2Dright;
% figure(i+7); imagesc(blend2Dleft); colormap gray; title('Left blending');
% 
% middleExt = [zeros(H,Wblend-W),origIm];
% leftExt = [left,zeros(H,Wblend-W)];
% blendedLeft = middleExt.*blend2Dleft + leftExt.*(1-blend2Dleft);
% scaleLeft = blendedLeft(:,Wblend-W+1:end);
% figure(i+8); imagesc(blendedLeft); colormap gray; title('Blended left image');
% 
% mergeIm = 0.5*(scaleTop+scaleBottom);
% figure(i+9); imagesc(mergeIm); colormap gray; title('Merged complete image');
% 
% % Scale image and output
% outIm = mergeIm(1:end-overlap,:);

% % Blend top
% win = (1:overlap)/overlap; % Window and inverted window
% winArea = repmat(win',1,Wnew);
% %outIm(1:overlap,:) = outIm(1:overlap,:).*winArea  ...
% %    +top(H-overlap+1:end,overlap+1:end-overlap).*(1-winArea);
% 
% % Blend right
% winArea = winArea';
% figure(200); imagesc(winArea); colormap gray;
% outIm(:,Wnew-overlap+1:end) = outIm(:,Wnew-overlap+1:end).*(1-winArea) ...
%     +right(overlap+1:(H-overlap),1:overlap).*winArea;
% 
% % Blend buttom
%  winArea = winArea';
% % outIm(Hnew-overlap+1:end,:) = outIm(Hnew-overlap+1:end,:).*(1-winArea)  ...
% %     +bottom(1:overlap,overlap+1:end-overlap).*winArea;
% 
% % Blend left
% winArea = winArea';
% outIm(:,1:overlap) = outIm(:,1:overlap).*winArea ...
%     +left(overlap+1:(H-overlap),W-overlap+1:end).*(1-winArea);
% 
% end
