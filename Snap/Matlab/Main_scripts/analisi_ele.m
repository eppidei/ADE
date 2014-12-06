clear all; close all;clc

addpath ../ele_samples/

file_name='schiocco.wma';

[samples,n_bits]=audioread(file_name);
AudioInfo = audioinfo(file_name);
% 
% fid = fopen('../intsamplescla.txt','r');
% 
% 
% samp=fscanf(fid,'%f');
% 
% fclose(fid);
% 
% samples=samp*2^(-15);
% AudioInfo.SampleRate=44100;

time_axe= 0: inv(AudioInfo.SampleRate): (length(samples)-1)* inv(AudioInfo.SampleRate);

samples_l=samples(:,1);
thres = 0.2;
[tresh_olds_idx,tg] = detect_energy_threshold(samples_l,thres);

figure('Name','Tg_detection');
hold on;
plot(time_axe,tg);
hold off;

figure('Name','Thresh_detection');
hold on;
plot(samples_l);
plot(tresh_olds_idx,0,'or');
hold off;

at=1e-4;
rt=50e-3;

cc=xrms2(tg,at,rt,AudioInfo.SampleRate);

figure('Name','Impulse smeared');
plot(time_axe,cc);%,time_axe,samples_l,'r');

samp_range_search = 100;
[ind,val,num]=find_local_max(cc,samp_range_search,thres);
figure('Name','events selected');
hold on;
plot(1:length(samples_l),samples_l);
plot(ind(1:num),0,'or');
hold off;

time_left = 2e-3;
time_right=20e-3;
max_time_right = 300e-3;

[events_got,event_lengts] = extract_events(samples_l,ind(1:num),time_left,time_right,AudioInfo.SampleRate ,max_time_right); 

!rm -rf ./extracted_samples/*.wav
Hd_dc = highpass_iir(AudioInfo.SampleRate,10,200,60,1);

len_fft=event_lengts(1);
freq_left=1500;
freq_right=3500;
[spect,percent_pow]= snap_recognition(events_got,num,AudioInfo.SampleRate,len_fft,freq_left,freq_right);

for i=1:length(event_lengts)

str=['events selected',num2str(i)];
freq_axe = 0:AudioInfo.SampleRate/(len_fft-1):AudioInfo.SampleRate/2;
figure('Name',str);
subplot(2,1,1)
plot(events_got(i,1:event_lengts(i)));
% subplot(4,1,2)
% dc_rem = filter(Hd_dc,events_got(i,1:event_lengts(i)));
% freq_anal = (fft(dc_rem).^2);

% plot(freq_axe,(abs(freq_anal(1:length(freq_axe)))));
subplot(2,1,2)
% freq_anal_nofilt = (fft(events_got(i,1:event_lengts(i))).^2);
plot(freq_axe,spect(i,1:len_fft/2));
pow_est=sprintf('%f',percent_pow(i));
title(['Pow fraction ',pow_est]);
 save_audio(events_got(i,:),i,event_lengts(i),'extracted_samples',AudioInfo.SampleRate)

% subplot(3,1,3)
% freq_anal_nofilt = (fft(events_got(i,1:event_lengts(i))));
% plot(freq_axe,(angle(freq_anal_nofilt(1:length(freq_axe)))));
% save_audio(events_got(i,:),i,event_lengts(i),'extracted_samples',AudioInfo.SampleRate)




end

frame_len = 500e-3*AudioInfo.SampleRate;
