clear all;close all; clc

load events_coll

len = size(events_coll,2);
n_even = size(events_coll,1);

fft_len = 256;
my_events = zeros(n_even,fft_len);
my_events(:,1:len)=events_coll;
my_events=my_events';

tf = abs(fft(my_events,[],1)).^2;

spec = tf(1:fft_len/2,:);

med = sum(spec,2);


len_filt = 5;
bb=ones(1,10)/len_filt;

aaa=filter(bb,1,med);

[m,i]=max(aaa);

figure
hold on;
plot(aaa);
plot(med,'ob');
plot(i,0,'or');
hold off;