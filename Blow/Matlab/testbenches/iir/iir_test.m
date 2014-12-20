clear all; close all;clc

addpath /home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/Blow/Matlab/Alg_funcs

%get filter params
Fs = 22050;
f1 = 200;
f2 = 1340;
db_att = 24;
IIR_test = Iir_butterwoth_generator(f1/(Fs/2),f2/(Fs/2),1,db_att);
IIR_test.PersistentMemory=true;


input_vector=[1,zeros(1,1200)];


out = filter(IIR_test,input_vector);


 scaleval=IIR_test.ScaleValues;
 sosmat=IIR_test.sosMatrix;