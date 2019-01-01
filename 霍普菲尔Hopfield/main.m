% TSP based Hopfield 
N=18;A=1.5;D=1;u0=0.02;
step_t=0.1;        %����
max=50000000;        %��������
citycood=[0.1, 0.5, 0.8, 0.6, 0.4, 0.2, 0.800, 0.2439, 0.1707, 0.2239, 0.5171,0.8732, 0.6878,0.8488,0.6683,0.6195,0.3250, 0.3520;
      
          0.8, 0.5, 0.1, 0.2, 0.4, 0.6, 0.9439, 0.1463,0.2293,0.7610,0.9414,0.6536,0.5219,0.3609,0.2536,0.2634 ,0.8523,0.8253 ]; %��������
%citycood=[0.5,0.3,0.4;0.4,0.4,0.1]; %��������  
d=dist(citycood',citycood);      %���м�������
U=0.2*rand(N,N)-0.1;
for count=1:max
    V=(1+tansig(U/u0))/2;
    E=cacuEnergy(V,d,A,D);   %��������,����1
    d_U=cacuDeltaU(V,d,A,D,step_t); %����U������������2
    U=U+d_U*step_t;
end
[newV,checkres]=routeCheck(V);      %���V�Ƿ�����Ч·��������3
if checkres<1
    finalE=cacuEnergy(newV,d,A,D);%
    routeLen=totalLength(newV,citycood);  %����·����ʵ���ȣ�����4
    plotRoute(newV,citycood);       %����·����������
else
    disp('·����Ч��');
end

