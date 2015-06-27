clear all;close all;clc

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
test_file ='increasing_blow.wav';
 file_path=[res_path,separator,test_file];

addpath(funcs_path)
addpath(mex_exec_path)
Frame_len = 256;
%%%
  [samples,Fs_wave,nbit]=wavread(file_path);
    if (size(samples,2)==2)
    audio_left = samples(:,1);
    audio_right = samples(:,2);
    elseif (size(samples,2)==1)
        audio_left = samples(:,1);
    else
       error('file format not recognized'); 
    end
    
    n_iterations=fix(length(audio_left)/Frame_len);
    
 %%%
 
 
 %%%%% BLOW CONFIGURATION %%%%%%

%nbit = 16;
Margin = 0.3;%0.35;
pow_thresh_high =0.25;% 0.2;
pow_thresh_low = 4e-3;
eval_time=100e-3;%40e-3;
Fs=44100;
if (Fs_wave~=Fs)
    
    error('check the sampling rate!');
end
time_pow_thresh_attack = 1e-3;%2e-3;
time_pow_thresh_release = 2e-3;
running_pow_win_time_fast = 1e-3;
running_pow_win_time_slow = 20e-3;

[bb_fast,bb_slow,sat_thresh,n_sat_thres,eval_time_samples,n_pow_thres_attack,n_pow_thres_release,running_pow_win_fast,running_pow_win_slow]=blow_config2(nbit,Margin,Fs,eval_time,...
    running_pow_win_time_fast,running_pow_win_time_slow,time_pow_thresh_attack,time_pow_thresh_release);

%%%%% BLOW INITIALIZATION %%%%%%

pstate=1;
state_fast = zeros(1,running_pow_win_fast-1);
state_slow = zeros(1,running_pow_win_slow-1);

eval_counter=0;
eval_pow=0;
eval_timer=0;

freq_pass = 20;
freq_stop = 60;
band_stop_rej_db = 40;
freq_pass2 = 2;
freq_stop2 = 5;
band_stop_rej_db2 = 40;
IIR_pow_filt = Iir_butterwoth_generator(1e-3,3e-3,1,band_stop_rej_db);
 IIR_pow_filt.PersistentMemory=true;
 IIR_pow_filt2 = Iir_butterwoth_generator(freq_pass2/(Fs/2),freq_stop2/(Fs/2),1,band_stop_rej_db2);
 IIR_pow_filt2.PersistentMemory=true;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%non linearity config
 fit_desiderata_x=[0,0.05,0.1,0.3,0.5,1];
     fit_desiderata_y=[0,0.05,0.1,0.3,0.7,1.5];
     [xData, yData] = prepareCurveData( fit_desiderata_x, fit_desiderata_y );

% Set up fittype and options.
ft = fittype( 'pchipinterp' );
opts = fitoptions( ft );

% Fit model to data.
[fitresult, gof] = fit( xData, yData, ft, opts );
test_inp = 0:1/255:1;
y = memoryless_blow_expander_fo(test_inp,fitresult);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%piecewise construction test
 my_exp = zeros(1,length(test_inp));
% for i=1:length(test_inp)
%     
%     data = test_inp(i);
%     
%     if (data>=fit_desiderata_x(1)) && (data<fit_desiderata_x(2))
%         my_exp(i)=fitresult.p.coefs(1,4)+fitresult.p.coefs(1,3)*(data-fit_desiderata_x(1))+fitresult.p.coefs(1,2)*(data-fit_desiderata_x(1))^2+fitresult.p.coefs(1,1)*(data-fit_desiderata_x(1))^3;
%     elseif (data>=fit_desiderata_x(2)) && (data<fit_desiderata_x(3))
%          my_exp(i)=fitresult.p.coefs(2,4)+fitresult.p.coefs(2,3)*(data-fit_desiderata_x(2))+fitresult.p.coefs(2,2)*(data-fit_desiderata_x(2))^2+fitresult.p.coefs(2,1)*(data-fit_desiderata_x(2))^3;
%     elseif (data>=fit_desiderata_x(3)) && (data<fit_desiderata_x(4))
%          my_exp(i)=fitresult.p.coefs(3,4)+fitresult.p.coefs(3,3)*(data-fit_desiderata_x(3))+fitresult.p.coefs(3,2)*(data-fit_desiderata_x(3))^2+fitresult.p.coefs(3,1)*(data-fit_desiderata_x(3))^3;
%     elseif (data>=fit_desiderata_x(4)) && (data<fit_desiderata_x(5))
%          my_exp(i)=fitresult.p.coefs(4,4)+fitresult.p.coefs(4,3)*(data-fit_desiderata_x(4))+fitresult.p.coefs(4,2)*(data-fit_desiderata_x(4))^2+fitresult.p.coefs(4,1)*(data-fit_desiderata_x(4))^3;
%     elseif (data>=fit_desiderata_x(5)) && (data<=fit_desiderata_x(6))
%           my_exp(i)=fitresult.p.coefs(5,4)+fitresult.p.coefs(5,3)*(data-fit_desiderata_x(5))+fitresult.p.coefs(5,2)*(data-fit_desiderata_x(5))^2+fitresult.p.coefs(5,1)*(data-fit_desiderata_x(5))^3;
%     end 
% end
% 
figure('Name','Non linearity')
plot(test_inp,test_inp,test_inp,y,'r',test_inp,my_exp,'+');


 hh=figure('Name','test_filt');

max_pow=1;

