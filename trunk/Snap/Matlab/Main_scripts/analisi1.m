clear all; close all;clc

addpath ../kitchen_test/

n_negative_samples = 25;
n_positive_samples = 28;
tot_samples = n_negative_samples + n_positive_samples;
max_data_len = 2^12;

Audio_info_T = struct('Expected_out',blanks(3),'Filename',blanks(64),'Samples',zeros(max_data_len,2),...
    'Fs',0,'Nbits',0,'N_samples',0);

Audio_data=struct('Test_case',Audio_info_T);
k=0;
Fsamp=44100;

for i=1:max(n_negative_samples,n_positive_samples);

idx=sprintf('%02d',i);
pos_str = ['pos',idx,'.wav'];
neg_str = ['neg',idx,'.wav'];

if exist(pos_str,'file') 
    
    k=k+1;
    Audio_data.Test_case(k).Expected_out  = 'pos';
     Audio_data.Test_case(k).Filename  = pos_str;
    
    [ Audio_data.Test_case(k).Samples, Audio_data.Test_case(i).Nbits]=audioread(pos_str);
    Audio_data.Test_case(k).N_samples=length( Audio_data.Test_case(k).Samples);
     Audio_data.Test_case(k).Fs=Fsamp;
else
    
     warning(['File ',pos_str,' not found at index']);
    
    
end
    
if exist(neg_str,'file') 
    
    k=k+1;
    Audio_data.Test_case(k).Expected_out  = 'neg';
     Audio_data.Test_case(k).Filename  = neg_str;
    
    [ Audio_data.Test_case(k).Samples, Audio_data.Test_case(i).Nbits]=audioread(neg_str);
      Audio_data.Test_case(k).N_samples=length( Audio_data.Test_case(k).Samples);
     Audio_data.Test_case(k).Fs=Fsamp;
    
else 
    
    warning(['File ',neg_str,' not found at index']);


end

end

view='neg';

% lp=fir1(512,0.05);
Hd = lp_iir(Fsamp,50,200,1,80);
for i =1:k
    
    if strcmp(Audio_data.Test_case(i).Expected_out,view);
        
       
        f_samp = Audio_data.Test_case(i).Fs;
         winn =2^(nextpow2( 0.03*f_samp));
        t_step = 1/f_samp;
        n_samp = Audio_data.Test_case(i).N_samples;
        t_axe = 0:t_step:(n_samp-1)*t_step;
        f_step=Audio_data.Test_case(i).Fs/(n_samp-1);
        f_axe = 0:f_step:f_samp/2;
    
        figure('Name',Audio_data.Test_case(i).Filename);
        subplot(2,2,1)
        plot( Audio_data.Test_case(i).Samples);
        title('Raw');
        subplot(2,2,2)
%         spectrogram(Audio_data.Test_case(i).Samples,winn,winn/4);
%         plot(10*log10(abs(dd)));
%          surf(t_axe,f_axe,10*log10(abs(dd)),'EdgeColor','none');  
        title('Raw LP');
        subplot(2,2,3)
        dspp=teager_kaiser(Audio_data.Test_case(i).Samples);
        plot( dspp);
         ttt=sprintf( 'Max Val = %f',max(dspp));
        title(['TG ',ttt]);
        subplot(2,2,4)
        dspp2=xrms(dspp,0.05);
%         spectrogram(dspp,winn,winn/4);
plot(dspp2);
%         plot(10*log10(abs(dd2)));
%          surf(t_axe,f_axe,10*log10(abs(dd2)),'EdgeColor','none');  
        title('TG LP');
    
    end
    
end

