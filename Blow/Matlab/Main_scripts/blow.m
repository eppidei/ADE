clear all; close all;clc

nbit = 16;
Margin = 0.3;

addpath ../blow

  file_name='Blow-s4mini-2.m4a';
  max_amp = 1-2^(-(nbit-1));

  sat_thresh = (1-Margin)*max_amp;
  
  [samples,n_bits]=audioread(file_name);
AudioInfo = audioinfo(file_name);

Fs = 44100;

samples_mono = samples(:,1);

% figure
% plot(samples_mono);




if AudioInfo.SampleRate > Fs
    [L,M]=rat(Fs/AudioInfo.SampleRate);
    actual_samples = resample(samples_mono,L,M);
else
    actual_samples = samples_mono;
    Fs = AudioInfo.SampleRate;
end

frame_time_len = 300e-3;
frame_len = round(frame_time_len*Fs)-1;

Ts = 1/Fs;
samples_len = length(actual_samples);

n_frames = fix(samples_len/frame_len);

time_axe= 0: Ts: (samples_len-1)* Ts;

% freq_axe = 0:Fs/(len_fft-1):Fs/2;
pow_thresh_high = 0.3;
pow_thresh_low = 4e-3;

eval_time  = 100e-3;
eval_time_samples = round(eval_time*Fs)+1;
n_sat_thres=20;%round(eval_time_samples/20);

pstate.Name='waiting';
pstate.Id = 1;
max_triggers_per_sample = 4;
trigger_collection  = zeros(n_frames,max_triggers_per_sample);
% stateT=struct('Name','','Id',0);
state_collection = zeros(1,samples_len);
runpow_collection_fast = zeros(1,samples_len);
runpow_collection_slow = zeros(1,samples_len);
% pow_acc=0;
running_pow_win_time_fast = 1e-3;
running_pow_win_fast = round(running_pow_win_time_fast*Fs)+1;
running_pow_win_time_slow = 100e-3;
running_pow_win_slow = round(running_pow_win_time_slow*Fs)+1;

time_pow_thresh = 2e-3;
% 
% if time_pow_thresh<eval_time
%     error('azz');
% end

n_pow_thres_samples=round(time_pow_thresh*Fs)+1;
n_pow_thres=round(n_pow_thres_samples);
win_counter=0;

 bb_fast=1/running_pow_win_fast*ones(1,running_pow_win_fast);
 state_fast = zeros(1,running_pow_win_fast-1);
 bb_slow=1/running_pow_win_slow*ones(1,running_pow_win_slow);
 state_slow = zeros(1,running_pow_win_slow-1);
figure('Name','Pow Global');
plot(filter(bb,1,actual_samples.^2));
 for i=1:n_frames
    
     sample_idx = (i-1)*frame_len+1:i*frame_len;
    actual_frame = actual_samples(sample_idx);
%      plot(abs(fft(actual_frame)));
    actual_taxe = time_axe(sample_idx);
    
    [run_pow_fast,state_fast]=filter(bb_fast,1,actual_frame.^2,state_fast);
    [run_pow_slow,state_slow]=filter(bb_slow,1,actual_frame.^2,state_slow);
    
     for j=1:frame_len
%         
         [nstate]=saturation_detector(pstate,actual_frame,j,run_pow_fast(j),run_pow_slow(j),pow_thresh_high,pow_thresh_low,sat_thresh,n_sat_thres,n_pow_thres,eval_time_samples);
%         
         state_collection((i-1)*frame_len+j)=nstate.Id;
% %         if strcmp(nstate.Name,'trackin') || strcmp(nstate.Name,'evaluation')
% %             runpow_collection((i-1)*frame_len+j)=pow_acc/win_counter;
% %         else
% %             runpow_collection((i-1)*frame_len+j)=0;
% %         end
% %         
         pstate=nstate;
     end
     runpow_collection_fast((i-1)*frame_len+1:i*frame_len)=run_pow_fast;
     runpow_collection_slow((i-1)*frame_len+1:i*frame_len)=run_pow_slow;
%     
%     last_frame=actual_frame;
end

figure('Name','Running pow plot fast');
plot(time_axe,runpow_collection_fast);%,time_axe,pow_thresh_high,'g',time_axe,pow_thresh_low,'r');
figure('Name','Running pow plot slow');
plot(time_axe,runpow_collection_slow);
figure('Name','State plot');
plot(time_axe,state_collection);


last_frame = zeros(1,frame_len);
  