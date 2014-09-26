function f = InterpFunc(imWidth,wWidth,type)
% Assume even width and windowWidth
f = zeros(1,imWidth);
f(imWidth/2+1:end) = 1;
switch type
    case 'lin'
        window = (0:wWidth-1)/wWidth;
        size(window)
        f((imWidth/2-wWidth/2+1):(imWidth/2+wWidth/2)) = window;
    case 'atan'
        scale = 1/10;
        window = (atan(scale*(-wWidth/2:wWidth/2))+pi/2)/pi
        f((imWidth/2-wWidth/2):(imWidth/2+wWidth/2)) = window;
end

end