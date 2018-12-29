#include <iostream>
#include <random>
#include <time.h>
#include <Windows.h>
#include <stdio.h>
#include <math.h>
#include <string>

//**************************************
//
//author: Duuang
//https://github.com/Duuang/AI-course-exercise
//
//**************************************


//**************************************
//parameters
//**************************************

#define RANDOM_RANGE 100000000  //生成随机数的最大范围，可以不用管

#define PHEROMONE_INIT_VALUE 1.0  //城市间初始化的信息素浓度，影响不大

#define NUM_OF_CITIES 48  //城市个数
#define POPULATION 34     //蚂蚁个数，m值
#define INFLUENCE_OF_PHEROMONE 1  //α值，信息素的影响权重
#define INFLUENCE_OF_DISTANCE 5   //β值，当前距离的影响权重
#define Q_VALUE 100     //Q值，每次蚂蚁走完一圈之后，一共释放的信息素的量
#define VOLATILIZATION 0.5  //(1 - ρ）中 ρ 的值，挥发系数

#define MAX_ITERATION_TIMES 1000000  //最大迭代次数

//城市坐标
typedef struct {
  int x, y;
}City;

//蚂蚁个体
typedef struct {
  int route[NUM_OF_CITIES];  //路径记录
  int visited[NUM_OF_CITIES];  //已访问记录
  int currentsteps;  //现在已经走的步数

  double distance_sum;  //个体走的总距离
}TSP;

//**************************************
//global variables start here
//**************************************、
City cities[NUM_OF_CITIES];  //城市

double distance[NUM_OF_CITIES][NUM_OF_CITIES];  //城市间距离
double pheromone[NUM_OF_CITIES][NUM_OF_CITIES];  //城市间信息素浓度

TSP ant[POPULATION];  //蚁群

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
//初始化距离矩阵和蚂蚁位置
//
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

//
//所有蚂蚁走完一圈完整的路径
//
void AllAntsMoveAround() {
  for (int i = 0; i < POPULATION; i++) {
    //每个蚂蚁的每步：当步数足够时跳出循环
    while (ant[i].currentsteps < NUM_OF_CITIES - 1) {
      int current_city = ant[i].route[ant[i].currentsteps];
      double decision_sum = 0;
      double decision_calculation[NUM_OF_CITIES] = { 0 };
      //计算每个路径的概率的大小
      for (int j = 0; j < NUM_OF_CITIES; j++) {
        if (ant[i].visited[j]) {
          continue;
        }
        //当前路径概率
        decision_calculation[j] = pow(pheromone[current_city][j], INFLUENCE_OF_PHEROMONE)
          * pow(1 / distance[current_city][j], INFLUENCE_OF_DISTANCE);
        //概率的和
        decision_sum += decision_calculation[j];
      }
      //按转移概率，用轮盘赌方式，选择下一步
      double random = Random(0, RANDOM_RANGE);
      //---------------------更改的部分，其实底下注释掉的部分也是一样，也没有错误：
      bool ifcanselect = false;
      int lastj = -100;
      if (decision_sum > 0) {  //当迭代次数很大时，decision_sum甚至都会变成0，这时不单独处理的话程序就挂掉了
        random = random * (decision_sum / RANDOM_RANGE);
        for (int j = 0; j < NUM_OF_CITIES; j++) {
          if (ant[i].visited[j]) {
            continue;
          } else {
            random = random - decision_calculation[j];
            if (random < 0.0) {
              ant[i].route[ant[i].currentsteps + 1] = j;
              ant[i].currentsteps++;
              ant[i].visited[j] = 1;
              ifcanselect = true;
              break;
            }
            lastj = j;
          }
        }
      }
      if (!ifcanselect) {  //如果decision_sum就是0，无法选择的情况，则选第一个能走的，直接走
        for (int j = 0; j < NUM_OF_CITIES; j++) {
          if (ant[i].visited[j] == 0) {
            ant[i].route[ant[i].currentsteps + 1] = j;
            ant[i].currentsteps++;
            ant[i].visited[j] = 1;
          }
        }
      }
      //int lastcity = -1;
      //double lower_bound[NUM_OF_CITIES] = { 0 };
      //double upper_bound[NUM_OF_CITIES] = { 0 };
      //bool firsttime = true;
      ////给每个城市分配随机数中奖区间
      //for (int j = 0; j < NUM_OF_CITIES; j++) {
      //  if (ant[i].visited[j]) {
      //    continue;
      //  }
      //  if (firsttime) {
      //    lower_bound[j] = 0;
      //    upper_bound[j] = lower_bound[j] + (decision_calculation[j] / decision_sum) * RANDOM_RANGE;
      //    firsttime = false;
      //    lastcity = j;
      //  } else {
      //    lower_bound[j] = upper_bound[lastcity];
      //    upper_bound[j] = lower_bound[j] + (decision_calculation[j] / decision_sum) * RANDOM_RANGE;
      //    lastcity = j;
      //  }
      //}
      ////看哪个城市中奖，然后走
      //int lastj = -10;
      //bool ifcanselect = false;
      //for (int j = 0; j < NUM_OF_CITIES; j++) {
      //  if (ant[i].visited[j]) {
      //    continue;
      //  }
      //  if (lower_bound[j] <= random && upper_bound[j] > random) {
      //    ant[i].route[ant[i].currentsteps + 1] = j;
      //    ant[i].currentsteps++;
      //    ant[i].visited[j] = 1;
      //    ifcanselect = true;
      //    break;
      //  }
      //  lastj = j;
      //}
      //if (!ifcanselect) {
      //  ant[i].route[ant[i].currentsteps + 1] = lastj;
      //  ant[i].currentsteps++;
      //  ant[i].visited[lastj] = 1;

      //}
    }
  }
}

