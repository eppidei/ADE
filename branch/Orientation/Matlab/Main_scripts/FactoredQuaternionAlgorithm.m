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
%%%%FILTERED AND NORMALIZED TO ONE%%%%%%%%%%%%%%
accel_x_body=resample(downsample(accel_x_data_plot,downsamp,1),N,D);
accel_y_body=resample(downsample(accel_y_data_plot,downsamp,1),N,D);
accel_z_body=resample(downsample(accel_z_data_plot,downsamp,1),N,D);
magneto_x_body=resample(downsample(magneto_x_data_plot,downsamp,1),N,D)/180;
magneto_y_body=resample(downsample(magneto_y_data_plot,downsamp,1),N,D)/180;
magneto_z_body=resample(downsample(magneto_z_data_plot,downsamp,1),N,D)/180;
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


qyaw=zeros(cycle_len,4);
qpitch=zeros(cycle_len,4);
qroll=zeros(cycle_len,4);

declinazione_magnetica_roma=2.44*pi/180;
intensita_magnetica_roma=46379.1e-9;
inclinazione_magnetica_roma=58.6*pi/180;
F=intensita_magnetica_roma;
D=declinazione_magnetica_roma;
I=inclinazione_magnetica_roma;
H=F*cos(I);
X=H*cos(D);
Y=H*sin(D);
Z=F*sin(I);
campo_magnetico_italia=[X;Y;Z]/norm([X;Y;Z]);%[11;33;5];%nanotesla
sy=zeros(1,cycle_len);
cy=zeros(1,cycle_len);

for i=1:cycle_len

    %%%pitch quaternion evaluation%%%%%
    a_norm=norm([accel_x_body_filt(i);accel_y_body_filt(i);accel_z_body_filt(i)]);
    sp=accel_x_body_filt(i)/a_norm;
    cp=sqrt(1-sp*sp);
    spd2=sin_ang_div2(sp,cp);
    cpd2=cos_ang_div2(cp);
    qpitch(i,:)=[cpd2;0;spd2;0];%%MATLAB NEEDS SCALAR COMP FIRST POS
    
    %%%%%%%%roll quaternion evaluation%%%%%
    
    sr=-accel_y_body_filt(i)/a_norm/cp;
    cr=accel_z_body_filt(i)/a_norm/cp;
    srd2=sin_ang_div2(sr,cr);
    crd2=cos_ang_div2(cr);
    qroll(i,:)=[crd2;srd2;0;0];
    
    q_roll_pitch=quatmultiply(qroll(i,:),qpitch(i,:));
    magneto_body_vect=[magneto_x_body_filt(i),magneto_y_body_filt(i),magneto_z_body_filt(i)]/norm([magneto_x_body_filt(i),magneto_y_body_filt(i),magneto_z_body_filt(i)]);
     magneto_earth=quatrotate(q_roll_pitch,magneto_body_vect);
%      magneto_earth2=quat_rotate(q_roll_pitch,magneto_body_vect);
     magneto_earth_x_y_norm=magneto_earth(1:2)/norm(magneto_earth(1:2));
     n_x_y=campo_magnetico_italia(1:2)/norm(campo_magnetico_italia(1:2));
%     
    %%%%%%yaw quaternion%%%%%%%%%%%%
    sy_cy=[magneto_earth_x_y_norm(1),magneto_earth_x_y_norm(2);-magneto_earth_x_y_norm(2),magneto_earth_x_y_norm(1)]*n_x_y;%%[1;1] if normalized
    sy(i)=sy_cy(2);
    cy(i)=sy_cy(1);
    syd2=sin_ang_div2(sy(i),cy(i));
    cyd2=cos_ang_div2(cy(i));
    qyaw(i,:)=[cyd2;0;0;syd2];
    
    %%%%ROTATION QUATERNION%%%%%
   qrot=quatmultiply(q_roll_pitch,qyaw(i,:));
%     qrot=quatmultiply(rot1, qroll(i,:));
    
    [y, p,r] = quat2angle(qroll(i,:),'ZYX');
    
      MobileViewer([0;0;0],y,p,r,37,55);
      


  

 
end

% State_degree=State*180;






