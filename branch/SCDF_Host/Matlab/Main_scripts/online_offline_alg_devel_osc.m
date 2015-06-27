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
test_file ='ipad_forte_staccato.wav';

addpath(funcs_path)
addpath(mex_exec_path)



%%%%%% devel type %%%%%%%

devel_type='online';
bypass_dsp='false';
frame_len = 256;

if strcmp(devel_type,'online')
    local_ip = '192.168.1.51';
    remote_ip1 = 192;
    remote_ip2 =168;
    remote_ip3 =1;
    remote_ip4 =56';
    port = 50000;
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

% h2=figure('Name','Blow Data');
% set(0,'Units','pixels') ;
% set(h1,'OuterPosition',[800 100 1000 100]);

% h3=figure('Name','Pow Data');

sample_time=1/Fs;
plot_approx_duration = 15;

plot_duration =fix(plot_approx_duration/((frame_len)*sample_time))*(frame_len)*sample_time;
t_axe = 0 : sample_time : plot_duration-sample_time;
k= 1;
len_plot = length(t_axe);
audio_data_plot=zeros(1,len_plot);
accel_x_data_plot=zeros(1,len_plot);
proxy_data_plot=zeros(1,len_plot);
light_data_plot=zeros(1,len_plot);
blow_plot=zeros(1,len_plot);
state_plot=zeros(1,len_plot);
pow_plot_slow=zeros(1,len_plot);
compr_plot=zeros(1,len_plot);

pow_plot_fast=zeros(1,len_plot);
time_id_plot=zeros(1,(len_plot/frame_len));
 xrms_o=zeros(1,len_plot);

%%%%% BLOW CONFIGURATION %%%%%%

%nbit = 16;
Margin = 0.3;
pow_thresh_high = 0.3;
pow_thresh_low = 4e-3;
eval_time=40e-3;
Fs=44100;
time_pow_thresh_attack = 2e-3;
time_pow_thresh_release = 2e-3;
running_pow_win_time_fast = 1e-3;
running_pow_win_time_slow = 20e-3;
% [bb_fast,bb_slow,sat_thresh,n_sat_thres,eval_time_samples,n_pow_thres,running_pow_win_fast,running_pow_win_slow]=blow_config(nbit,Margin,Fs,eval_time,...
%     running_pow_win_time_fast,running_pow_win_time_slow,time_pow_thresh);

[bb_fast,bb_slow,sat_thresh,n_sat_thres,eval_time_samples,n_pow_thres_attack,n_pow_thres_release,running_pow_win_fast,running_pow_win_slow]=blow_config2(nbit,Margin,Fs,eval_time,...
    running_pow_win_time_fast,running_pow_win_time_slow,time_pow_thresh_attack,time_pow_thresh_release);
%%%%% BLOW INITIALIZATION %%%%%%

pstate=1;
state_fast = zeros(1,running_pow_win_fast-1);
state_slow = zeros(1,running_pow_win_slow-1);

eval_counter=0;
eval_pow=0;
eval_timer=0;


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% i=1;
refresh_rate=200;
kmax=len_plot/frame_len;
if refresh_rate>=kmax
    
    error('refresh rate too slow');
end
 accel_x=zeros(1,256);
 proxy=zeros(1,256);

 
%  Hd = Fir_windows_generator(200,0.005);
 Hd = Iir_butterwoth_generator(0.001,0.003,1,40);
% hhh=figure('Name','Power and LP Filter');
% [H1,W1]=freqz(Hd,1,64,'whole');
% Hd.freqz;
% [H2,W2]=freqz(bb_slow,1,64,'whole');
% plot(W1/pi,10*log10(H1),'b',W2/pi,10*log10(H2),'r');
%  plot(W2/pi,10*log10(H2),'r');
% state_alternative=zeros(1,length(Hd.Numerator)-1);
 for i=1:n_iterations
    
    if strcmp(devel_type,'online')
        [audio_buff,audio_len,accel_buff,accel_len,gyro_buff,gyro_len,...
            magneto_buff,magneto_len,proxy_buff,proxy_len,light_buff,light_len,byte,buffer_counter,init_dbg,audio_timestamps]=mex_udp_receiver_oneport_osc(local_ip,remote_ip1,remote_ip2,remote_ip3,remote_ip4,port);
