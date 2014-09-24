function [ out ] = interpolateValues(a,b,x)
%INTERPOLATEVALUES Summary of this function goes here
%   Detailed explanation goes here
    
	ft = x * 3.1415927;
	f = (1 - cos(ft)) * 0.5;

	out = a*(1-f) + b*f;
  
end

