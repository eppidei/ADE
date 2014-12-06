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
max_amp = 100;
amp_gain=1;%0:max_amp/inp_len:max_amp-1/inp_len;
input = amp_gain.*sin(2*pi*mod(Fnorm*(1:inp_len),1));

[xrms_o,outp] = compressor(input,-50,1);

figure('Name','comp_o')
plot((1:inp_len),xrms_o,'b',(1:inp_len),input,'r');
