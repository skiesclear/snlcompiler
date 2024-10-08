// 主程序，读入样例程序文件进行分析
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LexicalAnalysis.h"
#include "firstFollowPredict.h"
#include "GrammerAnalysis.h"
#include "ObjectCodeGenerate.h"
#include "SemanticAnalysis.h"
int main()
{
    // 词法分析部分
    Token *tokenlisthead = NULL;    // token序列头
    tokenlisthead = getTokenList(); // 计算代码的tokenlist
    printToken(tokenlisthead);      // 将tokenlist输出到本地

    // 语法分析部分
    outFitstFollow();                                 // 计算三个集合并输出到本地
    treenode *RDTreeROOT = RDAnalysis(tokenlisthead); // 递归下降
    printTree(RDTreeROOT);

    // // 进行语义分析
    semanticAnalysis(RDTreeROOT);

    // // 目标代码生成
    codeGen(RDTreeROOT); // 语法树

    return 0;
}