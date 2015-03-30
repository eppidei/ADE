clear all;close all;clc

addpath ../Alg_funcs


Filter_phase_len=22;
Fs_in=44100;
Fs_out=11100;
Fsig=1e3;
[downfact,upfact]=rat(Fs_in/Fs_out);
input_len=500;


input=sin(2*pi*Fsig/Fs_in*(1:input_len)); 
useful_len=ceil(input_len*upfact/downfact);

time_in=0:1/Fs_in:(1/Fs_in)*(input_len-1);


Filter_len=upfact*Filter_phase_len;

h=fir1(Filter_len-1,1/upfact)*upfact;

%classic

sig_up=upsample(input,upfact);
filt_out=filter(h,1,sig_up);
out_ped=downsample(filt_out,downfact);


%polyphase
PolyMatrix=zeros(upfact,Filter_phase_len);
out_poly=zeros(upfact,input_len);

for i=1:upfact
    
    PolyMatrix(i,:)=h(i:upfact:Filter_len);
    out_poly(i,:)=filter(PolyMatrix(i,:),1,input);
end

out_poly2=downsample(reshape(out_poly,1,upfact*input_len),downfact);

%polycustom

out_custom2=zeros(1,useful_len);
state=zeros(upfact,Filter_phase_len);
out_custom=zeros(upfact,input_len);

for i=1:input_len

 for k=1:upfact
 
     [out_sample,state(k,:)]=fir_filtering(input(i),PolyMatrix(k,:),state(k,:));
      out_custom(k,i)=out_sample;
 end
 
 if (i==1)
     
     modulo_idx=1;
     upsampled_total_idx=1;
     block_idx=1;
     out_custom2(1)=out_custom(modulo_idx,block_idx);
     j=2;
    
     
 else
     
     if (i==block_idx)
        out_custom2(j)=out_custom(modulo_idx,block_idx);
        j=j+1;
     elseif (i>block_idx)
 
        [modulo_idx,upsampled_total_idx,block_idx]=get_res_indexes(upfact,downfact,0);
        
        
     end
 
 end
 
 if (block_idx>input_len)
     i
    break
 end

end
figure
hold on;
plot(out_poly2,'b');
 plot(out_custom2,'r');
 hold off;






