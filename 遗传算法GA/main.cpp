#include <iostream>
#include <random>
#include <time.h>
#include <Windows.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <fstream>

//要打开的tsp数据文件的路径，格式为每个点的坐标，具体格式见样例，可以从github下载数据文件来测试
#define FILEPATH "C:\\Users\\cky\\Documents\\GitHub\\TSPtestdata.txt"
//**************************************
//
//author: Duuang
//https://github.com/Duuang/AI-course-exercise
//
//**************************************


//**************************************
//parameters
//**************************************

#define POW_VALUE 10  //计算适应度时的指数大小
#define SHOW_1_RESULT_PER_AMOUNT 100  //每多少次迭代显示一次结果

#define NUM_OF_CITIES 48  //城市个数
#define POPULATION 500     //种群数量
#define RANDOM_RANGE 1000000    //生成随机数的最大范围，可以不用管
#define CROSS_POSIBILITY 0.90  //交叉概率
#define MUTATE_POSIBILITY 0.10  //变异概率

#define CROSS_LENGTH 20  //采用固定长度段交叉时的交叉段长度

#define MAX_ITERATION_TIMES 1000000  //最大迭代次数

//城市坐标
typedef struct {
  int x, y;
}City;

//表示一个个体
typedef struct {
  int gene[NUM_OF_CITIES];  //编码方式为城市编号序列，如 {0, 1, 2, 3, 4}
  double fitness;          //适应度
  double ifselect_upper;  //严格大于轮盘赌的数
  double ifselect_lower;  //小于或等于轮盘赌的数
}TSP;

//**************************************
//global variables start here
//**************************************

bool iffirsttime = true;  //是否是第一次迭代（没啥用，用来在第一次的值不够坏时重新初始化）

City cities[NUM_OF_CITIES];  //城市
double distance[NUM_OF_CITIES][NUM_OF_CITIES];  //城市距离矩阵

TSP father_generation[POPULATION];  //父代个体
TSP next_generation[POPULATION];   //子代个体（实际上被用做缓冲区而已，主要保存在父代中）

//
//生成[x,y]范围的随机整数
//
int Random(int x, int y) {
  std::random_device rd;
  //std::default_random_engine a;
  std::minstd_rand rng{ rd() };
  std::uniform_int_distribution<long> dist{ x, y };
  return dist(rng);
}

//
//初始化距离矩阵，和个体的基因
//
void Init() {
  //初始化距离矩阵，下标从0开始，到n-1
  for (int i = 0; i < NUM_OF_CITIES; i++) {
    for (int j = 0; j < NUM_OF_CITIES; j++) {
      distance[i][j] = sqrt((cities[i].x - cities[j].x) * (cities[i].x - cities[j].x)
        + (cities[i].y - cities[j].y) * (cities[i].y - cities[j].y));
    }
  }
  //初始化每个个体的基因，以0开头，其他随意
  for (int i = 0; i < POPULATION; i++) {
    father_generation[i].gene[0] = 0;
    int count = 0;
    int exist[NUM_OF_CITIES] = { 0 };
    while (true) {
      father_generation[i].gene[count + 1] = Random(1, NUM_OF_CITIES - 1);
      if (exist[father_generation[i].gene[count + 1]]) {

      } else {
        exist[father_generation[i].gene[count + 1]] = 1;
        count++;
        if (count == NUM_OF_CITIES - 1)
          break;
      }
    }
  }
}

//
//计算群体中每个个体的适应度，是用的默认公式加了指数
//
void CalcFitness(TSP *individual) {
  double fitness;
  for (int i = 0; i < POPULATION; i++) {
    fitness = 0;
    for (int j = 0; j < NUM_OF_CITIES - 1; j++) {
      fitness += distance[(individual + i)->gene[j]][(individual + i)->gene[j + 1]];
    }
    fitness += distance[(individual + i)->gene[0]][(individual + i)->gene[NUM_OF_CITIES - 1]];
    //！！！因为。。觉得收敛不够快，就加了个指数上去，普通遗传算法是指数为1的情况
    fitness = pow(fitness, POW_VALUE);
    fitness = 1.0 / fitness;
    father_generation[i].fitness = fitness;
  }
}

//
//选择，从父代数组中选择，保存到子代数组
//用轮盘赌方式，概率性选择
//
void Select() {
  double fitness_sum = 0;
  for (int i = 0; i < POPULATION; i++) {
    fitness_sum += father_generation[i].fitness;
  }
  //算轮盘赌分配的区域
  father_generation[0].ifselect_lower = 0;
  father_generation[0].ifselect_upper = RANDOM_RANGE * father_generation[0].fitness / fitness_sum;
  for (int i = 1; i < POPULATION; i++) {
    father_generation[i].ifselect_lower = father_generation[i - 1].ifselect_upper;
    father_generation[i].ifselect_upper = father_generation[i].ifselect_lower + RANDOM_RANGE * father_generation[i].fitness / fitness_sum;
  }
  //生成n个随机数
  for (int i = 0; i < POPULATION; i++) {
    int randomnum = Random(0, RANDOM_RANGE);
    for (int j = 0; j < POPULATION; j++) {
      //如果轮盘赌选中了，则把这个个体加到子代中，继续轮盘赌选下一个
      if (father_generation[j].ifselect_lower <= randomnum && father_generation[j].ifselect_upper > randomnum) {
        memcpy(next_generation + i, father_generation + j, sizeof(father_generation[j]));
        break;
      } else {
        if (j == POPULATION - 1)
          memcpy(next_generation + i, father_generation + j, sizeof(father_generation[j]));
      }
    }
  }
}

