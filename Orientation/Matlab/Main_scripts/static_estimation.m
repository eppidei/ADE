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

%     load('dump_slow_rotation_yaw_body_ontable_ipad');
%    load('dump_0_45_slow2fast');
 load('dump_rotation_m90p90_ipad');

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
Faudio_sample=44100;
Frame_audio_len = 256;
Fframe_audio=Faudio_sample/Frame_audio_len;
Fsens=50;
[N,D]=rat(Fsens/Fframe_audio);

% h1=figure;
downsamp=Frame_audio_len;

Hd_accel = butterwoth_LP_design(2,5,1,50,Fsens);
Hd_magneto = butterwoth_LP_design(5,10,1,50,Fsens);

accel_x_body=resample(downsample(accel_x_data_plot,downsamp,1),N,D);
accel_y_body=resample(downsample(accel_y_data_plot,downsamp,1),N,D);
accel_z_body=resample(downsample(accel_z_data_plot,downsamp,1),N,D);
magneto_x_body=resample(downsample(magneto_x_data_plot,downsamp,1),N,D)*pi/180;
magneto_y_body=resample(downsample(magneto_y_data_plot,downsamp,1),N,D)*pi/180;
magneto_z_body=resample(downsample(magneto_z_data_plot,downsamp,1),N,D)*pi/180;
cycle_len = length(accel_x_body);
%-71,18,-46 magneto init

%  h1=figure('Name','Earth frame evo');

%%%%ENFORCE STATIC CONDITIONS%%%

accel_x_body_filt=filtfilt(Hd_accel.SOSMatrix,Hd_accel.ScaleValues,accel_x_body);
accel_y_body_filt=filtfilt(Hd_accel.SOSMatrix,Hd_accel.ScaleValues,accel_y_body);
accel_z_body_filt=filtfilt(Hd_accel.SOSMatrix,Hd_accel.ScaleValues,accel_z_body);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%NOISE FILT%%%%%%%%%%%%%%
magneto_x_body_filt=filtfilt(Hd_magneto.SOSMatrix,Hd_magneto.ScaleValues,magneto_x_body);
magneto_y_body_filt=filtfilt(Hd_magneto.SOSMatrix,Hd_magneto.ScaleValues,magneto_y_body);
magneto_z_body_filt=filtfilt(Hd_magneto.SOSMatrix,Hd_magneto.ScaleValues,magneto_z_body);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%




for i=1:cycle_len

    %true in static condition
    pitch=asin(accel_x_body_filt(i)/grav);
    roll=asin(accel_y_body_filt(i)/accel_z_body_filt(i));
    %%%%%%%%
    C=update_rotation_matrix_body2earth(0,pitch,roll);
    
    magneto_earth=C*[magneto_x_body_filt(i);magneto_y_body_filt(i);magneto_z_body_filt(i)];
    yaw=-atan(magneto_earth(2)/magneto_earth(1));
    
    State(:,i)=[yaw;pitch;roll];
    
   
%      R=update_rotation_matrix_earth2body(yaw,pitch,roll);
%      evolution(:,:,i)=R*ipad_vertex;


  MobileViewer([0;0;0],[roll,pitch,yaw],37,55);

  
 
   

 
end

% State_degree=State*180;






