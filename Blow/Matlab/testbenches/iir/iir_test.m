clear all; close all;clc

addpath /home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/Blow/Matlab/Alg_funcs
addpath /home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/Blow/Matlab/testbenches/iir/iir_funcs
%% Configuration
Fs = 22050;
f1 = 200;
f2 = 1340;
db_att = 24;
IIR_test = Iir_butterwoth_generator(f1/(Fs/2),f2/(Fs/2),1,db_att);
IIR_test.PersistentMemory=true;
input_vector=[1,zeros(1,1200)];
scaleval=IIR_test.ScaleValues;
sosmat=IIR_test.sosMatrix;
%end Configuration

%% Unit Test 1
out=unit_test_1(IIR_test,input_vector);
%end Unit Test 1


 