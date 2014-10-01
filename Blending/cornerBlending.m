function finalGrid = cornerBlending(gridSize)

%gridSize = 256;

finalGrid = zeros(gridSize,gridSize,4);


for i = 1:gridSize
    for j = 1:gridSize
        
        diffX = i/gridSize;
        diffY = j/gridSize;
        
        interpolatedX1 = interpolateValues(1,0,diffX);
        interpolatedX2 = interpolateValues(0,0,diffX);
        
        finalGrid(i,j,1) = interpolateValues(interpolatedX1,interpolatedX2,diffY);
        
        interpolatedX1 = interpolateValues(0,1,diffX);
        interpolatedX2 = interpolateValues(0,0,diffX);
        
        finalGrid(i,j,2) = interpolateValues(interpolatedX1,interpolatedX2,diffY);

        interpolatedX1 = interpolateValues(0,0,diffX);
        interpolatedX2 = interpolateValues(1,0,diffX);
        
        finalGrid(i,j,3) = interpolateValues(interpolatedX1,interpolatedX2,diffY);

        interpolatedX1 = interpolateValues(0,0,diffX);
        interpolatedX2 = interpolateValues(0,1,diffX);
        
        finalGrid(i,j,4) = interpolateValues(interpolatedX1,interpolatedX2,diffY);
        
    end
end



%imagesc(finalGrid(:,:,1:3));

end
