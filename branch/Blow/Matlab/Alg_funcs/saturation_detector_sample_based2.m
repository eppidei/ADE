function  [nstate,eval_counter,eval_pow,eval_timer] =saturation_detector_sample_based2(pstate,sample,running_pow_fast,running_pow_slow,pow_thresh_high,pow_thresh_low,sat_thresh,n_sat_thres,n_pow_thres_attack,n_pow_thres_release,eval_time_init,...
    eval_counter_NS,eval_pow_NS,eval_timer_NS)
%#codegen

% persistent eval_counter_NS;
% persistent eval_pow_NS;
% persistent eval_timer_NS;
% persistent frame_n_latch;
% persistent win_counter;

% State_waiting.Name='waiting'; %1
State_waiting=1;
% State_waiting.Id  = 1;
% State_evaluation.Name='evaluation';%2
% State_evaluation.Id  = 2;
State_evaluation=2;
% State_trackin.Name='trackin';%3
% State_trackin.Id  = 3;
State_trackin=3;


% if isempty(eval_counter_NS)
%     eval_counter_NS=0;
% end
% 
% if isempty(eval_timer_NS)
%     eval_timer_NS=0;
% end
% 
% if isempty(eval_pow_NS)
%     eval_pow_NS=0;
% end

% if isempty(win_counter)
%     win_counter=0;
% end

% running_pow_len = frame_n_latch*frame_len+sample_idx;

% switch pstate.Idpow_acc

% for sample_idx=1:length(frame)

    if pstate==State_trackin
        

        if running_pow_slow<=pow_thresh_low
            eval_pow=eval_pow_NS+1;
        else
            eval_pow=eval_pow_NS;
        end

       if eval_pow>n_pow_thres_release
            nstate=State_waiting;
        else
             nstate=pstate;
       end
        
       eval_counter=0;
       eval_timer=0;
        
         
%         if sample_idx==frame_len
%             frame_n_latch=frame_n_latch+1;
%         end
    
    elseif pstate==State_evaluation
        eval_timer=eval_timer_NS-1;

       
        if abs(sample)>sat_thresh
            eval_counter=eval_counter_NS+1;
         else
             eval_counter=eval_counter_NS;
        end
        if running_pow_fast>=pow_thresh_high
            eval_pow=eval_pow_NS+1;
        else
            eval_pow=eval_pow_NS;
        end
%         running_pow_fast=pow_acc/win_counter;

        if eval_counter>n_sat_thres && eval_timer>=0 && eval_pow>=n_pow_thres_attack
            
            nstate=State_trackin;
            eval_pow=0;
%              running_pow_fast=(running_pow_fast*(win_counter-1)+frame)/win_counter;
%             running_pow_fast=running_pow_fast/sample_idx;

        elseif eval_timer==0

            nstate=State_waiting;
            
        else
            nstate=State_evaluation;
        end
        
%         if sample_idx==frame_len
%             frame_n_latch=frame_n_latch+1;
%         end
    
    
    elseif pstate==State_waiting
%         frame_n_latch=0;
        
        if abs(sample)>sat_thresh

            eval_timer=eval_time_init;
            eval_counter=0;
            eval_pow= 0;
%              win_counter=0;
%             pow_acc=0;
            nstate=State_evaluation;
        else
            eval_timer=eval_timer_NS;
             eval_counter= eval_counter_NS;
             eval_pow=0;
            nstate=State_waiting;

        end
        
    else
        
%         pstate
        error('State Unknown');
% end
    
    
    end
    
%     eval_counter=eval_counter_NS;
% eval_pow=eval_pow_NS;
% eval_timer=eval_timer_NS;
    
end


% end

