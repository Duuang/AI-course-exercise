#include <iostream>
#include <random>
#include <time.h>
#include <Windows.h>
#include <stdio.h>
#include <math.h>
#include <string>

//**************************************
//parameters
//**************************************

#define RANDOM_RANGE 100000

#define PHEROMONE_INIT_VALUE 100

#define NUM_OF_CITIES 24
#define POPULATION 50
#define INFLUENCE_OF_PHEROMONE 1
#define INFLUENCE_OF_DISTANCE 10
#define VOLATILIZATION 0.01

#define MAX_ITERATION_TIMES 1000000

typedef struct {
  int x, y;
}City;

typedef struct {
  int route[NUM_OF_CITIES];
  int visited[NUM_OF_CITIES];
  int currentsteps;

  double distance_sum;
}TSP;

//**************************************
//global variables start here
//**************************************
City cities[NUM_OF_CITIES] = { {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {1, 7}
, {2, 7}, {3, 7}, {4, 7}, {5, 7}, {6, 7}, {7, 7}, {7, 6}, {7, 5}, {7, 4}, {7, 3}, {7, 2}, {7, 1}
, {6, 1}, {5, 1}, {4, 1}, {3, 1}, {2, 1} };

double distance[NUM_OF_CITIES][NUM_OF_CITIES];
double pheromone[NUM_OF_CITIES][NUM_OF_CITIES];

TSP ant[POPULATION];


int Random(int x, int y) {
  std::random_device rd;
  //std::default_random_engine a;
  std::minstd_rand rng{ rd() };
  std::uniform_int_distribution<long> dist{ x, y };
  return dist(rng);
}

void Init() {
  //初始化距离矩阵，下标从0开始，到n-1
  for (int i = 0; i < NUM_OF_CITIES; i++) {
    for (int j = 0; j < NUM_OF_CITIES; j++) {
      distance[i][j] = sqrt((cities[i].x - cities[j].x) * (cities[i].x - cities[j].x)
        + (cities[i].y - cities[j].y) * (cities[i].y - cities[j].y));
    }
  }
  for (int i = 0; i < NUM_OF_CITIES; i++) {
    for (int j = 0; j < NUM_OF_CITIES; j++) {
      pheromone[i][j] = PHEROMONE_INIT_VALUE;
    }
  }
  //初始化蚂蚁位置
  memset(ant, 0, sizeof(ant));
  for (int i = 0; i < POPULATION; i++) {
    int randomstart = Random(0, NUM_OF_CITIES - 1);
    ant[i].route[0] = randomstart;
    ant[i].visited[randomstart] = 1;
    ant[i].currentsteps = 0;
    ant[i].distance_sum = 0;
  }

}

void AllAntsMoveOneStep() {
  for (int i = 0; i < POPULATION; i++) {
    while (ant[i].currentsteps < NUM_OF_CITIES - 1) {
      int current_city = ant[i].route[ant[i].currentsteps];
      double decision_sum = 0;
      double decision_calculation[NUM_OF_CITIES] = { 0 };
      //计算每个路径的概率的大小
      for (int j = 0; j < NUM_OF_CITIES; j++) {
        if (ant[i].visited[j]) {
          continue;
        }
        decision_calculation[j] = pow(pheromone[current_city][j], INFLUENCE_OF_PHEROMONE)
          * pow(1 / distance[current_city][j], INFLUENCE_OF_DISTANCE);
        decision_sum += decision_calculation[j];
      }
      //按概率走一步 ？ 还是按最大的走（不能吧）？
      int random = Random(0, RANDOM_RANGE - 1);  //TODO: 减了1，怕万一算完了正好卡在这个值上。。不过几率很小
      int lastcity = -1;
      double lower_bound[NUM_OF_CITIES] = { 0 };
      double upper_bound[NUM_OF_CITIES] = { 0 };
      bool firsttime = true;
      //给每个城市分配随机数中奖区间
      for (int j = 0; j < NUM_OF_CITIES; j++) {
        if (ant[i].visited[j]) {
          continue;
        }
        if (firsttime) {
          lower_bound[j] = 0;
          upper_bound[j] = lower_bound[j] + (decision_calculation[j] / decision_sum) * RANDOM_RANGE;
          firsttime = false;
          lastcity = j;
        } else {
          lower_bound[j] = upper_bound[lastcity];
          upper_bound[j] = lower_bound[j] + (decision_calculation[j] / decision_sum) * RANDOM_RANGE;
          lastcity = j;
        }
      }
      //看哪个城市中奖，然后走
      for (int j = 0; j < NUM_OF_CITIES; j++) {
        if (ant[i].visited[j]) {
          continue;
        }
        if (lower_bound[j] <= random && upper_bound[j] > random) {
          ant[i].route[ant[i].currentsteps + 1] = j;
          ant[i].currentsteps++;
          ant[i].visited[j] = 1;
          break;
        }
      }
    }
  }
}

void UpdatePheromoneAndInit() {
  //更新信息素
  
  for (int i = 0; i < POPULATION; i++) {
    //计算总距离
    for (int j = 0; j < NUM_OF_CITIES - 1; j++) {
      ant[i].distance_sum += distance[ant[i].route[j]][ant[i].route[j + 1]];
    }
    ant[i].distance_sum += distance[ant[i].route[0]][ant[i].route[NUM_OF_CITIES - 1]];
    //根据总距离更新信息素
    for (int j = 0; j < NUM_OF_CITIES - 1; j++) {
      pheromone[ant[i].route[j]][ant[i].route[j + 1]] = (1 - VOLATILIZATION) * pheromone[ant[i].route[j]][ant[i].route[j + 1]];
      pheromone[ant[i].route[j + 1]][ant[i].route[j]] = (1 - VOLATILIZATION) * pheromone[ant[i].route[j + 1]][ant[i].route[j]];
      pheromone[ant[i].route[j]][ant[i].route[j + 1]] += 100 / ant[i].distance_sum;
      pheromone[ant[i].route[j + 1]][ant[i].route[j]] += 100 / ant[i].distance_sum;
    }
    pheromone[ant[i].route[0]][ant[i].route[NUM_OF_CITIES - 1]] = (1 - VOLATILIZATION) * pheromone[ant[i].route[0]][ant[i].route[NUM_OF_CITIES - 1]];
    pheromone[ant[i].route[NUM_OF_CITIES - 1]][ant[i].route[0]] = (1 - VOLATILIZATION) * pheromone[ant[i].route[NUM_OF_CITIES - 1]][ant[i].route[0]];
    pheromone[ant[i].route[0]][ant[i].route[NUM_OF_CITIES - 1]] += 100 / ant[i].distance_sum;
    pheromone[ant[i].route[NUM_OF_CITIES - 1]][ant[i].route[0]] += 100 / ant[i].distance_sum;
  }
}
void InitAntsForNextIteration() {
  //初始化蚂蚁
     //初始化蚂蚁位置
    memset(ant, 0, sizeof(ant));
    for (int i = 0; i < POPULATION; i++) {
    int randomstart = Random(0, NUM_OF_CITIES - 1);
    ant[i].route[0] = randomstart;
    ant[i].visited[randomstart] = 1;
    ant[i].currentsteps = 0;
    ant[i].distance_sum = 0;
  }

}



int main() {
  int count = 0;
  Init();

  while (count <= MAX_ITERATION_TIMES) {
    AllAntsMoveOneStep();
    UpdatePheromoneAndInit();
    double mindistance = 0xFFFFFF;
    
    for (int i = 0; i < POPULATION; i++) {
      if (ant[i].distance_sum < mindistance) {
        mindistance = ant[i].distance_sum;
      }
    }
    std::cout << mindistance << std::endl;

    InitAntsForNextIteration();
    count++;
  }
}