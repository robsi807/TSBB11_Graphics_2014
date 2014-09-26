
function mergedIm = mergeImVert(north,mid,south,overlap,H,W)

mergedIm = [north(H-overlap/2:2*H-1.5*overlap-1,:);...
    mid(H-overlap/2:2*H-1.5*overlap-1,:);...
    south(H-overlap/2:2*H-1.5*overlap-1,:)];

end