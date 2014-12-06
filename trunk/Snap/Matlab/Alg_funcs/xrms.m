function y=xrms(L,tav)

len=length(L);

y=zeros(1,len);

for i=2:len
y(i) =  (1-tav)*y(i-1) + tav * L(i)^2;
end