function clean_event_abs_fft=denoiser (act_frame,back_frame,len_fft,n_events)

back_fft=zeros(len_fft,n_events);
act_fft=zeros(len_fft,n_events);
% ff_fft=zeros(len_fft,n_events);
back_spec=zeros(len_fft,n_events);
act_spec=zeros(len_fft,n_events);
% ff_spec=zeros(len_fft,n_events);
clean_event_time=zeros(len_fft,n_events);
clean_event_abs_fft=zeros(len_fft,n_events);



for i=1:n_events

back_fft(:,i)=fft(back_frame(i,1:len_fft));
act_fft(:,i)=fft(act_frame(i,1:len_fft));
% ff_fft(:,i)=fft(ff_frame(i,1:len_fft));
% phase_act_fft = angle(act_fft(:,i));

difff=abs(act_fft(:,i))-abs(back_fft(:,i));
clean_event_abs_fft(:,i) = difff.*( (sign(difff)+1)/2);
% clean_event_time(:,i) = (clean_event_abs_fft(:,i).*exp((1i)*phase_act_fft));

figure
hold on;
plot(abs(back_fft(:,i)),'r');
plot(abs(act_fft(:,i)),'b');
hold off;
figure
plot(clean_event_abs_fft(:,i));

 back_spec(:,i)=real(back_fft(:,i)).^2+imag(back_fft(:,i)).^2;
% act_spec(:,i)=real(act_fft(:,i)).^2+imag(act_fft(:,i)).^2;
% ff_spec(:,i)=real(ff_fft(:,i)).^2+imag(ff_fft(:,i)).^2;

% mean_spec(:,i) = (back_spec(:,i)+ff_spec(:,i))/2;



end