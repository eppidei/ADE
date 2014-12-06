function [bb_fast,bb_slow,sat_thresh,n_sat_thres,eval_time_samples,n_pow_thres_attack,n_pow_thres_release,running_pow_win_fast,running_pow_win_slow]=blow_config2(nbit,Margin,Fs,eval_time,...
    running_pow_win_time_fast,running_pow_win_time_slow,time_pow_thresh_attack,time_pow_thresh_release)



  max_amp = 1-2^(-(nbit-1));

  sat_thresh = (1-Margin)*max_amp;
  



eval_time_samples = round(eval_time*Fs)+1;
n_sat_thres=20;

running_pow_win_fast = round(running_pow_win_time_fast*Fs)+1;

running_pow_win_slow = round(running_pow_win_time_slow*Fs)+1;



n_pow_thres_samples_attack=round(time_pow_thresh_attack*Fs)+1;
n_pow_thres_attack=round(n_pow_thres_samples_attack);

n_pow_thres_samples_release=round(time_pow_thresh_release*Fs)+1;
n_pow_thres_release=round(n_pow_thres_samples_release);


 bb_fast=1/running_pow_win_fast*ones(1,running_pow_win_fast);

 bb_slow=1/running_pow_win_slow*ones(1,running_pow_win_slow);
 
end