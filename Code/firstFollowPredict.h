#pragma once
#include <string>
using namespace std;
constexpr int NonNum = 67;      // 非终结符数量
constexpr int ReserveNum = 43;  // 保留字数量
constexpr int ProductNum = 104; // 产生式数量
constexpr int maxloop = 40;     // 求follow集的最大循环次数
constexpr int maxlen = 20;      // 一个非终极符最多有多少个终极符
typedef struct first
{
    int num; // 该非终结符的first集中元素个数
    string ptr[maxlen];
    bool flag[ReserveNum + 1]; // 保证集合无重复元素（多一个$的位）
} first;

typedef struct follow
{
    int num;                   // 该非终结符的follow集中元素个数
    string ptr[maxlen];        // 指向各个终结符
    bool flag[ReserveNum + 1]; // 保证集合无重复元素（多一个$的位）
} follow;

typedef struct predict
{
    int num;
    string ptr[maxlen];
    bool flag[ReserveNum + 1];
} predict;

typedef struct production // 产生式类型
{
    string left;      // 产生式左部
    string right[10]; // 产生式右部
} production;

int nonterminalSymbolIndex(const string &s);
int terminalSymbolIndex(const string &s);
void calFirst(string &s);
void calFollow(const string &s, bool *flag);
void outFitstFollow();
void calPredict();
void outPredict();
