
function [linBlend] = BlendHorz(imMid,imLeft,imRight,overlap)
[H,W] = size(imMid);
Hnew = H;
Wnew = 3*W-2*overlap;

% Create linear weighting windows
% One for each image
slope = [0:overlap]/overlap;
midWeight = zeros(1,Wnew);
midWeight(W-overlap:W) = slope;
midWeight(W+1:2*W-overlap) = 1;
midWeight(2*W-2*overlap:2*W-overlap) = 1-slope; 
midWindow = repmat(midWeight,H,1);
figure(20); imagesc(midWindow); colormap gray;

rightWeight = zeros(1,Wnew);
rightWeight(2*W-2*overlap:2*W-overlap) = slope;
rightWeight(2*W-overlap+1:end) = 1;
rightWindow = repmat(rightWeight,H,1);
figure(21); imagesc(rightWindow); colormap gray;

leftWeight = zeros(1,Wnew);
leftWeight(W-overlap:W) = 1-slope;
leftWeight(1:W-overlap-1) = 1;
leftWindow = repmat(leftWeight,H,1);

% Extend all images to same size
midExt = [zeros(H,W-overlap),imMid,zeros(H,W-overlap)];
figure(29); imagesc(midExt); colormap gray;
leftExt = [imLeft,zeros(H,2*W-2*overlap)];
rightExt = [zeros(H,2*W-2*overlap),imRight];

% Blend images together
midWeighted = midExt.*midWindow;
leftWeighted = leftExt.*leftWindow;
rightWeighted = rightExt.*rightWindow;

figure(30); imagesc(leftWeighted); colormap gray;
figure(31); imagesc(midWeighted); colormap gray;
figure(32); imagesc(rightWeighted); colormap gray;

linBlend = midWeighted + leftWeighted + rightWeighted;

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