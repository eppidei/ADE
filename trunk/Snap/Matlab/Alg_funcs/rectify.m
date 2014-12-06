function y=rectify(x)

l=abs(x);

lp=fir1(248,0.2);

y=filter(lp,1,l);