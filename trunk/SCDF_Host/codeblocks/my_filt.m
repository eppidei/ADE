function [out,state]=my_filt(in,a,order,b,state,len_frame)

out=zeros(1,len_frame);

for k=1:len_frame

    out(k) = b(1)*in(k)+state(1);

        for i=1:order
       
            state(i)=b(i+1)*in(k)+state(i+1)-a(i+1)*out(k);
    
        end
        
end

end