function [out,state]=filter_DII_T (b, a,in, state)

 
len_frame=length(in);
order =max(length(a),length(b));

out=zeros(1,len_frame);


    for k=1:len_frame
 


    out(k) = b(1)*(in(k))+state(1);

        for i=1:(order-1)
        
            state(i)=b(i+1)*(in(k))+state(i+1)-a(i+1)*(out(k));
        end
   

    end
        
end