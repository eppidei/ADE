clear all;close all;clc

%%%This logic find the indexes you have to choose to downsample a vector by
%%%downrate after you have upsampled by mod_count

downrate =17;

orig_sample = 1:downrate*100;
orig_downsample = orig_sample(1:downrate:end);

mod_count = 83;

integer = fix(downrate/mod_count);
modu = mod(downrate,mod_count);
res_phase = 0;

elab_downsample = zeros(1,length(orig_downsample)-1);
index = 0;
new_modu = 0;
delta_sample_slope_acc = 0;
mod_acc = 0;
index_collected = zeros(1,length(orig_downsample)-1);
for i = 1:length(orig_downsample)-1
    
    mod_acc = new_modu + modu;
    new_modu = mod(mod_acc,mod_count);
    delta_sample_slope =  (integer+fix(mod_acc/mod_count));
    delta_sample_slope_acc = delta_sample_slope_acc + delta_sample_slope;
    index = delta_sample_slope_acc * mod_count + new_modu;
    index_collected(i)=(new_modu);
    elab_downsample(i)= index+1;
    
end
    
max(abs(orig_downsample(2:end)-elab_downsample))

 figure
 stem(index_collected);
    

% plot(diff(elab_downsample))
    