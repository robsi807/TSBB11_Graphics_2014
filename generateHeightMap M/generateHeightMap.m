gridSize = 256;

for numberOfPatches = 1:4
    finalGrid = zeros(gridSize,gridSize);
for n = 2:8 % 
    frequency = power(2,n);
    gradientPoints = frequency +1;
    amplitude = 1/frequency;
    
    gradients = rand(gradientPoints,gradientPoints);
    
    tempGrid = createPatch(gridSize,frequency,gradients)*amplitude;
    % Show every frequecy in image
    %figure(n), imagesc(tempGrid), colormap gray; 
    
    finalGrid = finalGrid + tempGrid;
end

% Save every image to file
% imwrite(finalGrid, ['testNoise' int2str(numberOfPatches)], 'PNG'); 

% Show every final 
% figure(numberOfPatches);
% imagesc(finalGrid), colormap gray;

end


