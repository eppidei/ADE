function sat_str=saturation_detector_frame(sat_str)



for j=1:sat_str.frame_len
        
%         idxxx = (k-1)*Frame_len+j;
        
        [sat_str.state,sat_str.eval_counter,sat_str.eval_pow,sat_str.eval_timer] =saturation_detector_sample_based2(sat_str.state,sat_str.audio_frame(j),sat_str.run_pow_fast(j),...
            sat_str.run_pow_slow(j),sat_str.pow_thresh_high,sat_str.pow_thresh_low,sat_str.sat_thresh,sat_str.n_sat_thres,...
            sat_str.n_pow_thres_attack,sat_str.n_pow_thres_release,sat_str.eval_time_samples,sat_str.eval_counter,sat_str.eval_pow,sat_str.eval_timer);
   
        
         if sat_str.state==3
           
             sat_str.blow(j)=1;
         end


end