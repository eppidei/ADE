clear all; close all;clc

Fs=22050;
frame_audio_len=2048;
thres=0.4;
buff_size=Fs*500e-3;
at=1e-4;
rt=50e-3;
samp_range_search=100;
time_left=2e-3;
time_right=20e-3;
detect_pow=0.3;
