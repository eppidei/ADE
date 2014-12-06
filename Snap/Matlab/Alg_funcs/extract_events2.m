function  [samples,samples_back,samp_len]  = extract_events2( total_recording,old_recording,main_idx,time_left,time_right,Fs ,max_len)

n_indx=length(main_idx);
n_max_samples = max_len*Fs;
samples=zeros(n_indx,n_max_samples);
samples_back=zeros(n_indx,n_max_samples);
% samples_ff=zeros(n_indx,n_max_samples);
% sample_left=zeros(1,n_idx);
% sample_right=zeros(1,n_idx);
samp_len=zeros(n_indx,1);
len_rec = length(total_recording);
len_aug = 2*len_rec;

augmented_frame = [old_recording,total_recording];

for i=1:n_indx
    
    sample_left = fix(len_rec+main_idx(i)-time_left*Fs);
    sample_right = ceil(len_rec+main_idx(i)+time_right*Fs);
    if sample_left<1
        sample_left=1;
    end
    if sample_right>len_aug
        sample_right=len_aug;
    end
    extract_len=sample_right-sample_left+1;
    samp_len(:,i)=extract_len;
    samples(i,1:extract_len)=augmented_frame(sample_left:sample_right);
    
    sample_left_back = sample_left-fix(extract_len);
    sample_right_back = sample_right-fix(extract_len);
    if sample_right_back<1
        sample_right_back=1;
    end
    if sample_left_back<1
        sample_left_back=1;
    end
    winddd = window('hamming',extract_len);
    samples_back(i,1:extract_len)=augmented_frame(sample_left_back:sample_right_back).*winddd';
    
    %%%%%
    
%     sample_left_ff = sample_left+extract_len;
%     sample_right_ff = sample_right+extract_len;
%     if sample_right_ff<len_aug || sample_left_ffk<len_aug
%     
%         samples_ff(i,1:extract_len)=augmented_frame(sample_left_ff:sample_right_ff);
%     else
%         samples_ff(i,1:extract_len)=samples_back(i,1:extract_len);
%     end
    
end


end

