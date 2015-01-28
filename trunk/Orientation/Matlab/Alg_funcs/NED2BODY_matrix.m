function R = NED2BODY_matrix(roll,elevation,heading)
% F.Fraja
% 
% this function computes the rotation matrix R to be used to pass from 
% Earth reference frame NED (North-East-Down) to body reference frame XYZ
% 
% [Ref. Wikipedia]
% 
% inputs: 
% - roll angle        (roll)      [rad]
% - flight path angle (elevation) [rad]
% - heading angle     (heading)   [rad]
%
% outputs: 
% - rotation matrix (R)
%--------------------------------------------------------------------------

% % assign angles
% psi   = heading;
% gamma = elevation;
% phi   = roll;
% 
% % shortcuts for sin and cos
% s_psi   = sin(psi);
% c_psi   = cos(psi);
% s_gamma = sin(gamma);
% c_gamma = cos(gamma);
% s_phi   = sin(phi);
% c_phi   = cos(phi);
% 
% % rotation matrices
% 
% % R1 (heading)
% R1 = [ c_psi s_psi 0;
%       -s_psi c_psi 0;
%         0     0    1];
%     
% % R2 (elevation)
% R2 = [c_gamma 0 -s_gamma;
%        0      1   0   ;
%       s_gamma 0  c_gamma];
%     
% % R3 (roll)
% R3 = [1   0     0   ;
%       0  c_phi s_phi;
%       0 -s_phi c_phi];
%    
% % final matrix
% R = R3*R2*R1;

sy=sin(heading);
cy=cos(heading);
sr=sin(roll);
sp=sin(elevation);
cr=cos(roll);
cp=cos(elevation);

R=[     cp*cy,         cp*sy,        -sp;...
   sr*sp*cy-cr*sy , sr*sp*sy+cr*cy, sr*cp;...
   cr*sp*cy+sr*sy , cr*sp*sy-sr*cy, cr*cp];

end

