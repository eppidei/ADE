function estimate = pow_est(data,n_slots)
%#codegen

% n_slots = 2;
len = length(data);
j=0;
% parzial_sum=single(0);
len_slot=fix(len/n_slots);
estimate = zeros(len,1);

while j<n_slots
%     for i=1:len_slot
%         parzial_sum = parzial_sum + data(j*len_slot+i)^2;
%     end
    idxs = j*len_slot+1:(j+1)*len_slot;
    parzial_sum=data(idxs)'*data(idxs);
    estimate(idxs) = parzial_sum/len_slot;
%     parzial_sum=single(0);
    j=j+1;   
end
