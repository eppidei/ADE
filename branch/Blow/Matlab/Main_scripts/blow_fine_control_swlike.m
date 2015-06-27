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
 
 pat='/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/Blow/Matlab/Main_scripts';
 
res_path =[pat,separator,up_dir,'resources'];%'/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/SCDF_Host/Matlab/resources';%
funcs_path = [pat,separator,up_dir,'Alg_funcs'];
mex_exec_path = [pat,separator,up_dir,'Mex',separator,'exec'];
%  load([res_path,'/blow_refine2']);
test_file ='increasing_blow.wav';
 file_path=[res_path,separator,test_file];
 


addpath(funcs_path)
addpath(mex_exec_path)
%% Configuration
Frame_len = 256;
%%%
  
 [audio_left,audio_right,audioinfos,n_iterations] = get_input_samples (file_path,Frame_len);
% 
%  audio_left=audio_data_plot;
%  n_iterations=fix(length(audio_left)/Frame_len);

 %%%
 
nbit = 16;%audioinfos.BitsPerSample;
Margin = 0.3;%0.35;
pow_thresh_high =0.25;% 0.2;
pow_thresh_low = 4e-3;
eval_time=100e-3;%40e-3;
Fs =  44100;%audioinfos.SampleRate;
% if (Fs_wave~=Fs)
%     
%     error('check the sampling rate!');
% end
time_pow_thresh_attack = 1e-3;%2e-3;
time_pow_thresh_release = 2e-3;
running_pow_win_time_fast = 1e-3;
running_pow_win_time_slow = 20e-3;

[bb_fast,bb_slow,sat_thresh,n_sat_thres,eval_time_samples,n_pow_thres_attack,n_pow_thres_release,running_pow_win_fast,running_pow_win_slow]=blow_config2(nbit,Margin,Fs,eval_time,...
    running_pow_win_time_fast,running_pow_win_time_slow,time_pow_thresh_attack,time_pow_thresh_release);

freq_pass = 40;
freq_stop = 80;
band_stop_rej_db = 40;
freq_pass2 = 0.8;
freq_stop2 = 10;
band_stop_rej_db2 =3;
dec_fact=32;


IIR_pow_filt = Iir_butterwoth_generator(freq_pass/(Fs/2),freq_stop/(Fs/2),1,band_stop_rej_db);
 IIR_pow_filt.PersistentMemory=true;
 iir_scaleval=IIR_pow_filt.ScaleValues;
 iir_sosmatrix=IIR_pow_filt.sosMatrix;

 
%   IIR_pow_filt2 = Iir_Chebyshev_typ1_generator(freq_pass2/(Fs/2/dec_fact),freq_stop2/(Fs/2/dec_fact),1,band_stop_rej_db2);
  IIR_pow_filt2 = Iir_butterwoth_generator(freq_pass2/(Fs/2/dec_fact),freq_stop2/(Fs/2/dec_fact),1,band_stop_rej_db2);
%  FIR_pow_filt2 =  Fir_equiripple_generator_atten2db(freq_pass2/(Fs/2/dec_fact),freq_stop2/(Fs/2/dec_fact));
 IIR_pow_filt2.PersistentMemory=true;
  iir2_scaleval=IIR_pow_filt2.ScaleValues;

 
  
 
 max_pow=1;
 IIR_pow_filt2.SosMatrix(1,1:3)= IIR_pow_filt2.SosMatrix(1,1:3)/max_pow; %to avoid another moltiplication add norm gain 2 first section numerator
  iir2_sosmatrix=IIR_pow_filt2.sosMatrix;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%expander setup%%%%%%%%%%%
 fit_desiderata_x=[0,0.05,0.1,0.3,0.6,1];
 fit_desiderata_y=[0,0.05,0.1,0.3,0.6,1];
 [xData, yData] = prepareCurveData( fit_desiderata_x, fit_desiderata_y );

