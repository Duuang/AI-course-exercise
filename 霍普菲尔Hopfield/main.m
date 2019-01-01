% TSP based Hopfield 
N=18;A=1.5;D=1;u0=0.02;
step_t=0.1;        %步长
max=50000000;        %迭代次数
citycood=[0.1, 0.5, 0.8, 0.6, 0.4, 0.2, 0.800, 0.2439, 0.1707, 0.2239, 0.5171,0.8732, 0.6878,0.8488,0.6683,0.6195,0.3250, 0.3520;
      
          0.8, 0.5, 0.1, 0.2, 0.4, 0.6, 0.9439, 0.1463,0.2293,0.7610,0.9414,0.6536,0.5219,0.3609,0.2536,0.2634 ,0.8523,0.8253 ]; %城市坐标
%citycood=[0.5,0.3,0.4;0.4,0.4,0.1]; %城市坐标  
d=dist(citycood',citycood);      %城市间距离矩阵
U=0.2*rand(N,N)-0.1;
for count=1:max
    V=(1+tansig(U/u0))/2;
    E=cacuEnergy(V,d,A,D);   %计算能量,函数1
    d_U=cacuDeltaU(V,d,A,D,step_t); %计算U的增量，函数2
    U=U+d_U*step_t;
end
[newV,checkres]=routeCheck(V);      %检查V是否是有效路径，函数3
if checkres<1
    finalE=cacuEnergy(newV,d,A,D);%
    routeLen=totalLength(newV,citycood);  %计算路径真实长度，函数4
    plotRoute(newV,citycood);       %绘制路径，函数五
else
    disp('路径无效！');
end

