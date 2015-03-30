function [idx_rel_out,upsampled_total_idx_out,block_idx]=get_res_indexes(upfact,downfact,init_phase)


persistent idx_rel_acc_delta;
persistent idx_rel;
persistent n_upsampled_block_delta_acc;

if isempty(idx_rel_acc_delta)
    idx_rel_acc_delta= mod(downfact,upfact);
end 

if isempty(idx_rel)
    idx_rel=init_phase;
end 

if isempty(n_upsampled_block_delta_acc)
    n_upsampled_block_delta_acc=0;
end

constant_n_upsampled_block_delta = fix(downfact/upfact);


    %calcolo indice relativo all' upfact del campione da pescare zero base
    
    idx_rel_acc = idx_rel + idx_rel_acc_delta;
    idx_rel = mod(idx_rel_acc,upfact);
   
    
     %calcolo indice assoluto del campione da pescare rispetto al frame
     %upsamplato zero based
    variable_n_upsampled_block_delta=fix(idx_rel_acc/upfact);
    n_upsampled_block_delta =  constant_n_upsampled_block_delta+variable_n_upsampled_block_delta;
    n_upsampled_block_delta_acc = n_upsampled_block_delta_acc + n_upsampled_block_delta;
    upsampled_total_idx = n_upsampled_block_delta_acc * upfact + idx_rel ;
    
%     elab_downsample(i)= index+1;
     idx_rel_out=idx_rel  +1;
     upsampled_total_idx_out = upsampled_total_idx + 1;
       block_idx=n_upsampled_block_delta_acc+1;


end