%%%%
n_samples = n_iterations*Frame_len;
run_pow_fast            =zeros(1,n_samples);
run_pow_slow_standard   =zeros(1,n_samples);
run_pow_iir =           zeros(1,n_samples);    
run_pow_iir_filt =           zeros(1,n_samples); 
test =           zeros(1,n_samples); 
normalized_pow_iir=zeros(1,n_samples);
expanded_pow_iir=zeros(1,n_samples);
state_plot=zeros(1,n_samples);
blow_plot=zeros(1,n_samples);
control_change=zeros(1,n_samples);

%%tews

      bbb_test=IIR_pow_filt2.sosMatrix(1,1:3)*IIR_pow_filt2.ScaleValues(1);
      aaa_test=IIR_pow_filt2.sosMatrix(1,4:6);
       bbb_test2=IIR_pow_filt2.sosMatrix(2,1:3)*IIR_pow_filt2.ScaleValues(2);
      aaa_test2=IIR_pow_filt2.sosMatrix(2,4:6);
       bbb_test3=IIR_pow_filt2.sosMatrix(3,1:3)*IIR_pow_filt2.ScaleValues(3);
      aaa_test3=IIR_pow_filt2.sosMatrix(3,4:6);
      state1=zeros(1,2);
      state2=zeros(1,2);
      state3=zeros(1,2);
for k=1:n_iterations
    
     idx = (k-1)*Frame_len+1:k*Frame_len;
     audio = audio_left(idx);
     audio_pow_squared=audio.^2;
     
     %%%standard pow calc based on time windows
     
     [run_pow_fast(idx),state_fast]=filter(bb_fast,1,audio_pow_squared,state_fast);
     [run_pow_slow_standard(idx),state_slow]=filter(bb_slow,1,audio_pow_squared,state_slow);
     [run_pow_iir(idx)]=filter(IIR_pow_filt,audio_pow_squared);%filter(bb_slow,1,audio.^2,state_slow);
    
      normalized_pow_iir(idx) = run_pow_iir(idx)/max_pow;
      
      
%       [test(idx),state1]=filter(bbb_test,aaa_test,normalized_pow_iir(idx),state1);
%        [test(idx),state2]=filter(bbb_test2,aaa_test2,test(idx),state2);
%        [test(idx),state3]=filter(bbb_test3,aaa_test3,test(idx),state3);
      
      [run_pow_iir_filt(idx)]=filter(IIR_pow_filt2,normalized_pow_iir(idx));

    expanded_pow_iir(idx)= memoryless_blow_expander_fo(run_pow_iir_filt(idx),fitresult);

    control_change(idx)=expanded_pow_iir(idx)*(128-50)+60;
 
   
    for j=1:Frame_len
        
        idxxx = (k-1)*Frame_len+j;
        
        [pstate,eval_counter,eval_pow,eval_timer] =saturation_detector_sample_based2(pstate,audio(j),run_pow_fast(idxxx),...
            run_pow_iir(idxxx),pow_thresh_high,pow_thresh_low,sat_thresh,n_sat_thres,...
            n_pow_thres_attack,n_pow_thres_release,eval_time_samples,eval_counter,eval_pow,eval_timer);
       state_plot(idxxx)=pstate;
        
        if pstate==3
           
             blow_plot(idxxx)=1;
             if blow_plot(idxxx-1)==0
             figure(hh)
             hold on;
             plot(run_pow_iir_filt(idx));
             plot(test(idx),'r');
             hold off;
             end
        end
    end

    
end
n_sub_plots = 7;
t_axe = 0: 1/Fs : (n_samples-1)/Fs;
    h1=figure('Name','Resuming plot');
    subplot(n_sub_plots,1,1)
    plot(t_axe,run_pow_iir);
    title('POW IIR');
    ylim([0 0.5]);
    subplot(n_sub_plots,1,3)
    plot(t_axe,normalized_pow_iir);
    title('NORMALIZED POW IIR');
    ylim([0 1]);
    subplot(n_sub_plots,1,4)
    plot(t_axe,audio_left(1:n_samples),t_axe,blow_plot,'r');
    legend('audio','blow');
    ylim([-1 1.1]);
    subplot(n_sub_plots,1,5)
    plot(t_axe,state_plot);
    title('STATES');
    subplot(n_sub_plots,1,2)
    plot(t_axe,expanded_pow_iir);
    title('EXPANDED IIR POW FILT');
    subplot(n_sub_plots,1,6)
    plot(t_axe,run_pow_slow_standard);
    title('POW SLOW STANDARD');
    subplot(n_sub_plots,1,7)
    plot(t_axe,run_pow_iir_filt);
    title('POW IIR FILT');
   h2=figure('Name','Control Change');
    plot(t_axe,control_change);
    title('CONTROL CHANGE');
    
% blow_state = zeros(1,n_samples);%[zeros(1,frame_len/4),ones(1,frame_len/2),zeros(1,frame_len/4)];
% blow_amp = 1;
% blow_rate = 0.8;
% note_state =blow_plot;% [zeros(1,n_samples/4),ones(1,frame_len/2),zeros(1,frame_len/4)];
% note_on_freq = 440;
% note_on_amp = 0.9;
% cg_state = blow_state;
% cg_num = 128;
% cg_freq = 0.5;
% cg_val = control_change;
% % 
%     data_out = stk_debug(blow_state,blow_amp,blow_rate,note_state,note_on_freq,note_on_amp,cg_state,cg_num,cg_val,n_samples);
%     
%     
% p=audioplayer(data_out,Fs);
% play(p);
  