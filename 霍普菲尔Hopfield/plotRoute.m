function   plotRoute(V,citycood)
figure;
title('����Hopfield������TSP');
xlabel('X����');
ylabel('Y����');
axis([0 10 0 10]);
axis on;
[xxx,order]=max(V);
newcood=citycood(:,order);
newcood=[newcood newcood(:,1)];
plot(newcood(1,:),newcood(2,:),'o-');