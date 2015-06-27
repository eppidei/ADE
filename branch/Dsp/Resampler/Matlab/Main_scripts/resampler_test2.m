clear all;close all;clc

addpath ../Alg_funcs

Filter_phase_len=22;
Fs_in=44100;
Fs_out=11100;
Fsig=1e3;
[downfact,upfact]=rat(Fs_in/Fs_out);
input_len=500;


input=sin(2*pi*Fsig/Fs_in*(1:input_len)); 

time_in=0:1/Fs_in:(1/Fs_in)*(input_len-1);


Filter_len=upfact*Filter_phase_len;

h=fir1(Filter_len-1,1/upfact)*upfact;
exact_delay=length(h)/2/downfact;
useful_len=ceil(input_len*upfact/downfact);

%%matlab function upfirdn integer group delay equalization
out_ufd=upfirdn(input,h,upfact,downfact);
time_ufd=0:1/Fs_out:(1/Fs_out)*(length(out_ufd)-1);


delay_comp_int=fix(exact_delay);
delay_comp_frac=mod(exact_delay,delay_comp_int);
first_useful_sample2=delay_comp_int+1;
out_ufd_comp2=out_ufd(first_useful_sample2:first_useful_sample2+useful_len);
lin_coeffs=[delay_comp_frac,1-delay_comp_frac];
out_ufd_comp3=filter(lin_coeffs,1,out_ufd_comp2);
out_ufd_comp_filt=out_ufd_comp3(2:end);

time_ufd_comp=time_ufd(1:useful_len);



PolyMatrix=zeros(upfact,Filter_phase_len);

for i=1:upfact
    
    PolyMatrix(i,:)=h(i:upfact:Filter_len);
end

out_custom=zeros(upfact,useful_len);
out_custom2=zeros(1,useful_len);
state=zeros(upfact,Filter_phase_len);
 for k=1:upfact
 [out_sample,state(k,:)]=fir_filtering(input(1),PolyMatrix(k,:),state(k,:));
 out_custom(k,1)=out_sample;
 end
 
 out_custom2(1)=out_custom(1,i);


for i=2:useful_len

 [modulo_idx,upsampled_total_idx,block_idx]=get_res_indexes(upfact,downfact,0);
 
 for k=1:upfact
 
     [out_sample,state(k,:)]=fir_filtering(input(i),PolyMatrix(k,:),state(k,:));
      out_custom(k,i)=out_sample;
 end
 
 
 
 
out_custom2(i)=out_custom(modulo_idx,i);
 
end



figure('Name','Output Resample');
plot(time_in,input,'b',time_ufd_comp,out_ufd_comp_filt,'ok',time_ufd_comp,out_custom2,'c');
legend('input','upfirdn comp frac lin');