#pragma once
#include <string>
#include <vector>
#include "LexicalAnalysis.h"
using namespace std;
struct treenode
{
    string str;               // 当前符号
    Token *token;             // token序列的一个结点
    vector<treenode *> child; // NonNum 比 ReserveNum 多
    int childnum;             // 当前节点的子节点数量
    int index;                // 节点编号，方便生成语法树结构图片
    treenode(int &nodeNumber)
    {
        this->token = nullptr;
        this->childnum = 0;
        this->index = nodeNumber++;
    }
    treenode(const string &str, int &nodeNumber)
    {
        this->str = str;
        this->index = nodeNumber++;
        this->childnum = 0;
        this->token = nullptr;
    }
    ~treenode()
    {
        delete this;
    }
    void addChild(treenode *childNode)
    {
        child.push_back(childNode);
        childnum++;
    }
};

treenode *RDAnalysis(Token *tokenhead); // 递归下降分析法，接收token序列头
treenode *matchToken(LexType tok);      // 匹配当前token与终结符，之后移动指针
void printTree(treenode *root);

treenode *program();

treenode *programHead();

treenode *declarePart();

treenode *typeDecPart();
treenode *typeDec();
treenode *typeDecList();
treenode *typeDecMore();
treenode *typeDef();
treenode *baseType();
treenode *structureType();
treenode *arrayType();
treenode *recType();
treenode *fieldDecList();
treenode *fieldDecMore();
treenode *IDList();
treenode *IDMore();

treenode *varDecPart();
treenode *varDec();
treenode *varDecList();
treenode *varDecMore();
treenode *varIDList();
treenode *varIDMore();

treenode *procDecpart();
treenode *procDec();
treenode *paramList();
treenode *paramDecList();
treenode *param();
treenode *paramMore();
treenode *formList();
treenode *fidMore();

treenode *procDecPart();

treenode *procBody();

treenode *programBody();
treenode *stmList();
treenode *stm();
treenode *stmMore();
treenode *conditionalStm();
treenode *loopStm();
treenode *inputStm();
treenode *outputStm();
treenode *returnStm();
treenode *assCall();
treenode *assignmentRest();
treenode *callStmRest();
treenode *variable();
treenode *variMore();
treenode *exp();
treenode *actparamList();
treenode *actparamMore();
treenode *term();
treenode *otherTerm();
treenode *addOp();
treenode *factor();
treenode *otherFactor();
treenode *multOp();
treenode *fieldVar();
treenode *fieldVarMore();