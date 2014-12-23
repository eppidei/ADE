function out=unit_test_2 (IIR_test,input_vector,frame_len) 

in_len = length(input_vector);
n_iterations=floor(in_len/frame_len);
out=zeros(1,in_len);

for i=1:n_iterations
    
    indexes = (i-1)*frame_len+1:i*frame_len;
    frame=input_vector(indexes );


        out(indexes) = filter(IIR_test,frame);
  
end

end