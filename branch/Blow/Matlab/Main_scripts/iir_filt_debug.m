clear all;close all;clc

len = 256;

Fs = 44100;
freq_pass = 20;
freq_stop = 60;
band_stop_rej_db = 40;
freq_pass2 = 2;
freq_stop2 = 5;
band_stop_rej_db2 = 40;

 IIR_pow_filt2 = Iir_butterwoth_generator(freq_pass2/(Fs/2),freq_stop2/(Fs/2),1,band_stop_rej_db2);
 IIR_pow_filt2.PersistentMemory=true;

bbb_test=IIR_pow_filt2.sosMatrix(1,1:3)*IIR_pow_filt2.ScaleValues(1);
aaa_test=IIR_pow_filt2.sosMatrix(1,4:6);
bbb_test2=IIR_pow_filt2.sosMatrix(2,1:3)*IIR_pow_filt2.ScaleValues(2);
aaa_test2=IIR_pow_filt2.sosMatrix(2,4:6);
bbb_test3=IIR_pow_filt2.sosMatrix(3,1:3)*IIR_pow_filt2.ScaleValues(3);
aaa_test3=IIR_pow_filt2.sosMatrix(3,4:6);

signal=10*randn(1,len);
out1 = zeros(1,len);
out2 = zeros(1,len);
temp1 = zeros(1,len);
temp2 = zeros(1,len);
state1= zeros(1,3-1);
state2= zeros(1,3-1);
state3= zeros(1,3-1);

for i=1:2

idx = (i-1)*len/2+1:i*len/2;

out1(idx) = filter(IIR_pow_filt2,signal(idx));


  [temp1(idx),state1]=filter_DII_T(bbb_test,aaa_test,signal(idx),state1);
      [temp2(idx),state2]=filter_DII_T(bbb_test2,aaa_test2,temp1(idx),state2);
      [out2(idx),state3]=filter_DII_T(bbb_test3,aaa_test3,temp2(idx),state3);
      
     
      
     
end    

 figure
      hold on;
      plot(out2,'+r')
      plot(out1,'ob');
      hold off;