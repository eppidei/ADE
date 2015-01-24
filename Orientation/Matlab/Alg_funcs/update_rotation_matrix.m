function R=update_rotation_matrix(yaw,pitch,roll)%[ψ,θ,φ]

sy=sin(yaw);
cy=cos(yaw);
sr=sin(roll);
sp=sin(pitch);
cr=cos(roll);
cp=cos(pitch);

R=[cp*cy,cp*cy,-sp;sr*sp*cy-cr*sy,sr*sp*sy+cr*cy,sr*cp;cr*sp*cy+sr*sy,cr*sp*sy-sr*cy,cr*cp];


end