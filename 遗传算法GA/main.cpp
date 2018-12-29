#include <iostream>
#include <random>
#include <time.h>
#include <Windows.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <fstream>

//Ҫ�򿪵�tsp�����ļ���·������ʽΪÿ��������꣬�����ʽ�����������Դ�github���������ļ�������
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

#define POW_VALUE 10  //������Ӧ��ʱ��ָ����С
#define SHOW_1_RESULT_PER_AMOUNT 100  //ÿ���ٴε�����ʾһ�ν��

#define NUM_OF_CITIES 48  //���и���
#define POPULATION 500     //��Ⱥ����
#define RANDOM_RANGE 1000000    //��������������Χ�����Բ��ù�
#define CROSS_POSIBILITY 0.90  //�������
#define MUTATE_POSIBILITY 0.10  //�������

#define CROSS_LENGTH 20  //���ù̶����ȶν���ʱ�Ľ���γ���

#define MAX_ITERATION_TIMES 1000000  //����������

//��������
typedef struct {
  int x, y;
}City;

//��ʾһ������
typedef struct {
  int gene[NUM_OF_CITIES];  //���뷽ʽΪ���б�����У��� {0, 1, 2, 3, 4}
  double fitness;          //��Ӧ��
  double ifselect_upper;  //�ϸ�������̶ĵ���
  double ifselect_lower;  //С�ڻ�������̶ĵ���
}TSP;

//**************************************
//global variables start here
//**************************************

bool iffirsttime = true;  //�Ƿ��ǵ�һ�ε�����ûɶ�ã������ڵ�һ�ε�ֵ������ʱ���³�ʼ����

City cities[NUM_OF_CITIES];  //����
double distance[NUM_OF_CITIES][NUM_OF_CITIES];  //���о������

