clear all; close all;clc

sim_mat_common_params

  addpath ../ele_samples/

  file_name='schiocco.wma';

%   load schiochhi_ele1.mat;

[samples,n_bits]=audioread(file_name);
AudioInfo = audioinfo(file_name);
% 
% samples = double(squeeze(data.Data));
% AudioInfo.SampleRate = 44100;

%  stem(samples);

% Fs = 22050;
samples_mono = samples((1:end/2),1);

if AudioInfo.SampleRate > Fs
    [L,M]=rat(Fs/AudioInfo.SampleRate);
    actual_samples = resample(samples_mono,L,M);
else
    actual_samples = samples_mono;
    Fs = AudioInfo.SampleRate;
end



Ts = 1/Fs;
samples_len = length(actual_samples);
% frame_time_len = 300e-3;
% frame_len = round(frame_time_len*Fs)-1;
n_frames = fix(samples_len/frame_len);

time_axe= 0: Ts: (samples_len-1)* Ts;
n_frame_refresh_graph = 40;
n_subrows = 4;
n_subcol = n_frame_refresh_graph/n_subrows;


% at=1e-4;
% rt=50e-3;

% time_left = 0.5e-3;
% time_right = 5e-3;
max_time_right = 300e-3;

max_event_len_time = time_right+time_left;
    
    len_fft=512;
    
        
%     freq_left=2000;
%     freq_right=3000;
    
    max_events_per_frame = 1;

% samp_range_search_time = 80e-3;
% samp_range_search = ceil(samp_range_search_time*Fs)-1;

% spectral_threshold_schiocco  = 0.2;
% spectral_threshold_schiocco_high_var  = 0.4;
% max_range  = [2000,3000];
% spectral_threshold_struscio  = 0.08;

freq_axe = 0:Fs/(len_fft-1):Fs/2;

 h1=figure('Name','Tg frame');
 h2=figure('Name','Sample Frame');
