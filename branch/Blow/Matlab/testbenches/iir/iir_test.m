clear all; close all;clc

addpath /home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/Blow/Matlab/Alg_funcs
addpath /home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/Blow/Matlab/testbenches/iir/iir_funcs
%% Configuration
Fs = 22050;
f1 = 200;
f2 = 1340;
db_att = 80;
IIR_test = Iir_butterwoth_generator(f1/(Fs/2),f2/(Fs/2),1,db_att);
IIR_test.PersistentMemory=true;
input_len=2^12;
frame_len = 2^8;
input_vector=[1,zeros(1,input_len-1)];
input_vector2=randn(1,input_len);
scaleval=IIR_test.ScaleValues;
sosmat=(IIR_test.sosMatrix);
save('iir_test_ws');
%end Configuration

%% Unit Test 1
out=unit_test_1(IIR_test,input_vector);
%end Unit Test 1

%% Unit Test 2
out=unit_test_2(IIR_test,input_vector2,frame_len);
%end Unit Test 1


 