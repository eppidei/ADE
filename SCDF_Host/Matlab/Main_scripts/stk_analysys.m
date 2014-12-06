clear all;close all;clc

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

mex_exec_path = [up_dir,'Mex',separator,'exec'];

addpath(mex_exec_path)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

frame_len = 262144;
blow_state = zeros(1,frame_len);%[zeros(1,frame_len/4),ones(1,frame_len/2),zeros(1,frame_len/4)];
blow_amp = 1;
blow_rate = 0.8;
note_state = [zeros(1,frame_len/4),ones(1,frame_len/2),zeros(1,frame_len/4)];
note_on_freq = 440;
note_on_amp = 0.9;
cg_state = blow_state;
cg_num = 128;
cg_freq = 0.5;
cg_val = 40*sin(2*pi*mod(cg_freq/44100*(1:frame_len),1))+80;

figure('Name','Breath pressure')
plot(cg_val);


data_out = stk_debug(blow_state,blow_amp,blow_rate,note_state,note_on_freq,note_on_amp,cg_state,cg_num,cg_val,frame_len);

data_2_plot=data_out(1:frame_len);

figure('Name','Stk out');
hold on;
plot(data_2_plot);
plot(note_state);
hold off;

p=audioplayer(data_2_plot,44100);
play(p);