h3=figure('Name','Tg smoothing'); 
  h4=figure('Name','Peak actually detected');
  
   !rm -rf ./extracted_samples/*.wav
   
   
   num_events_catched = 0;
   num_estimated_true_snaps = 0;
   num_estimated_false_snaps = 0;
   
   positive_events_idx = zeros(1,max_events_per_frame*n_frames);
   negative_events_idx = zeros(1,max_events_per_frame*n_frames);
   
   ene_frames=zeros(size(actual_samples));
   tot_thresh = zeros(frame_len,n_frames);
   last_tru_event_index = zeros(1,n_frames);
   max_time_bet_2_true_events = 1;
   max_distance_bet_2_true_events = max_time_bet_2_true_events*Fs;
   double_trans = zeros(1,n_frames);
    tot_var = zeros(max_events_per_frame,n_frames);
    
%     thresh_gain = 7;
%     thresh_bias = 2e-1;
%     search_step = 3;
%     look_ahead_step = 3;
    max_event_got=0;
    snip = zeros(n_frames*max_events_per_frame,len_fft/2);
    snap = zeros(n_frames*max_events_per_frame,len_fft/2);
    
    var_coll = zeros(n_frames*max_events_per_frame,2);
    bar_coll = zeros(n_frames*max_events_per_frame,2);
    old_frame = zeros(1,frame_len);
    
   
     
%      sample_left = fix(main_idx(i)-*Fs);
    extract_len = (time_right-time_left)*Fs;
%     extract_len=sample_right-sample_left+1;
     events_coll = zeros(n_frames*max_events_per_frame,extract_len);
   
for i=1:n_frames
    
    if mod(i-1,n_frame_refresh_graph)==0 && i>1
        close all;
        
    end

    sample_idx = (i-1)*frame_len+1:i*frame_len;
    actual_frame = actual_samples(sample_idx);
    actual_taxe = time_axe(sample_idx);
    sub_plot_idx = mod(i-1,n_frame_refresh_graph)+1;
    
    estimate = pow_est(actual_frame);
    
%     dbval = 10*log10(frame_pow);

    thres =thresh_gain.*estimate+thresh_bias;
    
    tot_thresh(:,i)=thres;
    [tresh_olds_idx,tg] = detect_energy_threshold(actual_frame,thres);
    
     ene_frames(sample_idx)=tg;
    tg_mean = mean(tg);
    
%     tg_str = ['Energy frame No. ',num2str(i)];
    mean_val_str = sprintf(' Tg mean = %f\n Max =%f \n Frame No.%u',tg_mean,max(actual_frame),i);
    
    figure(h1);
    subplot(n_subrows,n_subcol,sub_plot_idx);
    plot(actual_taxe,tg,actual_taxe,thres,actual_taxe,estimate);
    title(mean_val_str);
%     
%     Thresh_str = ['Sample Frame No. ',num2str(i)];
    figure(h2);
    subplot(n_subrows,n_subcol,sub_plot_idx);
    hold on;
    plot(actual_taxe,actual_frame);
    if isempty(tresh_olds_idx)==0
     plot(actual_taxe(tresh_olds_idx),0,'or');
    end
    hold off;
    title(mean_val_str);


    cc=xrms2(tg,at,rt,Fs);
    
   

%     Impsme_str = ['Impsme Frame No. ',num2str(i)];
    figure(h3);
    subplot(n_subrows,n_subcol,sub_plot_idx);
    plot(actual_taxe,cc,actual_taxe,thres);%,time_axe,samples_l,'r');
    title(mean_val_str);

%     peak_detect_str = ['Peak_search Frame No. ',num2str(i)];
    [ind,val,num]=find_local_max(cc,samp_range_search,thres,search_step,look_ahead_step);
    figure(h4);
     subplot(n_subrows,n_subcol,sub_plot_idx);
    hold on;
    plot(1:frame_len,actual_frame);
    if num>0
    plot(ind(1:num),0,'or');
    end
    hold off;
    title(mean_val_str);

% 
    [events_got,event_lengts] = extract_events(actual_frame,ind(1:num),time_left,time_right,Fs ,max_time_right); 
% 
[mval,jj]=max(event_lengts);
   if mval>len_fft
       mval_str =sprintf('len fft %d smaller than extracted event %d at frame %d\n',len_fft,event_lengts(jj),i);
       warning(mval_str);
   end
%     Hd_dc = highpass_iir(AudioInfo.SampleRate,10,200,60,1)http://www.libero.it/?cs_referrer=posta.libero.it;


     [spect,percent_pow,band_peak,vara,balance]= snap_recognition(events_got,num,Fs,len_fft,freq_left,freq_right,max_events_per_frame);
     
     tot_var(:,i)=vara;
% 
event_processes = num;

if num>max_events_per_frame
    event_processes=max_events_per_frame;
end
if num>max_event_got
    max_event_got=num;
end
     
     for j=1:event_processes
         
      
             if num==0 
                 break;
                 
%              elseif num>max_events_per_frame
                 
%                  warning('max events exceeded');
         
             else
                  speck = spect(j,1:len_fft/2);
                     str_pow = sprintf('  Pow band2= %f\n Th > %f Var %f peak %f \n',percent_pow(2,j),spectral_threshold_schiocco,vara(j),band_peak(j));
%                  str_pow = sprintf(' Pow band1= %f\n Th > %f < %f \n, Pow band2= %f\n Th > %f\n',percent_pow(1,j),spectral_threshold_struscio,spectral_threshold_schiocco,percent_pow(2,j),spectral_threshold_schiocco);
                      str=['Abolute events selected ',sprintf('%d',num_events_catched + j),' Frame ',sprintf('%d',i) ,' Rel event ',sprintf('%d',j)];
                    h_tmp=figure;
                    subplot(2,1,1);
                    plot(1:event_lengts(j),events_got(j,1:event_lengts(j)));
                    subplot(2,1,2);
                    plot(freq_axe,speck);
                    
                    
                   
                    if percent_pow(2,j)>spectral_threshold_schiocco && band_peak(j)<max_range(2) && band_peak(j)>max_range(1)
                        num_estimated_true_snaps=num_estimated_true_snaps+1;
                        positive_events_idx(num_estimated_true_snaps)=(i-1)*frame_len+ind(j);
                        
                        got_str = ' SNAP';
%                         figure(h5);
%                         hold on;
%                          plot(freq_axe,spect(j,1:len_fft/2));
%                          hold off;

                            snap((i-1)*max_events_per_frame+j,:)=speck;
                            events_coll((i-1)*max_events_per_frame+j,:)=events_got(j,1:extract_len);
                            var_coll((i-1)*max_events_per_frame+j,1)=vara(j);
                            bar_coll((i-1)*max_events_per_frame+j,1)=balance(j);
                       
                        
                 
                        
%                         if j>1
%                             state = 1;
%                             if (ind(j)-ind(j-1)) < max_distance_bet_2_true_events
%                                 double_trans(i)=1;
%                                 state = 0;
%                             end
%                         elseif mod(ind(j)-last_tru_event_index(i),frame_len) < max_distance_bet_2_true_events
%                             double_trans(i)=1;
%                             
%                         end
%                             
%                             last_tru_event_index(i)=ind(j);
                    else
                        num_estimated_false_snaps=num_estimated_false_snaps+1;
                        negative_events_idx(num_estimated_false_snaps)=(i-1)*frame_len+ind(j);
                        
                        got_str = ' SNIP';
%                         figure(h6);
%                         hold on;
%                          plot(freq_axe,speck);
%                          hold off;

                         snip((i-1)*max_events_per_frame+j,:)=speck;
                        
                            var_coll((i-1)*max_events_per_frame+j,2)=vara(j);
                            bar_coll((i-1)*max_events_per_frame+j,2)=balance(j);
                    end
                     title([str_pow,str,got_str]);
                   
             end
      
     end
 num_events_catched = num_events_catched + num;
end

fprintf('num_events_catched %d \n',num_events_catched);
fprintf('num_estimated_true_snaps %d \n',num_estimated_true_snaps);
fprintf('num_estimated_false_snaps %d \n',num_estimated_false_snaps);
fprintf('max events detected per frame %d \n',max_event_got);

save_audio2(actual_samples,positive_events_idx,num_estimated_true_snaps,negative_events_idx,num_estimated_false_snaps,'extracted_samples',Fs);
h7=figure('Name','Resuming plot');
plot(1:samples_len,actual_samples,positive_events_idx(1:num_estimated_true_snaps),0,'or',negative_events_idx(1:num_estimated_false_snaps),0,'*k',last_tru_event_index,0,'+g');

% thresholds=repmat(tot_thresh,frame_len,1);
 thresholds_l = reshape(tot_thresh,1,frame_len*n_frames);
h8=figure('Name','Tg plot');
plot(1:samples_len,ene_frames,1:frame_len*n_frames,thresholds_l);

h9=figure('Name','Variance plot');
plot(1:n_frames,tot_var);


figure('Name','SNAP Collection');
plot(freq_axe,snap);

figure('Name','SNIP Collection');
plot(freq_axe,snip);

