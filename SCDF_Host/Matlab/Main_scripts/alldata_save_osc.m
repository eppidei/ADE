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

res_path = [up_dir,'resources'];
funcs_path = [up_dir,'Alg_funcs'];
mex_exec_path = [up_dir,'Mex',separator,'exec'];
test_file ='testaudio_soffiodistante.wav';

addpath(funcs_path)
addpath(mex_exec_path)

%%%%%%%%%MAIN VARIABLES%%%%%%%%%
save_ws_path=[res_path,separator,'roll_p90_m90_various_headeings_const_pitch'];
frame_len = 256;
plot_approx_duration = 10;



%%%%%% devel type %%%%%%%

devel_type='online';


if strcmp(devel_type,'online')
    local_ip = '192.168.1.52';
    remote_ip1 = 192;
    remote_ip2 =168;
    remote_ip3 =1;
    remote_ip4 =6;
    port = 50000;
    Fs = 44100;
    nbit = 16;
    n_iterations = 2^32-1;
elseif strcmp(devel_type,'offline')
    file_path=[res_path,separator,test_file];
    [samples,Fs,nbit]=wavread(file_path);
    if (size(samples,2)==2)
        audio_left = samples(:,1);
        audio_right = samples(:,2);
    elseif (size(samples,2)==1)
        audio_left = samples(:,1);
    else
        error('file format not recognized');
    end
    
    n_iterations=fix(length(audio_left)/frame_len);
else
    
    error('Unknown devel_type');
end

% h1=figure('Name','Data Plot');
% set(0,'Units','pixels') ;
% sszie=get(0,'ScreenSize');
%  set(h1,'OuterPosition',[800 520 1000 520]);

sample_time=1/Fs;


plot_duration =fix(plot_approx_duration/((frame_len)*sample_time))*(frame_len)*sample_time;
t_axe = 0 : sample_time : plot_duration-sample_time;
k= 1;
len_plot = length(t_axe);
audio_data_plot=zeros(1,len_plot);
audio_frame_time_data_plot=zeros(1,len_plot);
audio_timeid_data_plot=zeros(1,len_plot);
accel_x_data_plot=zeros(1,len_plot);
accel_y_data_plot=zeros(1,len_plot);
accel_z_data_plot=zeros(1,len_plot);
n_accel_data_plot=zeros(1,len_plot);
gyro_x_data_plot=zeros(1,len_plot);
gyro_y_data_plot=zeros(1,len_plot);
gyro_z_data_plot=zeros(1,len_plot);
n_gyro_data_plot=zeros(1,len_plot);
magneto_x_data_plot=zeros(1,len_plot);
magneto_y_data_plot=zeros(1,len_plot);
magneto_z_data_plot=zeros(1,len_plot);
n_magneto_data_plot=zeros(1,len_plot);
proxy_data_plot=zeros(1,len_plot);
light_data_plot=zeros(1,len_plot);
% time_id_plot=zeros(1,(len_plot));
udp_buff_plot=zeros(1,(len_plot));

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% i=1;

kmax=len_plot/frame_len;
% if refresh_rate>=kmax
%     
%     error('refresh rate too slow');
% end
accel_x=zeros(1,frame_len);
accel_y=zeros(1,frame_len);
accel_z=zeros(1,frame_len);
audio_frame_time=zeros(1,frame_len);
audio_timeid=zeros(1,frame_len);
n_accel=zeros(1,frame_len);
magneto_x=zeros(1,frame_len);
magneto_y=zeros(1,frame_len);
magneto_z=zeros(1,frame_len);
n_magneto=zeros(1,frame_len);
gyro_x=zeros(1,frame_len);
gyro_y=zeros(1,frame_len);
gyro_z=zeros(1,frame_len);
n_gyro=zeros(1,frame_len);
proxy=zeros(1,frame_len);
light=zeros(1,frame_len);
 fprintf('************Acquisition Started ********************\n');
