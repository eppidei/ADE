function y = memoryless_blow_expander_matrix(inp_frame,fitresult)

if strcmp(fitresult.p.form,'pp')
%%piecewise construction test

x_breaks = fitresult.p.breaks;

coeffs = fliplr(fitresult.p.coefs);
n_pieces = fitresult.p.pieces;

 y = zeros(1,length(inp_frame));
 persistent data_saved
 
 if isempty(data_saved)
     
     data_saved=0;
     
 end
 
order_plus_one = size(coeffs,2);
 

 coeffs_expanded = zeros(length(inp_frame),order_plus_one);
 data_min_breaks = zeros(order_plus_one,length(inp_frame));
     
 
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
            % warning('input data negative %f\n',data_saved);
        end
        zz=1;
        data = 0;
    
    end
          
    coeffs_expanded(i,:)=coeffs(zz,:);
    data_min_breaks(1,i)=1;
    data_min_breaks(2,i)=(data-x_breaks(zz));
    data_min_breaks(3,i)=(data-x_breaks(zz))^2;
    data_min_breaks(4,i)=(data-x_breaks(zz))^3;
 end
    


%  y(i)=coeffs_expanded(i,:)*data_min_breaks(:,i);


else
    
   error('algorithm %s not implemented',fitresult.form); 
    
end


 y=diag(coeffs_expanded*data_min_breaks);
end