//
//更新信息素
//
void UpdatePheromone() {
  //存放蚂蚁这圈，在城市间增加的信息素
  double tmp_pheromone[NUM_OF_CITIES][NUM_OF_CITIES];
  memset(tmp_pheromone, 0, sizeof(tmp_pheromone));
  for (int i = 0; i < POPULATION; i++) {
    //计算总距离
    for (int j = 0; j < NUM_OF_CITIES - 1; j++) {
      ant[i].distance_sum += distance[ant[i].route[j]][ant[i].route[j + 1]];
    }
    ant[i].distance_sum += distance[ant[i].route[0]][ant[i].route[NUM_OF_CITIES - 1]];
    //根据总距离更新信息素
    for (int j = 0; j < NUM_OF_CITIES - 1; j++) {
      tmp_pheromone[ant[i].route[j]][ant[i].route[j + 1]] += Q_VALUE / ant[i].distance_sum;
      tmp_pheromone[ant[i].route[j + 1]][ant[i].route[j]] += Q_VALUE / ant[i].distance_sum;
      /*pheromone[ant[i].route[j]][ant[i].route[j + 1]] = (1 - VOLATILIZATION) * pheromone[ant[i].route[j]][ant[i].route[j + 1]];
      pheromone[ant[i].route[j + 1]][ant[i].route[j]] = (1 - VOLATILIZATION) * pheromone[ant[i].route[j + 1]][ant[i].route[j]];
      pheromone[ant[i].route[j]][ant[i].route[j + 1]] += Q_VALUE / ant[i].distance_sum;
      pheromone[ant[i].route[j + 1]][ant[i].route[j]] += Q_VALUE / ant[i].distance_sum;*/
    }
    tmp_pheromone[ant[i].route[0]][ant[i].route[NUM_OF_CITIES - 1]] += Q_VALUE / ant[i].distance_sum;
    tmp_pheromone[ant[i].route[NUM_OF_CITIES - 1]][ant[i].route[0]] += Q_VALUE / ant[i].distance_sum;
    /*pheromone[ant[i].route[0]][ant[i].route[NUM_OF_CITIES - 1]] = (1 - VOLATILIZATION) * pheromone[ant[i].route[0]][ant[i].route[NUM_OF_CITIES - 1]];
    pheromone[ant[i].route[NUM_OF_CITIES - 1]][ant[i].route[0]] = (1 - VOLATILIZATION) * pheromone[ant[i].route[NUM_OF_CITIES - 1]][ant[i].route[0]];
    pheromone[ant[i].route[0]][ant[i].route[NUM_OF_CITIES - 1]] += Q_VALUE / ant[i].distance_sum;
    pheromone[ant[i].route[NUM_OF_CITIES - 1]][ant[i].route[0]] += Q_VALUE / ant[i].distance_sum;*/
  }
  //更新信息素，原信息素 * 衰减 + 蚂蚁走一圈生成的
  for (int i = 0; i < NUM_OF_CITIES; i++) {
    for (int j = 0; j < NUM_OF_CITIES; j++) {
      pheromone[i][j] = (1 - VOLATILIZATION) * pheromone[i][j] + tmp_pheromone[i][j];
      pheromone[j][i] = pheromone[i][j];
    }
  }

}

//
////初始化蚂蚁，准备下一轮迭代
//
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
  GetTestDataFromFile();  //文件中读TSP数据
  int count = 0;

  Init();
  double mindistance = 0xFFFFFF;
  while (count <= MAX_ITERATION_TIMES) {
    AllAntsMoveAround();
    UpdatePheromone();


    for (int i = 0; i < POPULATION; i++) {
      if (ant[i].distance_sum < mindistance) {
        mindistance = ant[i].distance_sum;
      }
    }
    std::cout << count << ":  " << mindistance << std::endl;

    InitAntsForNextIteration();
    count++;
  }
  return 0;
}