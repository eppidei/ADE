clear all;close all;clc

Filter_phase_len=22;
Fs_in=44100;
Fs_out=11100;
Fsig=1e3;
[downfact,upfact]=rat(Fs_in/Fs_out);

input_len=500;

input=sin(2*pi*Fsig/Fs_in*(1:input_len)); 

time_in=0:1/Fs_in:(1/Fs_in)*(input_len-1);


Filter_len=upfact*Filter_phase_len-1;

h=fir1(Filter_len,1/upfact)*upfact;
exact_delay=length(h)/2/downfact;

%%matlab function upfirdn

out_ufd=upfirdn(input,h,upfact,downfact);
filt_del=Filter_phase_len/2;
time_ufd=0:1/Fs_out:(1/Fs_out)*(length(out_ufd)-1);

%%matlab function upfirdn integer group delay equalization

useful_len=ceil(input_len*upfact/downfact);
delay_comp=round(exact_delay);
time_ufd_comp=time_ufd(1:useful_len);
first_useful_sample=delay_comp+1;
out_ufd_comp=out_ufd(first_useful_sample:first_useful_sample+useful_len-1);

%%matlab function upfirdn fractional group delay equalization

delay_comp_int=fix(exact_delay);
delay_comp_frac=mod(exact_delay,delay_comp_int);
first_useful_sample2=delay_comp_int+1;
out_ufd_comp2=out_ufd(first_useful_sample2:first_useful_sample2+useful_len);
lin_coeffs=[delay_comp_frac,1-delay_comp_frac];
out_ufd_comp3=filter(lin_coeffs,1,out_ufd_comp2);
out_ufd_comp_filt=out_ufd_comp3(2:end);
xx=1:useful_len+1;
out_ufd_comp4=spline(xx,out_ufd_comp2,xx+delay_comp_frac);
out_ufd_comp_filt2=out_ufd_comp4(1:end-1);
%%matlab function resample


out_res=resample(input,upfact,downfact,h);
time_res=0:1/Fs_out:(1/Fs_out)*(length(out_res)-1);


% %%matlab function pedantinc
% 
% sig_up=upsample(input,upfact);
% filt_out=filter(h,1,sig_up);
%  out_ped=downsample(filt_out,downfact);
% state=zeros(1,length(h));
% out_ped2=zeros(1,length(filt_out));
% for i=1:length(sig_up)
%     [out_ped2(i),state]=fir_filtering(sig_up(i),h,state);
% end
% time_ped=0:1/Fs_out:(1/Fs_out)*(length(out_ped)-1);


figure('Name','Output Resample');
plot(time_in,input,'b',time_ufd_comp,out_ufd_comp,'om',time_res,out_res,'og',time_ufd_comp,out_ufd_comp_filt,'ok',time_ufd_comp,out_ufd_comp_filt2,'oc');
legend('input','upfirdn comp','resample','upfirdn comp frac lin','upfirdn comp frac spline');

