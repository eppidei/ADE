clear all; close all;clc

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% based on "Estimating Three-Dimensional Orientation of Human Body Parts by
% Inertial/Magnetic Sensing, 2011"
% Angelo Maria Sabatini
% Additive EKF, quaternion based, direct state
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%




%%%%path definition
if (isunix || ismac)
    separator='/';
elseif ispc
    separator='\';
else
    warning('Computer unknown check path separators');
end

this_dir =['.',separator];
up_dir   =['..',separator];

res_path = [up_dir,up_dir,up_dir,'SCDF_Host',separator,'Matlab',separator,'resources'];
funcs_path = [up_dir,'Alg_funcs'];
mex_exec_path = [up_dir,'Mex',separator,'exec'];
test_file ='testaudio_soffiodistante.wav';

addpath(funcs_path)
addpath(res_path)

load('dump_rotation_m90p90_ipad');

Fs = 50; 


