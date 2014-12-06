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



%%%%%% devel type %%%%%%%

devel_type='online';
bypass_dsp='true';
frame_len = 256;
sensor_type='audio';

if strcmp(devel_type,'online')
    local_ip = '192.168.1.213';
    remote_ip = '192.168.1.238';
    port = 50003;
    Fs = 44100;
    nbit = 16;
    n_iterations = Inf;
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

h1=figure('Name','Audio Data');
% set(0,'Units','pixels') ;
% sszie=get(0,'ScreenSize');
% set(h1,'OuterPosition',[800 520 1000 520]);

h2=figure('Name','Blow Data');
% set(0,'Units','pixels') ;
% set(h1,'OuterPosition',[800 100 1000 100]);

h3=figure('Name','Pow Data');

sample_time=1/Fs;
plot_approx_duration = 5;

plot_duration =fix(plot_approx_duration/((frame_len)*sample_time))*(frame_len)*sample_time;
t_axe = 0 : sample_time : plot_duration-sample_time;
k= 1;
len_plot = length(t_axe);
audio_data_plot=zeros(1,len_plot);
blow_plot=zeros(1,len_plot);
pow_plot=zeros(1,len_plot);
time_id_plot=zeros(1,(len_plot/frame_len));

%%%%% BLOW CONFIGURATION %%%%%%

%nbit = 16;
Margin = 0.3;
pow_thresh_high = 0.3;
pow_thresh_low = 4e-3;
eval_time=100e-3;
Fs=44100;
time_pow_thresh = 2e-3;
running_pow_win_time_fast = 1e-3;
running_pow_win_time_slow = 100e-3;
[bb_fast,bb_slow,sat_thresh,n_sat_thres,eval_time_samples,n_pow_thres,running_pow_win_fast,running_pow_win_slow]=blow_config(nbit,Margin,Fs,eval_time,...
    running_pow_win_time_fast,running_pow_win_time_slow,time_pow_thresh);
%%%%% BLOW INITIALIZATION %%%%%%

pstate=1;
state_fast = zeros(1,running_pow_win_fast-1);
state_slow = zeros(1,running_pow_win_slow-1);
eval_counter=0;
eval_pow=0;
eval_timer=0;


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% i=1;
refresh_rate=40;
kmax=len_plot/frame_len;
if refresh_rate>=kmax
    
    error('refresh rate too slow');
end

 for i=1:n_iterations
    
    if strcmp(devel_type,'online')
        [audio,byte,buffer_counter,init_dbg,num_samples]=mex_udp_receiver_oneport(local_ip,remote_ip,port);
   
        if strcmp(sensor_type,'proxy')
              if (audio(1)>0 && num_samples>0)
                  fprintf('FAR!!!\n');
              elseif (audio(1)==0 && num_samples>0)
                  fprintf('CLOSE!!!\n');
              end
        elseif strcmp(sensor_type,'audio')
             audio=audio(1:num_samples);
                  if (num_samples~=frame_len && num_samples>0)
                        error('num samples differs from frame_len');
                  end
                  idx = (k-1)*num_samples+1:k*num_samples;
        end
         
         
           time_id_plot(k)=buffer_counter;
    elseif strcmp(devel_type,'offline')
        num_samples=frame_len;
        audio_total = audio_left;
        
        idx = (k-1)*num_samples+1:k*num_samples;
        audio = audio_total(idx);
    else
         error('Unknown devel_type');
    end
    
   
     
           % audio_len = num_samples;%
           % audio_len =num_samples;%length(audio);
          
        %      audio_data_plot(idx)=audio(1:audio_len);
        %     accelx=audio(1:3:audio_len);
            if strcmp(sensor_type,'audio')
                audio_data_plot(idx)=audio;%accelx;
            end
            if mod(k,refresh_rate)==0
                    figure(h1);
                    plot(audio_data_plot);
%                     ylim([-1 1]);
                    if strcmp(devel_type,'offline')
                       figure(h2);
                       plot(t_axe,blow_plot);
                       ylim([0 3]);
                        figure(h3);
                       plot(t_axe,pow_plot);
                    end
                  
            end
    
   if strcmp(bypass_dsp,'false') 
    
    [run_pow_fast,state_fast]=filter(bb_fast,1,audio.^2,state_fast);
    [run_pow_slow,state_slow]=filter(bb_slow,1,audio.^2,state_slow);
    
    for j=1:num_samples
        
        [pstate,eval_counter,eval_pow,eval_timer] =saturation_detector_sample_based(pstate,audio(j),run_pow_fast(j),...
            run_pow_slow(j),pow_thresh_high,pow_thresh_low,sat_thresh,n_sat_thres,...
            n_pow_thres,eval_time_samples,eval_counter,eval_pow,eval_timer);
        blow_plot((k-1)*num_samples+j)=pstate;
        if pstate==3
            pow_plot((k-1)*num_samples+j)=run_pow_slow(j);
        end
    end
   end
    
    k=k+1;
    %     i=i+1;
    if k==kmax
        
        
        k=1;
        %         i=1;
%                   wavwrite(audio_data_plot,Fs,'testaudio.wav');
        audio_data_plot=zeros(1,len_plot);
        blow_plot=zeros(1,len_plot);
        pow_plot=zeros(1,len_plot);
        time_id_plot=zeros(1,(len_plot/frame_len));
        
        
    end
    
end