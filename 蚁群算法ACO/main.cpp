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

#define RANDOM_RANGE 100000000  //��������������Χ�����Բ��ù�

#define PHEROMONE_INIT_VALUE 1.0  //���м��ʼ������Ϣ��Ũ�ȣ�Ӱ�첻��

#define NUM_OF_CITIES 48  //���и���
#define POPULATION 34     //���ϸ�����mֵ
#define INFLUENCE_OF_PHEROMONE 1  //��ֵ����Ϣ�ص�Ӱ��Ȩ��
#define INFLUENCE_OF_DISTANCE 5   //��ֵ����ǰ�����Ӱ��Ȩ��
#define Q_VALUE 100     //Qֵ��ÿ����������һȦ֮��һ���ͷŵ���Ϣ�ص���
#define VOLATILIZATION 0.5  //(1 - �ѣ��� �� ��ֵ���ӷ�ϵ��

#define MAX_ITERATION_TIMES 1000000  //����������

//��������
typedef struct {
  int x, y;
}City;

//���ϸ���
typedef struct {
  int route[NUM_OF_CITIES];  //·����¼
  int visited[NUM_OF_CITIES];  //�ѷ��ʼ�¼
  int currentsteps;  //�����Ѿ��ߵĲ���

  double distance_sum;  //�����ߵ��ܾ���
}TSP;

//**************************************
//global variables start here
//**************************************��
City cities[NUM_OF_CITIES];  //����

double distance[NUM_OF_CITIES][NUM_OF_CITIES];  //���м����
double pheromone[NUM_OF_CITIES][NUM_OF_CITIES];  //���м���Ϣ��Ũ��

TSP ant[POPULATION];  //��Ⱥ

//
//����[x,y]��Χ���������
//
int Random(int x, int y) {
  std::random_device rd;
  //std::default_random_engine a;
  std::minstd_rand rng{ rd() };
  std::uniform_int_distribution<long> dist{ x, y };
  return dist(rng);
}

//
//��ʼ��������������λ��
//
void Init() {
  //��ʼ����������±��0��ʼ����n-1
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
  //��ʼ������λ��
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
//������������һȦ������·��
//
void AllAntsMoveAround() {
  for (int i = 0; i < POPULATION; i++) {
    //ÿ�����ϵ�ÿ�����������㹻ʱ����ѭ��
    while (ant[i].currentsteps < NUM_OF_CITIES - 1) {
      int current_city = ant[i].route[ant[i].currentsteps];
      double decision_sum = 0;
      double decision_calculation[NUM_OF_CITIES] = { 0 };
      //����ÿ��·���ĸ��ʵĴ�С
      for (int j = 0; j < NUM_OF_CITIES; j++) {
        if (ant[i].visited[j]) {
          continue;
        }
        //��ǰ·������
        decision_calculation[j] = pow(pheromone[current_city][j], INFLUENCE_OF_PHEROMONE)
          * pow(1 / distance[current_city][j], INFLUENCE_OF_DISTANCE);
        //���ʵĺ�
        decision_sum += decision_calculation[j];
      }
      //��ת�Ƹ��ʣ������̶ķ�ʽ��ѡ����һ��
      double random = Random(0, RANDOM_RANGE);
      //---------------------���ĵĲ��֣���ʵ����ע�͵��Ĳ���Ҳ��һ����Ҳû�д���
      bool ifcanselect = false;
      int lastj = -100;
      if (decision_sum > 0) {  //�����������ܴ�ʱ��decision_sum����������0����ʱ����������Ļ�����͹ҵ���
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
      if (!ifcanselect) {  //���decision_sum����0���޷�ѡ����������ѡ��һ�����ߵģ�ֱ����
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
      ////��ÿ�����з���������н�����
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
      ////���ĸ������н���Ȼ����
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
//������Ϣ��
//
void UpdatePheromone() {
  //���������Ȧ���ڳ��м����ӵ���Ϣ��
  double tmp_pheromone[NUM_OF_CITIES][NUM_OF_CITIES];
  memset(tmp_pheromone, 0, sizeof(tmp_pheromone));
  for (int i = 0; i < POPULATION; i++) {
    //�����ܾ���
    for (int j = 0; j < NUM_OF_CITIES - 1; j++) {
      ant[i].distance_sum += distance[ant[i].route[j]][ant[i].route[j + 1]];
    }
    ant[i].distance_sum += distance[ant[i].route[0]][ant[i].route[NUM_OF_CITIES - 1]];
    //�����ܾ��������Ϣ��
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
  //������Ϣ�أ�ԭ��Ϣ�� * ˥�� + ������һȦ���ɵ�
  for (int i = 0; i < NUM_OF_CITIES; i++) {
    for (int j = 0; j < NUM_OF_CITIES; j++) {
      pheromone[i][j] = (1 - VOLATILIZATION) * pheromone[i][j] + tmp_pheromone[i][j];
      pheromone[j][i] = pheromone[i][j];
    }
  }

}

//
////��ʼ�����ϣ�׼����һ�ֵ���
//
void InitAntsForNextIteration() {
  //��ʼ������
     //��ʼ������λ��
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
//���ļ����ݴ���ڴ�
//
void GetTestDataFromFile() {
  errno_t err;
  FILE *fp;
  err = fopen_s(&fp, "C:\\Users\\cky\\Documents\\GitHub\\TSPtestdata.txt", "r");
  if (fp == NULL) {
    printf("open file failed\n");
    return;
  }
  fseek(fp, 0L, SEEK_END);/* ��λ���ļ�ĩβ */  //h.t.t.p.s://blog.csdn.net/jiayanhui2877/article/details/8222882
  int flen = ftell(fp); /* �õ��ļ���С */
  char stringbuffer[100005] = { '\0' };
  fseek(fp, 0L, SEEK_SET); /* ��λ���ļ���ͷ */
  fread(stringbuffer, flen, 1, fp); /* һ���Զ�ȡȫ���ļ����� */
  //TODO:: ����������ˣ��ļ����\r\n�����˻��������ʣ��һ�ֽڵ�\n��
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
  GetTestDataFromFile();  //�ļ��ж�TSP����
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