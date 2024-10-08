#pragma once
#include <string>
using namespace std;
constexpr int NonNum = 67;      // ���ս������
constexpr int ReserveNum = 43;  // ����������
constexpr int ProductNum = 104; // ����ʽ����
constexpr int maxloop = 40;     // ��follow�������ѭ������
constexpr int maxlen = 20;      // һ�����ռ�������ж��ٸ��ռ���
typedef struct first
{
    int num; // �÷��ս����first����Ԫ�ظ���
    string ptr[maxlen];
    bool flag[ReserveNum + 1]; // ��֤�������ظ�Ԫ�أ���һ��$��λ��
} first;

typedef struct follow
{
    int num;                   // �÷��ս����follow����Ԫ�ظ���
    string ptr[maxlen];        // ָ������ս��
    bool flag[ReserveNum + 1]; // ��֤�������ظ�Ԫ�أ���һ��$��λ��
} follow;

typedef struct predict
{
    int num;
    string ptr[maxlen];
    bool flag[ReserveNum + 1];
} predict;

typedef struct production // ����ʽ����
{
    string left;      // ����ʽ��
    string right[10]; // ����ʽ�Ҳ�
} production;

int nonterminalSymbolIndex(const string &s);
int terminalSymbolIndex(const string &s);
void calFirst(string &s);
void calFollow(const string &s, bool *flag);
void outFitstFollow();
void calPredict();
void outPredict();
