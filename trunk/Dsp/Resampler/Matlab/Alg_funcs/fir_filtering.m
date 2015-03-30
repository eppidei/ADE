function [out_sample,state]=fir_filtering(in_sample,b,state)


order=length(b)-1;

if (length(state)~=(order+1))
   
    error('length state %d order %d\n',length(state),order);
    
end



        out_sample = b(1)*in_sample+state(1);



        for i=1:order% lostate deve essere inizializzato a 0 e lungo pari a order+1 (es. biquad ordine 2)
        
            state(i)=b(i+1)*in_sample+state(i+1);
        end

    

