function d_U=cacuDeltaU(V,d,A,D,step_t)
[n,n]=size(V);
t1=repmat(sum(V,2)-1,1,n);
t2=repmat(sum(V,1)-1,n,1);
permitV=V(:,2:n);
permitV=[permitV V(:,1)];
t3=d*permitV;
d_U=-step_t*(A*t1+A*t2+D*t3);