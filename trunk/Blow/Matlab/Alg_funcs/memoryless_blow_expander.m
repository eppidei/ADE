function y = memoryless_blow_expander(inp,type,gain)

if max(inp)>1 || min(inp)<-1
    
    error('normalize input to +/-1');
    
end

len = length(inp);


if strcmp(type,'quad')
    
y=gain.*inp.^2;

elseif strcmp(type,'cub')
    
y=gain.*inp.^3;

elseif strcmp(type,'leo1')
   y=zeros(1,len) ;
    for i=1:len
        
        if (inp(i)<=0.253)
            
            y(i)=inp(i);
            
        else
            
            y(i)=(2.5*inp(i))^3;
            
        end
        
    end
    

else
    
    error('unknown type');
    
end



end