function [spectrum,percent_pow,band_peak,vara,balance ]= snap_recognition2(events_got,n_events,Fs,len_fft,frq_sx_band,frq_dx_band,max_events)


% frq_sx=1500;
% frq_dx=3000;

% len_fft=512;


freq_step=Fs/(len_fft-1);
% sx_bin_b1=round(frq_sx_band1/freq_step)+1;
% dx_bin_b1=round(frq_dx_band1/freq_step)+1;
sx_bin_b2=round(frq_sx_band/freq_step)+1;
dx_bin_b2=round(frq_dx_band/freq_step)+1;
% max_events=10;
percent_pow=zeros(2,max_events);
bin_peak=zeros(1,max_events);
band_peak=zeros(1,max_events);
max_peak=zeros(1,max_events);
vara=zeros(1,max_events);
sel_pow2=zeros(1,max_events);
tot_pow=zeros(1,max_events);
balance=zeros(1,max_events);
% fft_axe=0:freq_step:Fs;
bal_range = round(500/freq_step)+1;
used_events = n_events;
if n_events>max_events
    warning('too much events\n');
    n_events
    used_events = max_events;
end
spectrum=zeros(max_events,len_fft);

% if n_events>=1

    for i=1:used_events
            
%          fff = fft(events_got(1:len_fft,i));
        spectrum(i,:) =real(events_got(1:len_fft,i)).^2+imag(events_got(1:len_fft,i));
        spectrum(i,1:3) = 0;
        whole_pow_spec = spectrum(i,1:len_fft/2);
        tot_pow(i) = sum(whole_pow_spec);
%          sel_pow1(i) = sum(spectrum(i,sx_bin_b1:dx_bin_b1));
        sel_pow_spec = spectrum(i,sx_bin_b2:dx_bin_b2);
         vara(i)=var(sel_pow_spec);
        sel_pow2(i) = sum(sel_pow_spec);
%          percent_pow(1,i)=sel_pow1(i)/tot_pow(i);
        percent_pow(2,i)=sel_pow2(i)/tot_pow(i);
        [max_peak(i),bin_peak(i)]=max(sel_pow_spec);
        peak_idx = sx_bin_b2+bin_peak(i);
         band_peak(i)=(peak_idx-1)*freq_step;
         bal_right = peak_idx+bal_range;
         bal_left = peak_idx-bal_range;
         rp = sum(spectrum(i,peak_idx:1:bal_right));
         lp = sum(spectrum(i,peak_idx:-1:bal_left));
         balance(i)= rp/lp;
    end
    
   

% end