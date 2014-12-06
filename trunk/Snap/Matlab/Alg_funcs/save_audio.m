function save_audio(samples,sample_id,samp_len,folder_str,Fs)

filename=sprintf('./%s/Event num%s.wav',folder_str,num2str(sample_id));
audiowrite(filename,samples(1:samp_len),Fs);



end