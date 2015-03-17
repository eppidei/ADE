function [indexes,vals,n_indexes]=find_local_max(data,samples_range,min_thresh,search_step,look_ahead_step)

len = length(data);
max_indexes = 10;
indexes=zeros(1,max_indexes);
vals=zeros(1,max_indexes);
k=1;

for i = 3: search_step :samples_range
    
    idx_l = i-look_ahead_step:-look_ahead_step:look_ahead_step;
    idx_r = i+look_ahead_step:look_ahead_step:i+samples_range;
    a1 =  data(i)> data(idx_l);
    a2 =  data(i)> data(idx_r);
     b1 =  ones(size(idx_l)) ;
    b =  ones(size(idx_r)) ;
    if ( sum(a1-b1)==0) && (sum(a2-b)==0) && data(i)>min_thresh(i)

        indexes(k)=i;
        vals(k)=data(i);
        k=k+1;
    end
    
    last_idx = i;
    
end

for i = last_idx+search_step : search_step :len -samples_range -search_step
    
    idx_l = i-look_ahead_step:-look_ahead_step:i-samples_range;
    idx_r = i+look_ahead_step:look_ahead_step:i+samples_range;
    a1 =  data(i)> data(idx_l);
    a2 =  data(i)> data(idx_r);
    b =  ones(size(idx_r)) ;
    if (sum(a1-b)==0) && ( sum(a2-b)==0) && data(i)>min_thresh(i)

        indexes(k)=i;
        vals(k)=data(i);
        k=k+1;
    end
last_idx = i;
end

for i = last_idx+search_step: search_step : len
    
    idx_l = i-look_ahead_step:-look_ahead_step:i-samples_range;
    idx_r = i+look_ahead_step:look_ahead_step:len;
    a1 =  data(i)> data(idx_l);
    a2 =  data(i)> data(idx_r);
    b =  ones(size(idx_l)) ;
     b2 =  ones(size(idx_r)) ;

     
    if (sum(a1-b)==0) && ( (sum(a2-b2)==0)) && data(i)>min_thresh(i)

        indexes(k)=i;
        vals(k)=data(i);
        k=k+1;
    end
    
end

if (k>1)

[vals,sort_idx]=sort(vals,'descend');
indexes=indexes(sort_idx);

end

n_indexes=k-1;
end