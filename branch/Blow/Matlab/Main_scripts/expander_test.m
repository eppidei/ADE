clear all; close all;clc

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


funcs_path = [up_dir,'Alg_funcs'];

addpath(funcs_path)



Fs = 44100;
Fsig = 800;
Fnorm = Fs/Fsig;
inp_len = 8192;
amp_gain=0:1/inp_len:1-1/inp_len;
input = amp_gain.*sin(2*pi*mod(Fnorm*(1:inp_len),1));

outp = expander(input,0.8,2);

figure('Name','Exp_o')
plot((1:inp_len),outp,'ob',(1:inp_len),input,'+r');
