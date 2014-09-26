
function [linBlend] = BlendHorz(imMid,imTop,imBottom,overlap)
[H,W] = size(imMid);
Hnew = 3*H-2*overlap;
Wnew = W;

% Create linear weighting windows
% One for each image
slope = [0:overlap]/overlap;
midWeight = zeros(Hnew,1);
midWeight(W-overlap:W) = slope;
midWeight(W+1:2*W-overlap) = 1;
midWeight(2*W-2*overlap:2*W-overlap) = 1-slope; 
midWindow = repmat(midWeight,1,W);
figure(20); imagesc(midWindow); colormap gray;

bottomWeight = zeros(Hnew,1);
bottomWeight(2*W-2*overlap:2*W-overlap) = slope;
bottomWeight(2*W-overlap+1:end) = 1;
bottomWindow = repmat(bottomWeight,1,W);
figure(21); imagesc(bottomWindow); colormap gray;

leftWeight = zeros(Hnew,1);
leftWeight(W-overlap:W) = 1-slope;
leftWeight(1:W-overlap-1) = 1;
leftWindow = repmat(leftWeight,1,W);

% Extend all images to same size
midExt = [zeros(H-overlap,W);imMid;zeros(H-overlap,W)];
figure(29); imagesc(midExt); colormap gray;
topExt = [imTop;zeros(2*H-2*overlap,W)];
bottomExt = [zeros(2*H-2*overlap,W);imBottom];

% Blend images together
midWeighted = midExt.*midWindow;
topWeighted = topExt.*leftWindow;
bottomWeighted = bottomExt.*bottomWindow;

figure(30); imagesc(topWeighted); colormap gray;
figure(31); imagesc(midWeighted); colormap gray;
figure(32); imagesc(bottomWeighted); colormap gray;

linBlend = midWeighted + topWeighted + bottomWeighted;

end

function bla = asdf()
lin1D = InterpFunc(Wnew,overlap,'lin');
linWindow = repmat(lin1D,H,1);

% Extend images so they are of same size
extIm = zeros(H,W-overlap);
leftExt = [imMid,extIm];
rightExt = [extIm,imRight];
leftWeighted = leftExt.*(1-linWindow);
rightWeighted = rightExt.*linWindow;

% Blend linearly
linBlend = leftWeighted+rightWeighted;

% Low pass filter images
kern = [1 2 1;2 4 2;1 2 1]/16;
leftLp = leftWeighted; 
rightLp = rightWeighted;
for q = 1:3
    leftLp = conv2(leftLp,kern,'same');
    rightLp = conv2(rightLp,kern,'same');
end
leftLapl = leftWeighted-leftLp;
rightLapl = rightWeighted-rightLp;

figure(10); imagesc(leftLapl); colormap gray;

% Blend each band and add
multiBlend = leftLp + leftLapl + rightLp + rightLapl;
end