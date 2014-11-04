function [finalGrid, finalGrid2] = cornerBlending(gridSize)

%gridSize = 256;

finalGrid = zeros(gridSize,gridSize,4);

for i = 1:gridSize
    for j = 1:gridSize
        
        diffX = (i-1)/(gridSize-1);
        diffY = (j-1)/(gridSize-1);
        
        interpolatedX1 = interpolateValues(1,0,diffX);
        interpolatedX2 = 0;
        
        finalGrid(i,j,1) = interpolateValues(interpolatedX1,interpolatedX2,diffY);
        
        interpolatedX1 = 1-interpolatedX1;
        interpolatedX2 = 0;
        
        finalGrid(i,j,2) = interpolateValues(interpolatedX1,interpolatedX2,diffY);

        interpolatedX1 = 0;
        interpolatedX2 = interpolateValues(1,0,diffX);
        
        finalGrid(i,j,3) = interpolateValues(interpolatedX1,interpolatedX2,diffY);

        interpolatedX1 = 0;
        interpolatedX2 = 1-interpolatedX2;
        
        finalGrid(i,j,4) = interpolateValues(interpolatedX1,interpolatedX2,diffY);
        
    end
end



%imagesc(finalGrid(:,:,1:3));

end
