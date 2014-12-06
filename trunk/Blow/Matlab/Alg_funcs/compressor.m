function  [xrms,y] = compressor(input,comp_thresh,comp_slope)



tav = 0.01;
at=0.03;
rt=0.003;
delay = 150;



g=1;
buffer = zeros(1,delay);

len =  length(input);

y = zeros(1,len);
xrms= zeros(1,len);

for i=1:len
    
    xrms(i)=(1-tav)*xrms(i)+tav*input(i)^2;
    X = 10*log10(xrms(i));
    
    G = min(0,comp_slope*(comp_thresh-X));

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

