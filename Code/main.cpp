// �����򣬶������������ļ����з���
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
    // �ʷ���������
    Token *tokenlisthead = NULL;    // token����ͷ
    tokenlisthead = getTokenList(); // ��������tokenlist
    printToken(tokenlisthead);      // ��tokenlist���������

    // �﷨��������
    outFitstFollow();                                 // �����������ϲ����������
    treenode *RDTreeROOT = RDAnalysis(tokenlisthead); // �ݹ��½�
    printTree(RDTreeROOT);

    // // �����������
    semanticAnalysis(RDTreeROOT);

    // // Ŀ���������
    codeGen(RDTreeROOT); // �﷨��

    return 0;
}