//
//交叉函数1，失败的交叉函数。。用的交替位置交叉法（Alternating Position Crossover，APX）
//事实证明这个交叉方法就是逗比，因为根本不保留整段基因啊。。跟完全随机有啥区别么？
//程序中调用的是下面的Cross2()函数
//
void Cross() {
  //交叉概率，判断是否交叉
  double cross_possibility = CROSS_POSIBILITY;
  for (int i = 0; i < POPULATION; i++) {
    int randomnum = Random(0, RANDOM_RANGE);
    if (randomnum > cross_possibility * RANDOM_RANGE) {
      continue;
      //交叉，用的交替位置交叉法（Alternating Position Crossover，APX）
      //大概意思是，基因1：a1 a2 a3 a4 a5
      //           基因2：b1 b2 b3 b4 b5
      //然后按照a1->b1->a2->b2->a3......的顺序，依次把一个基因选出来组成DNA，如果遇到重复的则跳过，看下一个
    } else {
      int father1 = Random(0, POPULATION - 1);
      int father2 = Random(0, POPULATION - 1);
      int exist[NUM_OF_CITIES] = { 0 };
      int count = 0;  //count == n - 1时跳出
      for (int j = 1; true; j++) {
        if (exist[next_generation[father1].gene[j]]) {
          ;  // 如果存在，直接下一个
        } else {  //如果不存在，则附加到基因上
          father_generation[i].gene[count + 1] = next_generation[father1].gene[j];
          exist[next_generation[father1].gene[j]] = 1;
          count++;
          if (count == NUM_OF_CITIES - 1)
            break;
        }
        if (exist[next_generation[father2].gene[j]]) {
          ;
        } else {
          father_generation[i].gene[count + 1] = next_generation[father2].gene[j];
          exist[next_generation[father2].gene[j]] = 1;
          count++;
          if (count == NUM_OF_CITIES - 1)
            break;
        }
      }
    }
  }
}

//
//交叉函数2，吸取了教训，这回用的是整段基因的交叉。。
//交叉长度已经给定，随机选择交叉起点，个体1的基因和个体2的基因在这段上交换，
//然后，其他基因就按照个体1的填，如果遇到冲突，则选择填入一个最小的不冲突的城市
//
void Cross2() {
  //交叉概率
  double cross_possibility = CROSS_POSIBILITY;
  for (int i = 0; i < POPULATION; i++) {
    int randomnum = Random(0, RANDOM_RANGE);
    if (randomnum > cross_possibility * RANDOM_RANGE) {
      continue;
      //交叉，上面那方法坑爹啊！！试试固定长度两点交叉
    } else {
      int father1 = Random(0, POPULATION - 1);
      int father2 = Random(0, POPULATION - 1);
      //指定一个固定长度的一段，起始位置随机，来进行两点间交叉
      int exist[NUM_OF_CITIES] = { 0 };
      //pos:起始位置
      int pos = Random(1, NUM_OF_CITIES - 1 - (CROSS_LENGTH - 1));

      //交换指定的那段基因
      for (int j = pos; j < pos + CROSS_LENGTH; j++) {
        int tmp = next_generation[father2].gene[j];
        father_generation[i].gene[j] = tmp;
        exist[tmp] = 1;
      }
      //其他的基因，按照能填就填，冲突就找一个不冲突的，这么填完
      for (int j = 1; j < pos; j++) {
        if (exist[next_generation[father1].gene[j]] == 0) {
          father_generation[i].gene[j] = next_generation[father1].gene[j];
          exist[next_generation[father1].gene[j]] = 1;
        } else {
          for (int k = 1; k < NUM_OF_CITIES; k++) {
            if (exist[k] == 0) {
              father_generation[i].gene[j] = k;
              exist[k] = 1;
              break;
            }
          }
        }
      }
      for (int j = pos + CROSS_LENGTH; j < NUM_OF_CITIES; j++) {
        if (exist[next_generation[father1].gene[j]] == 0) {
          father_generation[i].gene[j] = next_generation[father1].gene[j];
          exist[next_generation[father1].gene[j]] = 1;
        } else {
          for (int k = 1; k < NUM_OF_CITIES; k++) {
            if (exist[k] == 0) {
              father_generation[i].gene[j] = k;
              exist[k] = 1;
              break;
            }
          }
        }
      }
      /*while (true) {
        int ifallnoneexist = true;
        for (int k = pos; k < pos + CROSS_LENGTH; k++) {
          if (exist[next_generation[father1].gene[k]] != 0) {
            ifallnoneexist = false;
            int tmp = next_generation[father2].gene[k];
            next_generation[father2].gene[k] = next_generation[father1].gene[k];
            next_generation[father1].gene[k] = tmp;
            break;
          }
        }
        if (ifallnoneexist)
          break;
      }*/
      /*if (exist[next_generation[father2].gene[j]] == 0) {
        father_generation[i].gene[j] = next_generation[father2].gene[j];
        exist[next_generation[father2].gene[j]] = 1;
      }*/

    }
  }
}

