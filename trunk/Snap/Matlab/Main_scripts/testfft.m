clear all;close all;clc
% 
% fid = fopen('impulsefft.txt','r');
% 
% 
% A=fscanf(fid,'%f');
% 
% fclose(fid);
% 
% 
% A2 = fft(1:512);
% A3 = (real(A2).^2+imag(A2).^2);
% A4 = abs((A2).^2);
% 
% 
% figure
% hold on;
%  plot(A(100:420),'g');
%  plot(A3(100:420),'ro');
% %  plot(A4(100:420),'k+');
% hold off;

fid = fopen('intsamplescla.txt','r');


samp=fscanf(fid,'%f');

fclose(fid);

fid = fopen('fftcla.txt','r');


ff=fscanf(fid,'%f');

fclose(fid);


fid = fopen('xrmscla.txt','r');


xrms=fscanf(fid,'%f');

fclose(fid);


sampflo = samp.*(2^-15);
fax = 0 : 44100/511 : 44100;

freq_step=22050/(512-1);
sx_bin=round(1500/freq_step)+1;
dx_bin=round(3000/freq_step)+1;

tot_pow = sum(ff(1:256))
sel_pow =sum((ff(sx_bin:dx_bin)))

figure
plot(sampflo);
figure
plot(fax,ff)
figure
plot(xrms)



