function  [samples,samp_len]  = extract_events( total_recording,main_idx,time_left,time_right,Fs ,max_len)

n_indx=length(main_idx);
n_max_samples = max_len*Fs;
samples=zeros(n_indx,n_max_samples);
% sample_left=zeros(1,n_idx);
% sample_right=zeros(1,n_idx);
samp_len=zeros(n_indx,1);

for i=1:n_indx
    
    sample_left = fix(main_idx(i)-time_left*Fs);
    sample_right = ceil(main_idx(i)+time_right*Fs);
    if sample_left<1
        sample_left=1;
    end
    if sample_right>length(total_recording)
        sample_right=length(total_recording);
    end
    extract_len=sample_right-sample_left+1;
    samp_len(:,i)=extract_len;
    samples(i,1:extract_len)=total_recording(sample_left:sample_right);
    
end


end

