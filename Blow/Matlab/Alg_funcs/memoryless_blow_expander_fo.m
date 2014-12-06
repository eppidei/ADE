function y = memoryless_blow_expander_fo(inp,fit_obj)

if max(inp)>1 || min(inp)<-1
    
    error('normalize input to +/-1');
    
end

len = length(inp);

% 
%    y=zeros(1,len) ;
%     for i=1:len
        
%        y(i)=fit_obj(inp(i));
        y=fit_obj(inp);
%     end
    



end