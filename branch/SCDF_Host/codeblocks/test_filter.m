clear all;close all;clc



a=[1.0,0.99978,-0.9898];
    b=[0.878,0.888,0.456];
%      state=[0.0,0.0];
     in=[1,2,3,4,5,6,7,8,9,0.1,-0.1,0.1];
     
     state=[0,0];
     [out,state]=filter(b,a,in,state)
     
     my_state=[0,0,0];
     [my_out,my_state]=my_filt(in,a,2,b,my_state,length(in))