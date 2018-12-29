#include <iostream>
#include <random>
#include <time.h>
#include <Windows.h>
#include <stdio.h>
#include <math.h>
#include <string>

#define SHOW_1_RESULT_PER_AMOUNT 1


#define NUM_OF_CITIES 24
#define POPULATION 50
#define RANDOM_RANGE 100000
#define CROSS_POSIBILITY 0.80
#define MUTATE_POSIBILITY 0.00

#define CROSS_LENGTH 5

#define MAX_ITERATION_TIMES 1000000



bool iffirsttime = true;

int Random(int x, int y) {
  std::random_device rd;
  //std::default_random_engine a;
  std::minstd_rand rng{ rd() };
  std::uniform_int_distribution<long> dist{ x, y };
  return dist(rng);
}


typedef struct {
  int x, y;
}City;

//int gene[POPULATION][NUM_OF_CITIES];
typedef struct {
  int gene[NUM_OF_CITIES];
  double fitness;
  double ifselect_upper;  //严格大于
  double ifselect_lower;  //小于或等于轮盘赌的数
}TSP;

City cities[NUM_OF_CITIES] = { {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {1, 7}
, {2, 7}, {3, 7}, {4, 7}, {5, 7}, {6, 7}, {7, 7}, {7, 6}, {7, 5}, {7, 4}, {7, 3}, {7, 2}, {7, 1}
, {6, 1}, {5, 1}, {4, 1}, {3, 1}, {2, 1} };
double distance[NUM_OF_CITIES][NUM_OF_CITIES];

TSP father_generation[POPULATION];
TSP next_generation[POPULATION];

void CitiesInit() {
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

void CalcFitness(TSP *individual) {
  double fitness;
  for (int i = 0; i < POPULATION; i++) {
    fitness = 0;
    for (int j = 0; j < NUM_OF_CITIES - 1; j++) {
      fitness += distance[(individual + i)->gene[j]][(individual + i)->gene[j + 1]];
    }
    fitness += distance[(individual + i)->gene[0]][(individual + i)->gene[NUM_OF_CITIES - 1]];
    fitness = pow(fitness, 3);
    fitness = 1.0 / fitness;
    father_generation[i].fitness = fitness;

  }
}

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
      if (father_generation[j].ifselect_lower <= randomnum && father_generation[j].ifselect_upper > randomnum) {
        memcpy(next_generation + i, father_generation + j, sizeof(father_generation[j]));
        break;
      } else {
        if (j == POPULATION - 1)
          memcpy(next_generation + i, father_generation + j, sizeof(father_generation[j]));
      }
    }
  }
  //父亲复制为子代值，下一步交叉在父代数组中进行
  memcpy(father_generation, next_generation, sizeof(next_generation));
}

void Cross() {
  double cross_possibility = CROSS_POSIBILITY;
  for (int i = 0; i < POPULATION; i++) {
    int randomnum = Random(0, RANDOM_RANGE);
    if (randomnum > cross_possibility * RANDOM_RANGE) {
      continue;
      //交叉，用的交替位置交叉法（Alternating Position Crossover，APX）
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

void Cross2() {
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
      int pos = Random(1, NUM_OF_CITIES - 1 - (CROSS_LENGTH - 1));
      
      for (int j = pos; j < pos + CROSS_LENGTH; j++) {
        int tmp = next_generation[father2].gene[j];
        next_generation[father2].gene[j] = next_generation[father1].gene[j];
        next_generation[father1].gene[j] = tmp;
        father_generation[i].gene[j] = tmp;
        exist[tmp] = 1;
      }
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

void Mutate() {
  double mutate_possibility = MUTATE_POSIBILITY;
  for (int i = 0; i < POPULATION; i++) {
    int randomnum = Random(0, RANDOM_RANGE);
    if (randomnum > mutate_possibility * RANDOM_RANGE) {
      continue;
    } else {
      int pos1 = Random(1, NUM_OF_CITIES - 1);
      int pos2 = Random(1, NUM_OF_CITIES - 1);
      int tmp = father_generation[i].gene[pos1];
      father_generation[i].gene[pos1] = father_generation[i].gene[pos2];
      father_generation[i].gene[pos2] = tmp;
    }
  }
}



int main() {
  CitiesInit();

  int count = 0;
  while (count < MAX_ITERATION_TIMES) {
    while (true) {
      CalcFitness(father_generation);
      if (!iffirsttime) {
        int lastmaxpos = 0;
        if (count % SHOW_1_RESULT_PER_AMOUNT == 0) {
          double mindistance = 0;
          for (int i = 0; i < POPULATION; i++) {
            if (father_generation[i].fitness > mindistance) {
              mindistance = father_generation[i].fitness;
              lastmaxpos = i;
            }
            }
          
          
          
          mindistance = 1 / mindistance;
          mindistance = pow(mindistance, 1.0 / 3);
            std::cout << mindistance << std::endl;
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
         mindistance = pow(mindistance, 1.0 / 3);
        if (mindistance > 50) {
          
          std::cout << mindistance << std::endl;
          iffirsttime = false;
          break;
        } else {
          CitiesInit();
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
}