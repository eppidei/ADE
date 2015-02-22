function MobileViewer(position_NEU,yaw,pitch,roll,azimuth,elevation)
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
armLenghtLong = 3; % may not be indicative of real model
armLenghtShort = 1;
dir_axe_len=1;
 dir_axe =linspace(0,dir_axe_len,6);% 0:0.05:dir_axe_len;
  n_arm_points=10;
 axisLimit = [-armLenghtLong armLenghtLong -armLenghtLong armLenghtLong -armLenghtLong armLenghtLong]*2;
% --------------------- EXECUTION SECTION ---------------------------------

persistent flag
persistent h1

% static variables initialization
if isempty(flag)
   flag=0;
end
if flag==0
     h1=figure('Name','Mobile Viewer');
    
     flag=1;
end

% initialize figure
    
    figure(h1);
 
    
    

% compute rotation matrix
NEU2BODY_mat = angle2dcm(yaw,pitch,roll,'ZYX');%NEU2BODY_matrix(attitude(1),attitude(2),attitude(3));



% defaultRotMatrix = [1 0 0;0 1 0;0 0 1]; % default heading +45�
arm1 = [linspace(-armLenghtShort,armLenghtShort,n_arm_points);linspace(armLenghtLong,armLenghtLong,n_arm_points);linspace(0,0,n_arm_points)];%[armLenght,[linspace(0,armLenght,n_arm_points);linspace(armLenght,armLenght,n_arm_points);linspace(armLenght,armLenght,n_arm_points)];
arm2 = [linspace(-armLenghtShort,armLenghtShort,n_arm_points);linspace(-armLenghtLong,-armLenghtLong,n_arm_points);linspace(0,0,n_arm_points)];%[linspace(0,0,n_arm_points);linspace(0,armLenght,n_arm_points);linspace(0,0,n_arm_points)];
arm3 = [linspace(-armLenghtShort,-armLenghtShort,n_arm_points);linspace(-armLenghtLong,armLenghtLong,n_arm_points);linspace(0,0,n_arm_points)];%[linspace(0,-armLenght,n_arm_points);linspace(0,0,n_arm_points);linspace(0,0,n_arm_points)];
arm4 = [linspace(armLenghtShort,armLenghtShort,n_arm_points);linspace(-armLenghtLong,armLenghtLong,n_arm_points);linspace(0,0,n_arm_points)];%[linspace(0,0,n_arm_points);linspace(0,-armLenght,n_arm_points);linspace(0,0,n_arm_points)];

% heading line definition (is defined along North direction) 
headLineLenght = armLenghtLong*1.5; 
headLine = [linspace(0,headLineLenght,n_arm_points);linspace(0,0,n_arm_points);linspace(0,0,n_arm_points)];

% top line definition (is defined along Up direction) 
topLineLenght = armLenghtLong*1.5;
topLine = [linspace(0,0,n_arm_points);linspace(0,0,n_arm_points);linspace(0,topLineLenght,n_arm_points)];



% body movement (translation + rotation)
arm1 = moveBody(arm1,position_NEU,NEU2BODY_mat);
arm2 = moveBody(arm2,position_NEU,NEU2BODY_mat);
arm3 = moveBody(arm3,position_NEU,NEU2BODY_mat);
arm4 = moveBody(arm4,position_NEU,NEU2BODY_mat);
headLine = moveBody(headLine,position_NEU,NEU2BODY_mat);
topLine = moveBody(topLine,position_NEU,NEU2BODY_mat);


% body plot (in ENU reference frame)
armx=[arm1(2,:),arm2(2,:),arm3(2,:),arm4(2,:)];
army=[arm1(1,:),arm2(1,:),arm3(1,:),arm4(1,:)];
armz=[arm1(3,:),arm2(3,:),arm3(3,:),arm4(3,:)];

plot3(0,dir_axe,0,'g.',...
    armx,army,armz,'ro',...
    headLine(2,:),headLine(1,:),headLine(3,:),'k-',...
    topLine(2,:),topLine(1,:),topLine(3,:),'m-',...
     0,0,2*dir_axe,'g.',...
    dir_axe,0,0,'g.');
    text(0,dir_axe_len,0,'N');
    text(0,0,2*dir_axe_len,'U');
    text(dir_axe_len,0,0,'E');
    
  % orientation
%   az = -30; % azimuth
% el = 60;  % elevation
% axis orientation
    view(azimuth,elevation)
    % axis limit
    axis(axisLimit)
 grid on;


end

function movedBody = moveBody(body,displacement,NEU2BODY_mat)
% apply a rotation and then a displacement to an array of points in 3D

% init
sizeArm = size(body,2); 


neu_body_inv=transp(NEU2BODY_mat);
displacement_int = repmat(displacement,1,sizeArm);


movedBody=neu_body_inv*body+displacement_int;

end


