clear all;close all;clc

addpath /home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/Blow/Matlab/Alg_funcs/
addpath /home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/Blow/Matlab/testbenches/polyfit/poly_funcs/

%% Configuration
fit_desiderata_x=[0,0.05,0.1,0.3,0.5,1];
 fit_desiderata_y=[0,0.05,0.1,0.3,0.7,1.5];
 [xData, yData] = prepareCurveData( fit_desiderata_x, fit_desiderata_y );

% Set up fittype and options.
ft = fittype( 'pchipinterp' );
opts = fitoptions( ft );

% Fit model to data.
[fitresult, gof] = fit( xData, yData, ft, opts );
test_inp = 0:1/255:1;
breaks = fitresult.p.breaks;
coeffs = fitresult.p.coefs;
y = memoryless_blow_expander_fo(test_inp,fitresult);
%end Configuration

%% Unit Test 1
out=Unit_test1(test_inp,fitresult);
%end Unit Test 1

check_=sum(abs(out(:)-y(:)));