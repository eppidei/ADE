clear all;close all;clc

Fs = 16000;
Frame_len = 512;

file_path = ['../../dump_test_',num2str(Frame_len),'_',num2str(Fs),'_ath.txt'];


Frame_time = Frame_len/Fs;

fid=fopen(file_path,'r');
data =zeros(1,10e3);
i=1;
while ~feof(fid)
    
   line=fgetl(fid) ;
       
    [token,remain]=strtok(line,' ');
    [token2,remain2]=strtok(remain,' ');
    data(i)=str2double(token2);
    i=i+1;
end


fclose(fid);

n_data = i-1;

data2=data(1:n_data);

istantaneous_frame_time = diff(data2);
total_mean = mean(istantaneous_frame_time);


figure('Name',['Time arrival interval Frame ',num2str(Frame_len),' Fs ',num2str(Fs)]);
plot((1:n_data-1),istantaneous_frame_time,'ob',(1:n_data-1),Frame_time*ones(1,n_data-1),...
    'r',(1:n_data-1),total_mean*ones(1,n_data-1),'g');
ylim([0 2*Frame_time])
legend('Istantaneous Frame time','teoretical','global mean');

running_mean = zeros(1,n_data-1);
for i=1:n_data-1

running_sum = sum(istantaneous_frame_time(1:i));
running_mean(i)=running_sum/i;

end

figure('Name',['expected mean and running mean Frame ',num2str(Frame_len),' Fs ',num2str(Fs)]);
plot((1:n_data-1),running_mean,'b',(1:n_data-1),Frame_time*ones(1,n_data-1),'r',(1:n_data-1),total_mean*ones(1,n_data-1),'g');
ylim([0 2*Frame_time])
legend('running mean','teoretical','global mean');