for i=1:kmax
  
    if strcmp(devel_type,'online')
        [audio_buff,audio_len,accel_buff,accel_len,gyro_buff,gyro_len,...
            magneto_buff,magneto_len,proxy_buff,proxy_len,light_buff,light_len,byte,buffer_counter,init_dbg,audio_timestamps,audio_timeid]=mex_udp_receiver_oneport_osc(local_ip,remote_ip1,remote_ip2,remote_ip3,remote_ip4,port);
        if (audio_len~=frame_len && audio_len>0)
            error('num samples differs from frame_len');
        end
       
        idx = (k-1)*audio_len+1:k*audio_len;
        
        %%%%%% AUDIO
        audio=audio_buff(1:audio_len);
        audio_data_plot(idx)=audio;
%         str_audio_len = [' Frame_len = ',num2str(audio_len)];
        %%%%%%FRAME TIME %%%%%%%%%%%
        
        audio_frame_time=ones(1,audio_len)*(audio_timestamps(2)-audio_timestamps(1));
        audio_timeid=ones(1,audio_len)*audio_timeid;
        
        %%%%% ACCELEROMETER
        n_accel=ones(1,audio_len)*accel_len;
        if (accel_len==3)
            accel_x=ones(1,audio_len)*accel_buff(1);
            accel_y=ones(1,audio_len)*accel_buff(2);
            accel_z=ones(1,audio_len)*accel_buff(3);
        elseif  (accel_len==6)
            accel_x=[ones(1,audio_len/2)*accel_buff(1),ones(1,audio_len/2)*accel_buff(4)];
            accel_y=[ones(1,audio_len/2)*accel_buff(2),ones(1,audio_len/2)*accel_buff(5)];
            accel_z=[ones(1,audio_len/2)*accel_buff(3),ones(1,audio_len/2)*accel_buff(6)];
        elseif  (accel_len==9)
%             warning('9');
            accel_x=[ones(1,fix(audio_len/3))*accel_buff(1),ones(1,fix(audio_len/3))*accel_buff(4),ones(1,fix(audio_len/3)+1)*accel_buff(7)];
            accel_y=[ones(1,fix(audio_len/3))*accel_buff(2),ones(1,fix(audio_len/3))*accel_buff(5),ones(1,fix(audio_len/3)+1)*accel_buff(8)];
            accel_z=[ones(1,fix(audio_len/3))*accel_buff(3),ones(1,fix(audio_len/3))*accel_buff(6),ones(1,fix(audio_len/3)+1)*accel_buff(9)];
        elseif (accel_len>9 && accel_len<99)
            warning('Num accel data not handled %d',accel_len);
         elseif (accel_len>99)
             
             error('Num accel data not handled %d',accel_len);
        end
        
        audio_frame_time_data_plot(idx)=audio_frame_time;
        audio_timeid_data_plot(idx)=audio_timeid;
        accel_x_data_plot(idx)=accel_x;
        accel_y_data_plot(idx)=accel_y;
        accel_z_data_plot(idx)=accel_z;
        n_accel_data_plot(idx)=n_accel;
%          str_accel_len = [' Frame_len = ',num2str(accel_len)];
        %%%%% GYROSCOPE
         n_gyro=ones(1,audio_len)*gyro_len;
        if (gyro_len==3)
            gyro_x=ones(1,audio_len)*gyro_buff(1);
            gyro_y=ones(1,audio_len)*gyro_buff(2);
            gyro_z=ones(1,audio_len)*gyro_buff(3);
        elseif  (gyro_len==6)
            gyro_x=[ones(1,audio_len/2)*gyro_buff(1),ones(1,audio_len/2)*gyro_buff(4)];
            gyro_y=[ones(1,audio_len/2)*gyro_buff(2),ones(1,audio_len/2)*gyro_buff(5)];
            gyro_z=[ones(1,audio_len/2)*gyro_buff(3),ones(1,audio_len/2)*gyro_buff(6)];
        elseif  (gyro_len==9)
