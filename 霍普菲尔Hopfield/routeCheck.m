function [newV,checkres]=routeCheck(V)
[r,c]=size(V);
newV=zeros(r,c);
[XC,Order]=max(V);
for j=1:c
newV(Order(j),j)=1;
end
SC=sum(newV);
SR=sum(newV');
checkres=sumsqr(SC-SR);
Order
