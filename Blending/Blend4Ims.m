
function [im] = Blend4Ims(im00,im01,im10,im11,black,overlap)

[H,W] = size(im00);
[blaj, bl00h] =  BlendHorz(im00,im01,black,overlap);
[blaj, bl00] = BlendVert(bl00h,im10,black,overlap);



end