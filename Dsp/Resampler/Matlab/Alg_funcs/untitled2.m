function [modulo_idx,upsampled_total_idx]=get_res_indexes(upfact,downfact,init_phase)


persistent modu;
persistent new_modu;
persistent delta_sample_slope_acc;

if isempty(modu)
    modu= mod(downfact,upfact);
end 

if isempty(new_modu)
    new_modu=init_phase;
end 

if isempty(delta_sample_slope_acc)
    delta_sample_slope_acc=0;
end


integer = fix(downfact/upfact);


    
    mod_acc = new_modu + modu;
    new_modu = mod(mod_acc,upfact);
    delta_sample_slope =  (integer+fix(mod_acc/upfact));
    delta_sample_slope_acc = delta_sample_slope_acc + delta_sample_slope;
    upsampled_total_idx = delta_sample_slope_acc * upfact + new_modu;
    modulo_idx=(new_modu);
%     elab_downsample(i)= index+1;
    



end