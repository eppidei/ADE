Fs=22050;
frame_time_len = 300e-3;
frame_len = round(frame_time_len*Fs)-1;
freq_left=1800;
freq_right=3200;
spectral_threshold_schiocco  = 0.2;
thresh_gain = 7;
thresh_bias = 2e-2;
at=1e-4;
rt=50e-3;
time_left=0.5e-3;
time_right=6e-3;
samp_range_search_time = 80e-3;
samp_range_search = ceil(samp_range_search_time*Fs)-1;
max_range  = [2000,3000];
search_step = 3;
look_ahead_step = 3;