
function mergedIm = mergeImHorz(west,mid,east,overlap)

[H,W] = size(mid)
mergedIm = [west(:,W-overlap/2:2*W-1.5*overlap-1),...
    mid(:,W-overlap/2:2*W-1.5*overlap-1),...
    east(:,W-overlap/2:2*W-1.5*overlap-1)];

end