% Set up fittype and options.
ft = fittype( 'pchipinterp' );
opts = fitoptions( ft );

% Fit model to data.
[fitresult, gof] = fit( xData, yData, ft, opts );
test_inp = 0:1/255:1;
y = memoryless_blow_expander_fo(test_inp,fitresult);
y2 = memoryless_blow_expander(test_inp,fitresult);

breaks = fitresult.p.breaks;
coeffs = fitresult.p.coefs;

figure('Name','Non linearity')
plot(test_inp,test_inp,test_inp,y,'or',test_inp,y2,'+b');

%%%%blow initi
n_samples = n_iterations*Frame_len;
run_pow_fast            =zeros(1,n_samples);
run_pow_slow_standard   =zeros(1,n_samples);
run_pow_iir =           zeros(1,n_samples);    
run_pow_iir_filt =           zeros(1,n_samples/dec_fact); 
dec_out =           zeros(1,n_samples/dec_fact); 
test =           zeros(1,n_samples); 
normalized_pow_iir=zeros(1,n_samples);
expanded_pow_iir=zeros(1,n_samples/dec_fact);
state_plot=zeros(1,n_samples);
blow_plot=zeros(1,n_samples);
control_change=zeros(1,n_samples);
state1=zeros(1,2);
state2=zeros(1,2);
state3=zeros(1,2);

pstate=1;
state_fast = zeros(1,running_pow_win_fast-1);
state_slow = zeros(1,running_pow_win_slow-1);



% hh=figure('Name','test_filt');


sat_detect_struct.pow_thresh_high=pow_thresh_high;
sat_detect_struct.pow_thresh_low=pow_thresh_low;
sat_detect_struct.sat_thresh=sat_thresh;
sat_detect_struct.n_sat_thres=n_sat_thres;
sat_detect_struct.n_pow_thres_attack=n_pow_thres_attack;
sat_detect_struct.n_pow_thres_release=n_pow_thres_release;
sat_detect_struct.frame_len = Frame_len;

sat_detect_struct.eval_time_samples = eval_time_samples;
 sat_detect_struct.eval_counter=0;%zeros(1,Frame_len+1);%IL +1 È DOVUTO AL FATTO CHE PS STATE E NEXT STATE SONO INGLOBATI IN UNICA VARIABILE ALTRIMENTI CI
    sat_detect_struct.eval_pow=0;%zeros(1,Frame_len+1);% SAREBBERO STATI DUE ARRAY DI FRAME LEN
    sat_detect_struct.eval_timer=0;%zeros(1,Frame_len+1);
    sat_detect_struct.state=1;%ones(1,Frame_len+1);
    save('blow_config_ws');
%end Configuration

%% Blow
for k=1:n_iterations
     idx = (k-1)*Frame_len+1:k*Frame_len;
     audio = audio_left(idx);
     audio_pow_squared=audio.^2;
     
     %%%standard pow calc based on time windows
     
     %FIR
     [run_pow_fast(idx),state_fast]=filter(bb_fast,1,audio_pow_squared,state_fast);
     [run_pow_slow_standard(idx),state_slow]=filter(bb_slow,1,audio_pow_squared,state_slow);
     %IIR
     [run_pow_iir(idx)]=filter(IIR_pow_filt,audio_pow_squared);%filter(bb_slow,1,audio.^2,state_slow);
    
       normalized_pow_iir(idx) = run_pow_iir(idx);%/max_pow;
     
     
    frame_idx = (k-1)*Frame_len +1 : k*Frame_len;
    sat_detect_struct.audio_frame = audio;
    sat_detect_struct.run_pow_fast = run_pow_fast(frame_idx);
    sat_detect_struct.run_pow_slow = run_pow_iir(frame_idx);
    sat_detect_struct.blow = zeros(1,Frame_len);
   
    
    
    sat_detect_struct=saturation_detector_frame(sat_detect_struct);
