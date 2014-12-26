clear all; close all;clc

addpath /home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/Blow/Matlab/Alg_funcs
addpath /home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/Blow/Matlab/testbenches/fir/fir_funcs
%% Configuration
order=577;
Fc_normalized=0.176;
FIR_test = Fir_windows_generator(order,Fc_normalized);
FIR_test.PersistentMemory=true;
input_len=2^12;
frame_len = 2^8;
input_vector=[1,zeros(1,input_len-1)];
input_vector2=randn(1,input_len);
fir_coeffs=FIR_test.Numerator;
save('fir_test_ws');
%end Configuration

%% Unit Test 1
out=unit_test_1(FIR_test,input_vector);
%end Unit Test 1

%% Unit Test 2
out=unit_test_2(FIR_test,input_vector2,frame_len);
%end Unit Test 1


 