TSP father_generation[POPULATION];  //��������
TSP next_generation[POPULATION];   //�Ӵ����壨ʵ���ϱ��������������ѣ���Ҫ�����ڸ����У�

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
//��ʼ��������󣬺͸���Ļ���
//
void Init() {
  //��ʼ����������±��0��ʼ����n-1
  for (int i = 0; i < NUM_OF_CITIES; i++) {
    for (int j = 0; j < NUM_OF_CITIES; j++) {
      distance[i][j] = sqrt((cities[i].x - cities[j].x) * (cities[i].x - cities[j].x)
        + (cities[i].y - cities[j].y) * (cities[i].y - cities[j].y));
    }
  }
  //��ʼ��ÿ������Ļ�����0��ͷ����������
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
//����Ⱥ����ÿ���������Ӧ�ȣ����õ�Ĭ�Ϲ�ʽ����ָ��
//
void CalcFitness(TSP *individual) {
  double fitness;
  for (int i = 0; i < POPULATION; i++) {
    fitness = 0;
    for (int j = 0; j < NUM_OF_CITIES - 1; j++) {
      fitness += distance[(individual + i)->gene[j]][(individual + i)->gene[j + 1]];
    }
    fitness += distance[(individual + i)->gene[0]][(individual + i)->gene[NUM_OF_CITIES - 1]];
    //��������Ϊ�����������������죬�ͼ��˸�ָ����ȥ����ͨ�Ŵ��㷨��ָ��Ϊ1�����
    fitness = pow(fitness, POW_VALUE);
    fitness = 1.0 / fitness;
    father_generation[i].fitness = fitness;
  }
}

//
//ѡ�񣬴Ӹ���������ѡ�񣬱��浽�Ӵ�����
//�����̶ķ�ʽ��������ѡ��
//
void Select() {
  double fitness_sum = 0;
  for (int i = 0; i < POPULATION; i++) {
    fitness_sum += father_generation[i].fitness;
  }
  //�����̶ķ��������
  father_generation[0].ifselect_lower = 0;
  father_generation[0].ifselect_upper = RANDOM_RANGE * father_generation[0].fitness / fitness_sum;
  for (int i = 1; i < POPULATION; i++) {
    father_generation[i].ifselect_lower = father_generation[i - 1].ifselect_upper;
    father_generation[i].ifselect_upper = father_generation[i].ifselect_lower + RANDOM_RANGE * father_generation[i].fitness / fitness_sum;
  }
  //����n�������
  for (int i = 0; i < POPULATION; i++) {
    int randomnum = Random(0, RANDOM_RANGE);
    for (int j = 0; j < POPULATION; j++) {
      //������̶�ѡ���ˣ�����������ӵ��Ӵ��У��������̶�ѡ��һ��
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
//���溯��1��ʧ�ܵĽ��溯�������õĽ���λ�ý��淨��Alternating Position Crossover��APX��
//��ʵ֤��������淽�����Ƕ��ȣ���Ϊ�������������λ��򰡡�������ȫ�����ɶ����ô��
//�����е��õ��������Cross2()����
//
void Cross() {
  //������ʣ��ж��Ƿ񽻲�
  double cross_possibility = CROSS_POSIBILITY;
  for (int i = 0; i < POPULATION; i++) {
    int randomnum = Random(0, RANDOM_RANGE);
    if (randomnum > cross_possibility * RANDOM_RANGE) {
      continue;
      //���棬�õĽ���λ�ý��淨��Alternating Position Crossover��APX��
      //�����˼�ǣ�����1��a1 a2 a3 a4 a5
      //           ����2��b1 b2 b3 b4 b5
      //Ȼ����a1->b1->a2->b2->a3......��˳�����ΰ�һ������ѡ�������DNA����������ظ���������������һ��
    } else {
      int father1 = Random(0, POPULATION - 1);
      int father2 = Random(0, POPULATION - 1);
      int exist[NUM_OF_CITIES] = { 0 };
      int count = 0;  //count == n - 1ʱ����
      for (int j = 1; true; j++) {
        if (exist[next_generation[father1].gene[j]]) {
          ;  // ������ڣ�ֱ����һ��
        } else {  //��������ڣ��򸽼ӵ�������
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
//���溯��2����ȡ�˽�ѵ������õ������λ���Ľ��档��
//���泤���Ѿ����������ѡ�񽻲���㣬����1�Ļ���͸���2�Ļ���������Ͻ�����
//Ȼ����������Ͱ��ո���1������������ͻ����ѡ������һ����С�Ĳ���ͻ�ĳ���
//
void Cross2() {
  //�������
  double cross_possibility = CROSS_POSIBILITY;
  for (int i = 0; i < POPULATION; i++) {
    int randomnum = Random(0, RANDOM_RANGE);
    if (randomnum > cross_possibility * RANDOM_RANGE) {
      continue;
      //���棬�����Ƿ����ӵ����������Թ̶��������㽻��
    } else {
      int father1 = Random(0, POPULATION - 1);
      int father2 = Random(0, POPULATION - 1);
      //ָ��һ���̶����ȵ�һ�Σ���ʼλ�����������������佻��
      int exist[NUM_OF_CITIES] = { 0 };
      //pos:��ʼλ��
      int pos = Random(1, NUM_OF_CITIES - 1 - (CROSS_LENGTH - 1));

      //����ָ�����Ƕλ���
      for (int j = pos; j < pos + CROSS_LENGTH; j++) {
        int tmp = next_generation[father2].gene[j];
        father_generation[i].gene[j] = tmp;
        exist[tmp] = 1;
      }
      //�����Ļ��򣬰�����������ͻ����һ������ͻ�ģ���ô����
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
//ͻ�亯��
//����ͻ������У������ѡ����λ�ã���������
//
void Mutate() {
  //ͻ����ʣ��ж��Ƿ����ͻ��
  double mutate_possibility = MUTATE_POSIBILITY;
  for (int i = 0; i < POPULATION; i++) {
    int randomnum = Random(0, RANDOM_RANGE);
    if (randomnum > mutate_possibility * RANDOM_RANGE) {
      continue;
    } else {
      //�������λ��
      int pos1 = Random(1, NUM_OF_CITIES - 1);
      int pos2 = Random(1, NUM_OF_CITIES - 1);
      //����
      int tmp = father_generation[i].gene[pos1];
      father_generation[i].gene[pos1] = father_generation[i].gene[pos2];
      father_generation[i].gene[pos2] = tmp;
    }
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
  GetTestDataFromFile();

  Init();

  int count = 0;
  while (count < MAX_ITERATION_TIMES) {
    while (true) {
      CalcFitness(father_generation);
      if (!iffirsttime) {
        int lastmaxpos = 0;
        //countΪ�趨ֵ�ı���ʱ�������������̨
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