//
//突变函数
//若在突变概率中，则随机选两个位置，交换即可
//
void Mutate() {
  //突变概率，判断是否进行突变
  double mutate_possibility = MUTATE_POSIBILITY;
  for (int i = 0; i < POPULATION; i++) {
    int randomnum = Random(0, RANDOM_RANGE);
    if (randomnum > mutate_possibility * RANDOM_RANGE) {
      continue;
    } else {
      //随机两个位置
      int pos1 = Random(1, NUM_OF_CITIES - 1);
      int pos2 = Random(1, NUM_OF_CITIES - 1);
      //交换
      int tmp = father_generation[i].gene[pos1];
      father_generation[i].gene[pos1] = father_generation[i].gene[pos2];
      father_generation[i].gene[pos2] = tmp;
    }
  }
}

//
//将文件数据存进内存
//
void GetTestDataFromFile() {
  errno_t err;
  FILE *fp;
  err = fopen_s(&fp, "C:\\Users\\cky\\Documents\\GitHub\\TSPtestdata.txt", "r");
  if (fp == NULL) {
    printf("open file failed\n");
    return;
  }
  fseek(fp, 0L, SEEK_END);/* 定位到文件末尾 */  //h.t.t.p.s://blog.csdn.net/jiayanhui2877/article/details/8222882
  int flen = ftell(fp); /* 得到文件大小 */
  char stringbuffer[100005] = { '\0' };
  fseek(fp, 0L, SEEK_SET); /* 定位到文件开头 */
  fread(stringbuffer, flen, 1, fp); /* 一次性读取全部文件内容 */
  //TODO:: 缓冲区溢出了，文件里的\r\n，到了缓冲区里就剩个一字节的\n了
  int notnumpos[100005];
  int count = 0;
  notnumpos[0] = -1;
  for (int i = 0; i < flen; i++) {
    if (stringbuffer[i] > '9' || stringbuffer[i] < '0') {
      stringbuffer[i] = '\0';
      notnumpos[count + 1] = i;
      count++;
    }
  }
  count = 0;
  for (int i = 0; i < NUM_OF_CITIES; i++) {
    sscanf_s(stringbuffer + notnumpos[2 * i] + 1, "%d", &cities[i].x);
    sscanf_s(stringbuffer + notnumpos[2 * i + 1] + 1, "%d", &cities[i].y);
  }
  fclose(fp);
}


int main() {
  GetTestDataFromFile();

  Init();

  int count = 0;
  while (count < MAX_ITERATION_TIMES) {
    while (true) {
      CalcFitness(father_generation);
      if (!iffirsttime) {
        int lastmaxpos = 0;
        //count为设定值的倍数时，才输出到控制台
        if (count % SHOW_1_RESULT_PER_AMOUNT == 0) {
          double mindistance = 0;
          for (int i = 0; i < POPULATION; i++) {
            if (father_generation[i].fitness > mindistance) {
              mindistance = father_generation[i].fitness;
              lastmaxpos = i;
            }
          }
          mindistance = 1 / mindistance;
          mindistance = pow(mindistance, 1.0 / POW_VALUE);

          std::cout << count << ":  " << mindistance << std::endl;
          if (mindistance < 23.9) {
            printf("error!!!: %d\n", lastmaxpos);
            system("pause");
          }
        }
        break;

      } else {
        double mindistance = 0;
        for (int i = 0; i < POPULATION; i++) {
          if (father_generation[i].fitness > mindistance) {
            mindistance = father_generation[i].fitness;
          }
        }
        mindistance = 1 / mindistance;
        mindistance = pow(mindistance, 1.0 / POW_VALUE);
        if (mindistance > 50) {

          std::cout << "initial: " << mindistance << std::endl;
          iffirsttime = false;
          break;
        } else {
          Init();
        }
      }
    }
    count++;

    Select();
    Cross2();
    Mutate();

  }
  //std::cout << mindistance << std::endl;
   /*FILE *plotfp = _popen("gnuplot.exe","w");
   if (plotfp == NULL) {
     printf("open file failed\n");
   } else {
     char plot_output[] = "plot sin(x)\n";
     fprintf(plotfp, "%s", plot_output);
     fflush(plotfp);
   }*/
   // plot for [i=0:2] 'C:\Users\cky\Documents\GitHub\aaa.txt' index i with points

  Sleep(1000);
  printf("iteration stopped\n");
  system("pause");
  return 0;
}