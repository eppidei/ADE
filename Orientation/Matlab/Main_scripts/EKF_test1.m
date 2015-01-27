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

 load('dump_0_45_slow2fast');
% 
% magnet_stat.x=evaluate_statistics(magneto_x_data_plot,'earth');
% magnet_stat.y=evaluate_statistics(magneto_y_data_plot,'earth');
% magnet_stat.z=evaluate_statistics(magneto_z_data_plot,'earth');
% accel_stat.x=evaluate_statistics(accel_x_data_plot,'body');
% accel_stat.y=evaluate_statistics(accel_y_data_plot,'body');
% accel_stat.z=evaluate_statistics(accel_z_data_plot,'body');
% gyro_stat.x=evaluate_statistics(gyro_x_data_plot,'body');
% gyro_stat.y=evaluate_statistics(gyro_y_data_plot,'body');
% gyro_stat.z=evaluate_statistics(gyro_z_data_plot,'body');

Fs = 50; 

%[roll;pitch;jaw]; %bank angle,elevation,heading

len_data=length(gyro_x_data_plot);
State = zeros(3,len_data);
State_apriori = zeros(3,len_data);
w_ang = zeros(3,len_data);

roll0=magneto_x_data_plot(1);
pitch0=magneto_y_data_plot(1);
jaw0=magneto_x_data_plot(1);

State(:,1) = [roll0;pitch0;jaw0];


for i=1:len_data
    Rot_mat=update_din_matrix(State(1,i),State(2,i));
    w_ang=[gyro_x_data_plot(i);gyro_y_data_plot(i);gyro_z_data_plot(i)];
    
    State_apriori(:,i)=State(:,i)+Rot_mat*w_ang; %Backward Euler
 
end