%              warning('9');
            gyro_x=[ones(1,fix(audio_len/3))*gyro_buff(1),ones(1,fix(audio_len/3))*gyro_buff(4),ones(1,fix(audio_len/3)+1)*gyro_buff(7)];
            gyro_y=[ones(1,fix(audio_len/3))*gyro_buff(2),ones(1,fix(audio_len/3))*gyro_buff(5),ones(1,fix(audio_len/3)+1)*gyro_buff(8)];
            gyro_z=[ones(1,fix(audio_len/3))*gyro_buff(3),ones(1,fix(audio_len/3))*gyro_buff(6),ones(1,fix(audio_len/3)+1)*gyro_buff(9)];
        elseif (gyro_len>9 && gyro_len<99)
             warning('Num gyro data not handled %d',gyro_len);
        elseif (gyro_len>99)
            error('Num gyro data not handled %d',gyro_len);
        end
%           str_gyro_len = [' Frame_len = ',num2str(giro_len)];
        gyro_x_data_plot(idx)=gyro_x;
        gyro_y_data_plot(idx)=gyro_y;
        gyro_z_data_plot(idx)=gyro_z;
        n_gyro_data_plot(idx)=n_gyro;
        
        %%%%% MAGNETOSCOPE
         n_magneto=ones(1,audio_len)*magneto_len;
        if (magneto_len==3)
            magneto_x=ones(1,audio_len)*magneto_buff(1);
            magneto_y=ones(1,audio_len)*magneto_buff(2);
            magneto_z=ones(1,audio_len)*magneto_buff(3);
        elseif  (magneto_len==6)
            magneto_x=[ones(1,audio_len/2)*magneto_buff(1),ones(1,audio_len/2)*magneto_buff(4)];
            magneto_y=[ones(1,audio_len/2)*magneto_buff(2),ones(1,audio_len/2)*magneto_buff(5)];
            magneto_z=[ones(1,audio_len/2)*magneto_buff(3),ones(1,audio_len/2)*magneto_buff(6)];
        elseif  (magneto_len==9)
%             warning('9');
            magneto_x=[ones(1,fix(audio_len/3))*magneto_buff(1),ones(1,fix(audio_len/3))*magneto_buff(4),ones(1,fix(audio_len/3)+1)*magneto_buff(7)];
            magneto_y=[ones(1,fix(audio_len/3))*magneto_buff(2),ones(1,fix(audio_len/3))*magneto_buff(5),ones(1,fix(audio_len/3)+1)*magneto_buff(8)];
            magneto_z=[ones(1,fix(audio_len/3))*magneto_buff(3),ones(1,fix(audio_len/3))*magneto_buff(6),ones(1,fix(audio_len/3)+1)*magneto_buff(9)];
        elseif (magneto_len>9 && magneto_len<99)
            warning('Num magneto data not handled %d',magneto_len);
        elseif (magneto_len>99)
            error('Num magneto data not handled %d',magneto_len);
        end
%          str_magneto_len = [' Frame_len = ',num2str(magneto_len)];
        magneto_x_data_plot(idx)=magneto_x;
        magneto_y_data_plot(idx)=magneto_y;
        magneto_z_data_plot(idx)=magneto_z;
        n_magneto_data_plot(idx)=n_magneto;
        
        %%%%% PROXIMITY
        
        if (proxy_len==1)
            proxy=ones(1,audio_len)*proxy_buff(1);
        elseif  (proxy_len==2)
            proxy=[ones(1,audio_len/2)*proxy_buff(1),ones(1,audio_len/2)*proxy_buff(2)];
        elseif (proxy_len>2)
            error('Num proxy data not handled %d',proxy_len);
        elseif proxy_len==0
             proxy=ones(1,audio_len)*2.5;
        end
%          str_proxy_len = [' Frame_len = ',num2str(proxy_len)];
        proxy_data_plot(idx)=proxy;
        
        %%%%% LIGHT
        
        if (light_len==1)
            light=ones(1,audio_len)*light_buff(1);
        elseif  (light==2)
            light=[ones(1,audio_len/2)*light_buff(1),ones(1,audio_len/2)*light_buff(2)];
        elseif (light_len>2)
            error('Num light data not handled %d',light_len);
        end
        
        light_data_plot(idx)=light;
%         str_light_len = [' Frame_len = ',num2str(light_len)];
        %%%% TIME ID
        
        udp_buff_plot(idx)=buffer_counter;
        
  k=k+1;      
 
        
    else
        error('Unknown devel_type');
    end
    



end

fprintf('************Acquisition Ended ********************\n');

save(save_ws_path);