%         if (num_samples~=frame_len && num_samples>0)
%              error('num samples differs from frame_len');
%         end
       if audio_len>0
         idx = (k-1)*audio_len+1:k*audio_len;
          audio=audio_buff(1:audio_len);
       else
           audio_len=frame_len;
            audio=zeros(1,audio_len);
             idx = (k-1)*audio_len+1:k*audio_len;
       end

    
         
          if (accel_len==3)
            accel_x=ones(1,audio_len)*accel_buff(1);
          elseif  (accel_len==6)
              warning('due dati');
              accel_x=[ones(1,audio_len/2)*accel_buff(1),ones(1,audio_len/2)*accel_buff(4)];
          elseif (accel_len>6)
              accel_len
              error('NNNNNN');
          elseif (accel_len==0)
              accel_x=zeros(1,audio_len);
          end
          if (proxy_len==1)
            proxy=ones(1,audio_len)*proxy_buff(1);
          elseif  (proxy_len==2)
              warning('due dati');
              proxy=[ones(1,audio_len/2)*proxy_buff(1),ones(1,audio_len/2)*proxy_buff(2)];
          elseif (proxy_len>2)
              proxy_len
              error('OOOOOOO');
          elseif (proxy_len==0)
              proxy=zeros(1,audio_len);
          end
          if (light_len==1)
            light=ones(1,audio_len)*light_buff(1);
          elseif  (light==2)
              warning('due dati');
              light=[ones(1,audio_len/2)*light_buff(1),ones(1,audio_len/2)*light_buff(2)];
          elseif (light_len>2)
              light_len
              error('OOOOOOO');
          elseif (proxy_len==0)
              light=zeros(1,audio_len);
          end
           time_id_plot(k)=buffer_counter;
    elseif strcmp(devel_type,'offline')
        audio_len=frame_len;
        audio_total = audio_left;
        
        idx = (k-1)*audio_len+1:k*audio_len;
        audio = audio_total(idx);
    else
         error('Unknown devel_type');
    end
    
   
     
           % audio_len = num_samples;%
           % audio_len =num_samples;%length(audio);
          
        %      audio_data_plot(idx)=audio(1:audio_len);
        %     accelx=audio(1:3:audio_len);
            
            audio_data_plot(idx)=audio;%accelx;
            
            accel_x_data_plot(idx)=accel_x;
             proxy_data_plot(idx)=proxy/5;
              light_data_plot(idx)=light;
            if mod(k,refresh_rate)==0
                    figure(h1);
               n_plots=6;
%                       plot((1:len_plot),audio_data_plot,'b',(1:len_plot),accel_x_data_plot,'r',(1:len_plot),light_data_plot,'g');
                 subplot(n_plots,1,1)
                 plot(t_axe,pow_plot_slow);
                 title('POW SLOW');
                 ylim([0 0.5]);
                 subplot(n_plots,1,2)
                 plot(t_axe,pow_plot_fast);
                  title('POW FAST');
                 ylim([0 0.5]);
                 subplot(n_plots,1,3)
                 plot(t_axe,audio_data_plot,t_axe,blow_plot,'r');
                 legend('audio','blow');
                 ylim([-1 1.1]);
                  subplot(n_plots,1,4)
                  plot(t_axe,state_plot);
                   title('STATES');
                  subplot(n_plots,1,5)
                  plot(t_axe,compr_plot);
                   title('POW SLOW CG');
                   subplot(n_plots,1,6)
                  plot(t_axe,teager_kaiser(audio_data_plot));
                   title('TK operator');
%                   ylim([0 128]);
                 
%                     if strcmp(devel_type,'offline')
%                        figure(h2);
%                        plot(t_axe,blow_plot);
%                        ylim([0 3]);
%                         figure(h3);
%                        plot(t_axe,pow_plot_slow);
%                     end
                  
            end
    
%              [run_pow_slow,state_slow]=filter(bb_slow,1,audio.^2,state_slow);
%               pow_plot_slow(idx)=run_pow_slow;
            
   if strcmp(bypass_dsp,'false') 
    
     [run_pow_fast,state_fast]=filter(bb_fast,1,audio.^2,state_fast);
%      [run_pow_slow,state_slow]=filter(bb_slow,1,audio.^2,state_slow);
     Hd.PersistentMemory=true;
    [run_pow_slow]=filter(Hd,audio.^2);%filter(bb_slow,1,audio.^2,state_slow);
%     run_pow_fast=run_pow_slow;
     y = compressor(run_pow_slow,0.1,2/3);
%     pow_plot_slow(idx)=run_pow_slow;
    %xrms_o((k-1)*audio_len+1:k*audio_len)=xrms(audio,1e-4,2e-3,44100);
    
    compr_plot((k-1)*audio_len+1:k*audio_len)=y;
    for j=1:audio_len
         
        [pstate,eval_counter,eval_pow,eval_timer] =saturation_detector_sample_based2(pstate,audio(j),run_pow_fast(j),...
            run_pow_slow(j),pow_thresh_high,pow_thresh_low,sat_thresh,n_sat_thres,...
            n_pow_thres_attack,n_pow_thres_release,eval_time_samples,eval_counter,eval_pow,eval_timer);
       state_plot((k-1)*audio_len+j)=pstate;
        pow_plot_slow((k-1)*audio_len+j)=run_pow_slow(j);
        pow_plot_fast((k-1)*audio_len+j)=run_pow_fast(j);
        if pstate==3
           
             blow_plot((k-1)*audio_len+j)=1;
        end
    end

%  for j=1:audio_len
%      if proxy(j)==0
%           pow_plot_slow((k-1)*audio_len+j)=run_pow_slow(j);
%      end 
%  end

   end
    
    k=k+1;
    %     i=i+1;
    if k==kmax
        
        
        k=1;
        %         i=1;
        wavwrite(audio_data_plot,Fs,'ipad_piano_piano_staccato_flauto.wav');
        audio_data_plot=zeros(1,len_plot);
        accel_x_data_plot=zeros(1,len_plot);
         proxy_data_plot=zeros(1,len_plot);
         light_data_plot=zeros(1,len_plot);
        blow_plot=zeros(1,len_plot);
        state_plot=zeros(1,len_plot);
        pow_plot_slow=zeros(1,len_plot);
        compr_plot=zeros(1,len_plot);
        pow_plot_fast=zeros(1,len_plot);
        time_id_plot=zeros(1,(len_plot/frame_len));
        xrms_o=zeros(1,(len_plot));
        
        
    end
    
end