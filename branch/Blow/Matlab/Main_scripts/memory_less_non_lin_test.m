clear all ;close all;clc

len = 1024;
max_val = 1;

input_sig=0:max_val/(len-1):max_val;

 y = memoryless_blow_expander(input_sig,'quad',2);
 
 figure
 plot(input_sig,y,'r',input_sig,input_sig,'b');
