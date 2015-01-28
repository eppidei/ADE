function R=update_rotation_matrix_earth2body(yaw,pitch,roll)%[ψ,θ,φ]

%It should allow to transform a vector expressed in earth coordinates to body
%coordinates

sy=sin(yaw);
cy=cos(yaw);
sr=sin(roll);
sp=sin(pitch);
cr=cos(roll);
cp=cos(pitch);

R=[     cp*cy,         cp*sy,        -sp;...
   sr*sp*cy-cr*sy , sr*sp*sy+cr*cy, sr*cp;...
   cr*sp*cy+sr*sy , cr*sp*sy-sr*cy, cr*cp];
end