function y = teager_kaiser(x)

% y = teager_kaiser(x) applies the Teager-Kaiser energy operator to
% input signal x.

x=x(:); % Setting x as a column vector

L = length(x);

tgk1 =  x(2:L-1).^2;
tgk2=x(1:L-2).*x(3:L);

y =tgk1-tgk2;
y = [y(1); y; y(L-2)]; % so that lenght(y)=length(x)

