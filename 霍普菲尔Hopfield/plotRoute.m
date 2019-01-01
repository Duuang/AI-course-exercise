function   plotRoute(V,citycood)
figure;
title('连续Hopfield网络解决TSP');
xlabel('X坐标');
ylabel('Y坐标');
axis([0 10 0 10]);
axis on;
[xxx,order]=max(V);
newcood=citycood(:,order);
newcood=[newcood newcood(:,1)];
plot(newcood(1,:),newcood(2,:),'o-');