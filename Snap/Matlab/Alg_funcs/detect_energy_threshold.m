function [indexes,out_tg]=detect_energy_threshold(data,thresh)


% Audio_info_T = struct('Expected_out',blanks(3),'Filename',blanks(64),'Samples',zeros(max_data_len,2),...
%     'Fs',0,'Nbits',0,'N_samples',0);
% 
% Audio_data=struct('Test_case',Audio_info_T);

% len = length(data);

out_tg=teager_kaiser(data);

indexes=find(out_tg>thresh);

end

