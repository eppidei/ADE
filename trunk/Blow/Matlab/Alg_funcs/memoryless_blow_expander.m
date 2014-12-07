function y = memoryless_blow_expander(inp_frame,fitresult)

if strcmp(fitresult.p.form,'pp')
%%piecewise construction test

x_breaks = fitresult.p.breaks;

coeffs = fitresult.p.coefs;
n_pieces = fitresult.p.pieces;

 y = zeros(1,length(inp_frame));
 persistent data_saved
 
 if isempty(data_saved)
     
     data_saved=0;
     
 end
     
 
for i=1:length(inp_frame)
    
    data = inp_frame(i);
    
    if data>=0
 
    for k=1: n_pieces
        
        if k==1
            
            if  (data>=x_breaks(k)) && (data<x_breaks(k+1))
                zz=k;
                break;
 
            end
            
        elseif k==n_pieces
            
                if (data>=x_breaks(k)) && (data<=x_breaks(k+1))
                     zz=k;
                 break;
                end
            else
            
                if (data>=x_breaks(k)) && (data<x_breaks(k+1))
                     zz=k;
                     break;
                 end
        end
    end
    
    else
        
       
        if (data<data_saved)
            data_saved=data;
             warning('input data negative %f\n',data_saved);
        end
        zz=1;
        data = 0;
    
    end
      y(i)=coeffs(zz,4)+coeffs(zz,3)*(data-x_breaks(zz))+coeffs(zz,2)*(data-x_breaks(zz))^2+coeffs(zz,1)*(data-x_breaks(zz))^3;       
    
 end
    



else
    
   error('algorithm %s not implemented',fitresult.form); 
    
end


end