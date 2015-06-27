function save_audio2(recording,positive_idx,num_pos,negative_idx,num_neg,folder_str,Fs)

time_left = 50e-3;
time_right= 500e-3;
samples_left = fix(time_left*Fs)-1;
samples_right = fix(time_right*Fs)-1;

for i=1:num_pos
filename=sprintf('./%s/Event positive num%s.wav',folder_str,num2str(i));
idx1 = positive_idx(i)-samples_left;
idx2 = positive_idx(i)+samples_right;
if idx1<1
    idx1=1;
end
if idx2 > length(recording)
    idx2=length(recording);
end
audiowrite(filename,recording(idx1:idx2),Fs);
end

for i=1:num_neg
filename=sprintf('./%s/Event negative num%s.wav',folder_str,num2str(i));
idx1 = positive_idx(i)-samples_left;
idx2 = positive_idx(i)+samples_right;
if idx1<1
    idx1=1;
end
if idx2 > length(recording)
    idx2=length(recording);
end
audiowrite(filename,recording(idx1:idx2),Fs);
end


end