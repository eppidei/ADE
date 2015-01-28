clear all; close all;clc


%%%%path definition
if (isunix || ismac)
    separator='/';
elseif ispc
    separator='\';
else
    warning('Computer unknown check path separators');
end

this_dir =['.',separator];
up_dir   =['..',separator];

res_path = [up_dir,up_dir,up_dir,'SCDF_Host',separator,'Matlab',separator,'resources'];
funcs_path = [up_dir,'Alg_funcs'];
% mex_exec_path = [up_dir,'Mex',separator,'exec'];


addpath(funcs_path)
addpath(res_path)

%   load('dump_slow_rotation_yaw_body_ontable_ipad');
 load('dump_0_45_slow2fast');

Fs = 50; 

% %[roll;pitch;yaw]; %bank angle,elevation,heading
% 
 len_data=length(gyro_x_data_plot);
 State = zeros(3,len_data);
 evolution =  zeros(3,4,len_data);
 ipad_vertex=transp([1,1,0;-1,1,0;-1,-1,0;1,-1,0]);
% State_apriori = zeros(3,len_data);
% w_ang = zeros(3,len_data);
% 
% 
grav=-1;%9.80665;
% h1=figure;
downsamp=256;
accel_x_body=downsample(accel_x_data_plot,downsamp,1);
accel_y_body=downsample(accel_y_data_plot,downsamp,1);
accel_z_body=downsample(accel_z_data_plot,downsamp,1);
magneto_x_body=downsample(magneto_x_data_plot,downsamp,1)*pi/180;
magneto_y_body=downsample(magneto_y_data_plot,downsamp,1)*pi/180;
magneto_z_body=downsample(magneto_z_data_plot,downsamp,1)*pi/180;
cycle_len = fix(len_data/downsamp);
%-71,18,-46 magneto init

%  h1=figure('Name','Earth frame evo');


for i=1:cycle_len

    %true in static condition
    pitch=asin(accel_x_body(i)/grav);
    roll=asin(accel_y_body(i)/accel_z_body(i));
    %%%%%%%%
    C=update_rotation_matrix_body2earth(0,pitch,roll);
    
    magneto_earth=C*[magneto_x_body(i);magneto_y_body(i);magneto_z_body(i)];
    yaw=-atan(magneto_earth(2)/magneto_earth(1));
    
    State(:,i)=[yaw;pitch;roll];
    
   
%      R=update_rotation_matrix_earth2body(yaw,pitch,roll);
%      evolution(:,:,i)=R*ipad_vertex;

if (mod(i,4)==0)
  MobileViewer([0;0;0],[roll,pitch,yaw],37,55);
end
  
 
   

 
end

% State_degree=State*180;






