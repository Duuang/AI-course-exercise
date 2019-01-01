function Len=totalLength(V,citycood)
[xxx,order]=max(V);
newcood=citycood(:,order);
newcood=[newcood newcood(:,1)];
[r,c]=size(newcood);
Len=0;
for i=2:c
    Len=Len+dist(newcood(:,i-1)',newcood(:,1));
end