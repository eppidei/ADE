function  y = expander(input,exp_thresh,exp_slope)



tav = 0.01;
at=0.03;
rt=0.003;
delay = 150;


xrms = 0;
g=1;
buffer = zeros(1,delay);

len =  length(input);

y = zeros(1,len);

for i=1:len
    
    xrms=(1-tav)*xrms+tav*input(i)^2;
    X = 10*log10(xrms);
    
    G = min(0,exp_slope*(exp_thresh-X));
    f = 10^G/20;
    if f < g
        
        coeff = at;
        
    else
        coeff=rt;
    end
    
    g=(1-coeff)*g+coeff*f;
    y(i)=g*buffer(end);
    
    buffer = [input(i) buffer(1:end-1)];
    
end


end

