clear all; close all;clc

addpath /home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/Snap/Matlab/Main_scripts/

sim_mat_common_params

nbit = 16;
% Fs=22050;
% frame_audio_len=2048;

% frame_time_len = 300e-3;
% frame_len = round(frame_time_len*Fs)-1;

buff_size=frame_len;

% freq_left=2000;
% freq_right=3000;

n_slots = 3;
% spectral_threshold_schiocco  = 0.2;
%     thresh_gain = 7;
%     thresh_bias = 2e-1;

% mult_fact_thresh  = 100;
% 
% at=1e-4;
% rt=50e-3;
% time_left=0.5e-3;
% time_right=7e-3;

% max_range  = [2000,3000];

% search_step = 3;
% look_ahead_step = 3;


% samp_range_search_time = 80e-3;
% samp_range_search = ceil(samp_range_search_time*Fs)-1;

