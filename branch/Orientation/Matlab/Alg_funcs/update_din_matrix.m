function R=update_din_matrix(roll,pitch)%[φ,θ]

sr=sin(roll);
sp=sin(pitch);
cr=cos(roll);
cp=cos(pitch);

R=[1;0;0,sr*sp/cp;cr;sr/cp,sr*sp/cp,-sr,cr/cp];


end