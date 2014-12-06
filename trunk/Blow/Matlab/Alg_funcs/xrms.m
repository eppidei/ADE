function out=xrms(input,attack_time,release_time,Fs)

local_peak=(input(1))^2;
len=length(input);
at = 1-exp(-2.2/(Fs*attack_time));
rt = 1-exp(-2.2/(Fs*release_time));
out=zeros(1,len);

out(1)=local_peak;

for i=2:len
    
    a=(input(i)).^2;
    
    if a > local_peak
        
        coeff = at;
        
    else
        
        coeff = rt;
    end
    
    local_peak = (1-coeff)*local_peak+coeff*a;
      
    out(i)=local_peak;

end