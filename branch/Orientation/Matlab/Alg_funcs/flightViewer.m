function flightViewer(position_NED,attitude)
% display quadcopter movements and rotations

% NOTE 1: it's a computational slow function, so use a rate transition
% before calling it from simulink (e.g. at 10Hz)

% NOTE 2: 'NED' reference is plotted by matlab as 'ENU' reference
% when using plot or plot3

% INPUTS:
% - position_NED = [North, East, Down]
% - attitude = [Roll, Pitch, Yaw]

% -------------------- INPUT SECTION --------------------------------------

% body dimension
armLenght = 4; % may not be indicative of real model
 dir_axe = 0:0.01:1;
  n_arm_points=10;
 axisLimit = [-armLenght armLenght -armLenght armLenght -armLenght armLenght]*4;
% --------------------- EXECUTION SECTION ---------------------------------

persistent flag
persistent h1

% static variables initialization
if isempty(flag)
   flag=0;
end
if flag==0
     h1=figure('Name','Flight Viewer');
    
     flag=1;
end

% initialize figure
    
    figure(h1);
 
    
    

% compute rotation matrix
NED2BODY_mat = NED2BODY_matrix(attitude(1),attitude(2),attitude(3));


% arms definition (defined along North/East/South/West direction and then rotated of 45� to the right) (NED)
defaultRotMatrix = [0.7071 -0.7071 0;0.7071 0.7071 0;0 0 1]; % default heading +45�
arm1 = defaultRotMatrix*[linspace(0,armLenght,n_arm_points);linspace(0,0,n_arm_points);linspace(0,0,n_arm_points)];
arm2 = defaultRotMatrix*[linspace(0,0,n_arm_points);linspace(0,armLenght,n_arm_points);linspace(0,0,n_arm_points)];
arm3 = defaultRotMatrix*[linspace(0,-armLenght,n_arm_points);linspace(0,0,n_arm_points);linspace(0,0,n_arm_points)];
arm4 = defaultRotMatrix*[linspace(0,0,n_arm_points);linspace(0,-armLenght,n_arm_points);linspace(0,0,n_arm_points)];

% heading line definition (is defined along North direction) (NED)
headLineLenght = armLenght*2; 
headLine = [linspace(0,headLineLenght,n_arm_points);linspace(0,0,n_arm_points);linspace(0,0,n_arm_points)];

% top line definition (is defined along -D direction) (NED)
topLineLenght = armLenght*2;
topLine = [linspace(0,0,n_arm_points);linspace(0,0,n_arm_points);linspace(0,-topLineLenght,n_arm_points)];

% body movement (translation + rotation)
arm1 = moveBody(arm1,position_NED,NED2BODY_mat);
arm2 = moveBody(arm2,position_NED,NED2BODY_mat);
arm3 = moveBody(arm3,position_NED,NED2BODY_mat);
arm4 = moveBody(arm4,position_NED,NED2BODY_mat);
headLine = moveBody(headLine,position_NED,NED2BODY_mat);
topLine = moveBody(topLine,position_NED,NED2BODY_mat);


% body plot (in ENU reference frame)
armx=[arm1(2,:),arm2(2,:),arm3(2,:),arm4(2,:)];
army=[arm1(1,:),arm2(1,:),arm3(1,:),arm4(1,:)];
armz=[-arm1(3,:),-arm2(3,:),-arm3(3,:),-arm4(3,:)];

plot3(0,dir_axe,0,'g.',...
    armx,army,armz,...
    headLine(2,:),headLine(1,:),-headLine(3,:),'k-',...
    topLine(2,:),topLine(1,:),-topLine(3,:),'m-',...
   dir_axe,0,0,'g.');
text(0,1,0,'N');
text(1,0,0,'E');
  % orientation
  az = -30; % azimuth
el = 60;  % elevation


% axis orientation
    view(az,el)
    

    % axis limit
    axis(axisLimit)
 grid on;


end

function movedBody = moveBody(body,displacement,NED2BODY_mat)
% apply a rotation and then a displacement to an array of points in 3D

% init
sizeArm = size(body,2); 


ned_body_inv=transp(NED2BODY_mat);
displacement_int = repmat(displacement,1,sizeArm);


movedBody=ned_body_inv*body+displacement_int;

end


