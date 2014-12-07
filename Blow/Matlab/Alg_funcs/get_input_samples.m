function [audio_left,audio_right,audioinfos,n_iterations] = get_input_samples (file_path,Frame_len)

[samples,Fs]=audioread(file_path);
audioinfos=audioinfo(file_path);
    if (size(samples,2)==2)
    audio_left = samples(:,1);
    audio_right = samples(:,2);
    elseif (size(samples,2)==1)
        audio_left = samples(:,1);
        audio_right= samples(:,1);
    else
       error('file format not recognized'); 
    end
    
    n_iterations=fix(length(audio_left)/Frame_len);



end