%     for j=1:Frame_len
%         
%         idxxx = (k-1)*Frame_len+j;
%         
%         [pstate,eval_counter,eval_pow,eval_timer] =saturation_detector_sample_based2(pstate,audio(j),run_pow_fast(idxxx),...
%             run_pow_iir(idxxx),pow_thresh_high,pow_thresh_low,sat_thresh,n_sat_thres,...
%             n_pow_thres_attack,n_pow_thres_release,eval_time_samples,eval_counter,eval_pow,eval_timer);
%        state_plot(idxxx)=pstate;
%         
%         if pstate==3
%            
%              blow_plot(idxxx)=1;
% %              if blow_plot(idxxx-1)==0
% %              figure(hh)
% %              hold on;
% %              plot(run_pow_iir_filt(idx));
% %              plot(test(idx),'r');
% %              hold off;
% %              end
%         end
%     end

% sat_detect_struct.eval_counter(1)=sat_detect_struct.eval_counter(Frame_len+1);
% sat_detect_struct.eval_pow(1)=sat_detect_struct.eval_pow(Frame_len+1);
% sat_detect_struct.eval_timer(1)=sat_detect_struct.eval_timer(Frame_len+1);
% sat_detect_struct.state(1)=sat_detect_struct.state(Frame_len+1);

blow_plot(frame_idx)= sat_detect_struct.blow;
% state_plot(frame_idx)= sat_detect_struct.state(2:end);

%%%% POST PROCESSING %%%%%%%%%%

       %%%MORE POW FILTERING
       
       %%%decimate step %%%%%
       dec_idx=(k-1)*Frame_len/dec_fact+1:k*Frame_len/dec_fact;
       
%        dec_out(dec_idx) = decimate(normalized_pow_iir(idx),dec_fact);
 dec_out(dec_idx) = downsample(normalized_pow_iir(idx),dec_fact);
      
%       [run_pow_iir_filt(idx)]=filter(IIR_pow_filt2,normalized_pow_iir(idx));
         [run_pow_iir_filt(dec_idx)]=filter(IIR_pow_filt2,dec_out(dec_idx));
 
%          [run_pow_iir_filt(dec_idx)]=filtfilt(IIR_pow_filt2.sosMatrix,IIR_pow_filt2.ScaleValues,dec_out(dec_idx));
   %%%EXPANSION%%%%
%    tic
    expanded_pow_iir(dec_idx)= memoryless_blow_expander(run_pow_iir_filt(dec_idx),fitresult);
%     toc
%     tic
    testtt= memoryless_blow_expander_matrix(run_pow_iir_filt(dec_idx),fitresult);
%     toc
    
%     sum(abs(expanded_pow_iir(idx)-testtt'))
    
end
%end Blow
      
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
%     plot(t_axe,expanded_pow_iir);
    title('EXPANDED IIR POW FILT');
    subplot(n_sub_plots,1,6)
    plot(t_axe,run_pow_slow_standard);
    title('POW SLOW STANDARD');
    subplot(n_sub_plots,1,7)
%     plot(t_axe,run_pow_iir_filt);
    title('POW IIR FILT');
%    h2=figure('Name','Control Change');
%     plot(t_axe,control_change);
%     title('CONTROL CHANGE');

F_dec=Fs/dec_fact;
    n_sub_plots = 3;
t_axe_dec = 0: 1/F_dec : (n_samples/dec_fact-1)/F_dec;
h2=figure('Name','Resuming plot');

subplot(n_sub_plots,1,1)
plot(t_axe_dec,dec_out);
title('DECIMATED NORM POW');

 subplot(n_sub_plots,1,2)
plot(t_axe_dec,run_pow_iir_filt);
title('RUN POW IIR FILT');

 subplot(n_sub_plots,1,3)
plot(t_axe_dec,expanded_pow_iir);
title('EXPANDED IIR POW FILT');
