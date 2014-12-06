function [nstate]=saturation_detector(pstate,frame,sample_idx,running_pow_fast,running_pow_slow,pow_thresh_high,pow_thresh_low,amp_thresh,n_sat_thres,n_pow_thres,eval_time_init)


persistent eval_counter;
persistent eval_pow;
persistent eval_timer;
% persistent frame_n_latch;
% persistent win_counter;

State_waiting.Name='waiting';
State_waiting.Id  = 1;
State_evaluation.Name='evaluation';
State_evaluation.Id  = 2;
State_trackin.Name='trackin';
State_trackin.Id  = 3;


if isempty(eval_counter)
    eval_counter=0;
end

if isempty(eval_timer)
    eval_timer=0;
end

if isempty(eval_pow)
end

% if isempty(win_counter)
%     win_counter=0;
% end

% running_pow_len = frame_n_latch*frame_len+sample_idx;

% switch pstate.Idpow_acc

    if pstate.Id==State_trackin.Id
        
%        if win_counter==(window_len+1)
%            win_counter=window_len;
%        else
%            win_counter=win_counter+1;
%        end
        
%         eval_counter=eval_counter;
        
%         running_pow_fast=(running_pow_fast*(running_pow_len-1)+frame(sample_idx))/running_pow_len;
% if (sample_idx-win_counter)<1
%     
%     erase_idx = mod(sample_idx-win_counter);
%     pow_acc=pow_acc+frame(sample_idx)^2-old_frame(erase_idx)^2;
%     
% else
%     erase_idx = sample_idx-win_counter;
%     pow_acc=pow_acc+frame(sample_idx)^2-frame(erase_idx)^2;
% end
%     
%     running_pow_fast=pow_acc/win_counter;
    
%         if running_pow_fast>=pow_thresh_high

%                 nstate=pstate;

if running_pow_slow<=pow_thresh_low
    eval_pow=eval_pow+1;
end

       if eval_pow>n_pow_thres
            nstate=State_waiting;
        else
             nstate=pstate;
        end
        
         
%         if sample_idx==frame_len
%             frame_n_latch=frame_n_latch+1;
%         end
    
    elseif pstate.Id==State_evaluation.Id
        eval_timer=eval_timer-1;
%         if win_counter==(window_len+1)
%             win_counter=window_len;
%             if (sample_idx-window_len)<1
%     
%                 erase_idx = mod(sample_idx-window_len);
%                 pow_acc=pow_acc+frame(sample_idx)^2-old_frame(erase_idx)^2;
%     
%             else
%                 erase_idx = sample_idx-window_len;
%                 pow_acc=pow_acc+frame(sample_idx)^2-frame(erase_idx)^2;
%             end
% %             pow_acc=pow_acc+frame(sample_idx)^2-frame(sample_idx-window_len)^2;
%         else
%             win_counter=win_counter+1;
%              pow_acc=pow_acc+frame(sample_idx)^2;
%         end
       
        if abs(frame(sample_idx))>amp_thresh
            eval_counter=eval_counter+1;
%         else
%             eval_counter=eval_counter;
        end
        if running_pow_fast>=pow_thresh_high
            eval_pow=eval_pow+1;
        end
%         running_pow_fast=pow_acc/win_counter;

        if eval_counter>n_sat_thres && eval_timer>=0 && eval_pow>=n_pow_thres
            
            nstate=State_trackin;
            eval_pow=0;
%              running_pow_fast=(running_pow_fast*(win_counter-1)+frame(sample_idx))/win_counter;
%             running_pow_fast=running_pow_fast/sample_idx;

        elseif eval_timer==0

            nstate=State_waiting;
            
        else
            nstate=State_evaluation;
        end
        
%         if sample_idx==frame_len
%             frame_n_latch=frame_n_latch+1;
%         end
    
    
    elseif pstate.Id==State_waiting.Id
%         frame_n_latch=0;
        
        if frame(sample_idx)>amp_thresh

            eval_timer=eval_time_init;
            eval_counter=0;
            eval_pow= 0;
%              win_counter=0;
%             pow_acc=0;
            nstate=State_evaluation;
        else

            nstate=State_waiting;

        end
        
    else
        
        pstate
        error('State Unknown');
% end
    
    
    end
    
end


