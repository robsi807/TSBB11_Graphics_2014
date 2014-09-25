
function [multiBlend linBlend] = BlendIms(imLeft,imRight,overlap)
[H,W] = size(imLeft);
Hnew = H;
Wnew = 2*W-overlap;

% Create linear weighting window
lin1D = InterpFunc(Wnew,overlap,'lin');
linWindow = repmat(lin1D,H,1);

% Extend images so they are of same size
extIm = zeros(H,W-overlap);
leftExt = [imLeft,extIm];
rightExt = [extIm,imRight];
leftWeighted = leftExt.*(1-linWindow);
rightWeighted = rightExt.*linWindow;

% Blend linearly
linBlend = leftWeighted+rightWeighted;

% Low pass filter images
kern = [1 2 1;2 4 2;1 2 1]/16;
leftLp = leftWeighted; rightLp = rightWeighted;
for q = 1:3
    leftLp = conv2(leftLp,kern,'same');
    rightLp = conv2(rightLp,kern,'same');
end
leftLapl = leftWeighted-leftLp;
rightLapl = rightWeighted-rightLp;

% Blend each band and add
multiBlend = leftLp + leftLapl + rightLp + rightLapl;


end