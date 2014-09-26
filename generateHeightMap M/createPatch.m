function [ finalGrid] = interpolate( gridSize,frequency,gradients)
%INTERPOLATE Summary of this function goes here
%   Detailed explanation goes here

finalGrid = zeros(gridSize,gridSize);
numberOfPixels = gridSize/frequency;



for x = 1:gridSize
    
    diffX = (mod(x-1,numberOfPixels)+0.5)/numberOfPixels;
    for y = 1:gridSize
        
        gradientX = ceil(x/numberOfPixels);
        gradientY = ceil(y/numberOfPixels);
        
        diffY = (mod(y-1,numberOfPixels)+0.5)/numberOfPixels;
        
        interpolatedX1 = interpolateValues(gradients(gradientX,gradientY),gradients(gradientX+1,gradientY),diffX);
        interpolatedX2 = interpolateValues(gradients(gradientX,gradientY+1),gradients(gradientX+1,gradientY+1),diffX);
        
        finalGrid(x,y) = interpolateValues(interpolatedX1,interpolatedX2,diffY);

    end
end     

end

