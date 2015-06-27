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

%      load('dump_slow_rotation_yaw_body_ontable_ipad');
%    load('dump_0_45_slow2fast');
%  load('dump_rotation_m90p90_ipad');
 load('roll_p90_m90_various_headeings_const_pitch');

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
magneto_x_body=resample(downsample(magneto_x_data_plot,downsamp,1),N,D);
magneto_y_body=resample(downsample(magneto_y_data_plot,downsamp,1),N,D);
magneto_z_body=resample(downsample(magneto_z_data_plot,downsamp,1),N,D);

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

cycle_len =length( accel_x_body_filt);

pitch=asin(accel_x_body_filt)./grav;
roll=atan2(accel_y_body_filt,accel_z_body_filt);
%%avoid imaginary%%
% ind_rp1=find(roll>1);
% ind_rm1=find(roll<-1);
% roll(ind_rp1)=1;
% roll(ind_rm1)=-1;
%%%%%%%%%%%%%%%%%%%

C=angle2dcm(zeros(1,cycle_len),pitch,roll);
magneto_earth=zeros(3,cycle_len);
for i=1:cycle_len
magneto_earth(:,i)=transp(C(:,:,i))*[magneto_x_body_filt(i);magneto_y_body_filt(i);magneto_z_body_filt(i)];
end

yaw=-atan(magneto_earth(2,:)./magneto_earth(1,:));
State=[roll;pitch;yaw];
 for i=1:cycle_len
       MobileViewer([0;0;0],yaw(i),pitch(i),roll(i),37,55);
 end
%%%%% Animate %%%%%%%%%

% h=Aero.Animation;
% h.FramesPerSecond = Fs;
%  h.TimeScaling=1;
% idx1 = h.createBody('ac3d_xyzisrgb.ac','Ac3d');
% 
% time_axe=0:1/Fs:1/Fs*(cycle_len-1);
% position = zeros(cycle_len,3);
% tsdata=[time_axe',position,State'];
% h.Bodies{1}.TimeseriesSource=tsdata;
% h.Camera.ViewAngle=[45,45,45];
% for i=1:cycle_len
% h.move([0,0,0],[State(1,i),State(2,i),State(3,i)]);
% % end
% h.show();

% h.play();
% t = 0;
% h.updateBodies(t);
% h.updateCamera(t);
% h.Camera.PositionFcn = @staticCameraPosition;
% h.play();