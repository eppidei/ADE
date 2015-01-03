clear all;close all;clc

 in_len = 16;
  real_input=randn(1,in_len);
% cplx_input=complex(randn(1,in_len));



  out_c2c = fft(real_input);
%  out_c2c_forward = fft(cplx_input);
%   out_c2c_back = ifft(cplx_input);
%  out_real = ifft(out_cplx);
 save('fft_test_ws');
%end Configuration
 