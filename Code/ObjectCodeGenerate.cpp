#include <iostream>
#include <fstream>
#include <string>
#include "ObjectCodeGenerate.h"
using namespace std;
ofstream outOC("D:\\vscode_program\\C++\\SnlCompiler\\Output\\objectcode\\objectcode.txt");
int line = 0;
int tempVarNumber = 0;
extern vector<vector<SymbTable>> scope;

void searchInDisplay(treenode *varNode, int regNumber, int choose)
{
    int level, off = 0;
    const int scope_size = scope.size();
    for (int ix = 0; ix < scope_size; ix++)
    {
        const int scope_size_2 = scope[ix].size();
        for (int jx = 0; jx < scope_size_2; jx++)
        {
            if (scope[ix][jx].idname == varNode->token->Sem)
            {
                level = scope[ix][jx].attrIR.More.VarAttr.level;
                off += scope[ix][jx].attrIR.More.VarAttr.off;
            }
        }
    }
    if (choose == 0)
    {
        outOC << "  " << line++ << ":LDC " << regNumber << "," << level << ",0" << endl;
        outOC << "  " << line++ << ":LD " << regNumber << ",0," << regNumber << endl;
        outOC << "  " << line++ << ":LD " << regNumber << "," << off + 8 << "," << regNumber << endl;
    }
    else if (choose == 1)
    {
        outOC << "  " << line++ << ":LDC 3," << level << ",0" << endl;
        outOC << "  " << line++ << ":LD 3,0,3" << endl;
        outOC << "  " << line++ << ":LDA 3," << off + 8 - 1 << ",3" << endl;
        outOC << "  " << line++ << ":ADD " << regNumber << ",3," << regNumber << endl;
        outOC << "  " << line++ << ":LD " << regNumber << ",0," << regNumber << endl;
        outOC << "  " << line++ << ":SUB 3,3,3" << endl;
    }
}

void putInDisplay(treenode *varNode, int choose)
{
    int level, off = 0;
    const int scope_size = scope.size();
    for (int ix = 0; ix < scope_size; ix++)
    {
        const int scope_size_2 = scope[ix].size();
        for (int jx = 0; jx < scope_size_2; jx++)
        {
            if (scope[ix][jx].idname == varNode->token->Sem)
            {
                level = scope[ix][jx].attrIR.More.VarAttr.level;
                off += scope[ix][jx].attrIR.More.VarAttr.off;
            }
        }
    }
    if (choose == 0)
    {
        outOC << "  " << line++ << ":LDC 3," << level << ",0" << endl;
        outOC << "  " << line++ << ":LD 3,0,3" << endl;
        outOC << "  " << line++ << ":ST 0," << off + 8 << ",3" << endl;
    }
    else if (choose == 1)
    {
        outOC << "  " << line++ << ":LDC 3," << level << ",0" << endl;
        outOC << "  " << line++ << ":LD 3,0,3" << endl;
        outOC << "  " << line++ << ":LDA 3," << off + 8 - 1 << ",3" << endl;
        outOC << "  " << line++ << ":ADD 3,3,0" << endl;
        outOC << "  " << line++ << ":ST 1,0,3" << endl;
    }
    outOC << "  " << line++ << ":SUB 3,3,3" << endl;
}

void genProc(treenode *procDec) // 过程声明语法树节点代码生成函
{

    treenode *declarePart = procDec->child[6]->child[0];
    if (declarePart->child[2] != nullptr)
        genProc(declarePart->child[2]->child[0]);
    const int scope_size = scope.size();
    for (int ix = 0; ix < scope_size; ix++)
    {
        const int scope_size_2 = scope[ix].size();
        for (int jx = 0; jx < scope_size_2; jx++)
        {
            if (scope[ix][jx].idname == procDec->child[1]->token->Sem)
            {
                scope[ix][jx].attrIR.More.ProcAttr.code = line;
            }
        }
    }
    // 函数没有return语句
    genStmt(procDec->child[7]->child[0]->child[1]);
    // 动态链指针  返回地址  过程层数  机器状态（0，1，2，4）  返回值 形参变量区  局部变量区  上个同层函数sp位置
    outOC << "  " << line++ << ":LD 1,1,6" << endl;
    outOC << "  " << line++ << ":ST 1," << 1023 - tempVarNumber << ",3" << endl;
    tempVarNumber++;
    outOC << "  " << line++ << ":LD 0,3,6" << endl;
    outOC << "  " << line++ << ":LD 1,4,6" << endl;
    outOC << "  " << line++ << ":LD 2,5,6" << endl;
    outOC << "  " << line++ << ":LD 4,6,6" << endl;

    outOC << "  " << line++ << ":LDA 5,0,6" << endl;
    outOC << "  " << line++ << ":LD 6,0,5" << endl;

    outOC << "  " << line++ << ":SUB 3,3,3" << endl;
    tempVarNumber--;
    outOC << "  " << line++ << ":LD 3," << 1023 - tempVarNumber << ",3" << endl;
    outOC << "  " << line++ << ":JNE 7,0,3" << endl;

    if (procDec->child[8] != nullptr)
        genProc(procDec->child[8]->child[0]);
}

void ExpG(treenode *tree, int regNumber, int choose)
{
    if (tree->str == "Exp") // 这里是一个表达式
    {
        if (tree->child[1] == nullptr) // 但是这个表达式只有一个因子
        {
            // tempVarNumber++;
            ExpG(tree->child[0], regNumber, choose);
        }
        else // 这个表达式有两个因子，要进行运算
        {
            outOC << "  " << line++ << ":ST 2," << 1023 - tempVarNumber << ",3" << endl;
            tempVarNumber++;
            outOC << "  " << line++ << ":ST 0," << 1023 - tempVarNumber << ",3" << endl;
            tempVarNumber++;
            outOC << "  " << line++ << ":ST 1," << 1023 - tempVarNumber << ",3" << endl;
            tempVarNumber++;
            if (choose == 0)
            {
                if (tree->child[1]->child[0]->child[0]->token->Lex == PLUS)
                {
                    ExpG(tree->child[0], 0, choose);
                    ExpG(tree->child[1]->child[1], 1, choose);
                    outOC << "  " << line++ << ":ADD 2,0,1" << endl;
                }
                else if (tree->child[1]->child[0]->child[0]->token->Lex == MINUS)
                {
                    ExpG(tree->child[0], 0, 1 - choose);
                    ExpG(tree->child[1]->child[1], 1, 1 - choose);
                    outOC << "  " << line++ << ":SUB 2,0,1" << endl;
                }
            }
            else
            {
                if (tree->child[1]->child[0]->child[0]->token->Lex == PLUS)
                {
                    ExpG(tree->child[0], 0, 1 - choose);
                    ExpG(tree->child[1]->child[1], 1, 1 - choose);
                    outOC << "  " << line++ << ":SUB 2,0,1" << endl;
                }
                else if (tree->child[1]->child[0]->child[0]->token->Lex == MINUS)
                {
                    ExpG(tree->child[0], 0, choose);
                    ExpG(tree->child[1]->child[1], 1, choose);
                    outOC << "  " << line++ << ":ADD 2,0,1" << endl;
                }
            }

            tempVarNumber--;
            outOC << "  " << line++ << ":LD 1," << 1023 - tempVarNumber << ",3" << endl;
            tempVarNumber--;
            outOC << "  " << line++ << ":LD 0," << 1023 - tempVarNumber << ",3" << endl;
            outOC << "  " << line++ << ":LDA " << regNumber << ",0,2" << endl;
            tempVarNumber--;
            outOC << "  " << line++ << ":LD 2," << 1023 - tempVarNumber << ",3" << endl;
        }
    }
    else if (tree->str == "Term") // 这里是得到表达式中的一个因子
    {
        if (tree->child[1] == nullptr) // 这个是一个数的情况
        {
            treenode *factorNode = tree->child[0];
            if (factorNode->child[0]->str == "Variable") // 变量
            {
                if (factorNode->child[0]->child[1] != nullptr)
                {
                    // 这里只考虑了数组，没有考虑结构体
                    //  array

                    ExpG(factorNode->child[0]->child[1]->child[1], regNumber, 0);

                    searchInDisplay(factorNode->child[0]->child[0], regNumber, 1);
                }
                else
                {
                    // 这里没有考虑数组
                    searchInDisplay(factorNode->child[0]->child[0], regNumber);
                }
            }
            else // 常量
            {
                // 这里只考虑了int类型
                int temp = atoi(factorNode->child[0]->token->Sem.c_str());
                outOC << "  " << line++ << ":LDC " << regNumber << "," << temp << ",0" << endl;
            }
        }
        else // 这里是两个数，但是运算为乘法或者除法，这就导致优先级高，所以把这个看作一个数
        {
            outOC << "  " << line++ << ":ST 2," << 1023 - tempVarNumber << ",3" << endl;
            tempVarNumber++;
            outOC << "  " << line++ << ":ST 0," << 1023 - tempVarNumber << ",3" << endl;
            tempVarNumber++;
            outOC << "  " << line++ << ":ST 1," << 1023 - tempVarNumber << ",3" << endl;
            tempVarNumber++;

            ExpG(tree->child[0], 0, choose);
            ExpG(tree->child[1]->child[1], 1, choose);
            if (tree->child[1]->child[0]->child[0]->token->Lex == TIMES)
            {
                outOC << "  " << line++ << ":MUL 2,0,1" << endl;
            }
            else if (tree->child[1]->child[0]->child[0]->token->Lex == OVER)
            {
                outOC << "  " << line++ << ":DIV 2,0,1" << endl;
            }
            tempVarNumber--;
            outOC << "  " << line++ << ":LD 1," << 1023 - tempVarNumber << ",3" << endl;
            tempVarNumber--;
            outOC << "  " << line++ << ":LD 0," << 1023 - tempVarNumber << ",3" << endl;
            outOC << "  " << line++ << ":LDA " << regNumber << ",0,2" << endl;
            tempVarNumber--;
            outOC << "  " << line++ << ":LD 2," << 1023 - tempVarNumber << ",3" << endl;
        }
    }
    else if (tree->str == "Factor")
    {
        treenode *factorNode = tree;
        if (factorNode->child[0]->str == "Variable") // 变量
        {
            // 这里没有考虑数组
            searchInDisplay(factorNode->child[0]->child[0], regNumber);
        }
        else // 常量
        {
            // 这里只考虑了int类型
            int temp = atoi(factorNode->child[0]->token->Sem.c_str());
            outOC << "  " << line++ << ":LDC " << regNumber << "," << temp << ",0" << endl;
        }
    }
}

void parameterDeliver(treenode *actParamList, int off)
{
    outOC << "  " << line++ << ":ST 0," << 1023 - tempVarNumber << ",3" << endl;
    tempVarNumber++;
    outOC << "  " << line++ << ":ST 1," << 1023 - tempVarNumber << ",3" << endl;
    tempVarNumber++;
    ExpG(actParamList->child[0], 0, 0);
    outOC << "  " << line++ << ":LDA 1," << off << ",5" << endl;
    outOC << "  " << line++ << ":ST 0,0,1" << endl;
    tempVarNumber--;
    outOC << "  " << line++ << ":LD 1," << 1023 - tempVarNumber << ",3" << endl;
    tempVarNumber--;
    outOC << "  " << line++ << ":LD 0," << 1023 - tempVarNumber << ",3" << endl;
    if (actParamList->child[1] != nullptr)
    {
        parameterDeliver(actParamList->child[1]->child[1], off + 1);
    }
}

int findLevel(treenode *fun)
{
    int level;
    const int scope_size = scope.size();
    for (int ix = 0; ix < scope_size; ix++)
    {
        const int scope_size_2 = scope[ix].size();
        for (int jx = 0; jx < scope_size_2; jx++)
        {
            if (scope[ix][jx].idname == fun->token->Sem)
            {
                level = scope[ix][jx].attrIR.More.VarAttr.level; // fun 的level好想不太对
                return level + 1;
            }
        }
    }
}

int findFunc(treenode *fun)
{
    const int scope_size = scope.size();
    for (int ix = 0; ix < scope_size; ix++)
    {
        const int scope_size_2 = scope[ix].size();
        for (int jx = 0; jx < scope_size_2; jx++)
        {
            if (scope[ix][jx].idname == fun->token->Sem)
            {
                return scope[ix][jx].attrIR.More.ProcAttr.code;
            }
        }
    }
}

void AssCall(treenode *Stm)
{
    // 赋值语句  AssignmentRest
    if (Stm->child[1]->child[0]->str == "AssignmentRest")
    {
        // 找到左边变量的位置     普通变量  数组  结构体
        // 普通变量
        if (Stm->child[1]->child[0]->childnum == 2)
        {

            // 得到右边表达式的值
            outOC << "  " << line++ << ":ST 0," << 1023 - tempVarNumber << ",3" << endl;
            tempVarNumber++;
            ExpG(Stm->child[1]->child[0]->child[1], 0, 0);

            putInDisplay(Stm->child[0], 0);

            tempVarNumber--;
            outOC << "  " << line++ << ":LD 0," << 1023 - tempVarNumber << ",3" << endl;
        } // 数组或结构体
        else
        {

            if (Stm->child[1]->child[0]->child[0]->childnum == 2)
            { // 结构体
                // 先不写
                ExpG(Stm->child[1]->child[0]->child[2], 0, 0);
            }
            else if (Stm->child[1]->child[0]->child[0]->childnum == 3)
            { // array
                outOC << "  " << line++ << ":ST 0," << 1023 - tempVarNumber << ",3" << endl;
                tempVarNumber++;
                outOC << "  " << line++ << ":ST 1," << 1023 - tempVarNumber << ",3" << endl;
                tempVarNumber++;
                ExpG(Stm->child[1]->child[0]->child[0]->child[1], 0, 0);
                ExpG(Stm->child[1]->child[0]->child[2], 1, 0);

                putInDisplay(Stm->child[0], 1);
                tempVarNumber--;
                outOC << "  " << line++ << ":LD 1," << 1023 - tempVarNumber << ",3" << endl;
                tempVarNumber--;
                outOC << "  " << line++ << ":LD 0," << 1023 - tempVarNumber << ",3" << endl;
            }
            // 得到右边表达式的值
        }
    }
    // 过程调用语句  CallStmRest
    if (Stm->child[1]->child[0]->str == "CallStmRest")
    {

        int index = findFunc(Stm->child[0]); // z

        // 首先传递参数  这里没有考虑indir（其实这个代码编写就没有考虑）
        parameterDeliver(Stm->child[1]->child[0]->child[1], 8);
        // 1、填写机器状态
        outOC << "  " << line++ << ":ST 0,3,5" << endl;
        outOC << "  " << line++ << ":ST 1,4,5" << endl;
        outOC << "  " << line++ << ":ST 2,5,5" << endl;
        outOC << "  " << line++ << ":ST 4,6,5" << endl;

        // 2、填写管理信息
        // 2.1、过程层数 任何函数都不用填写，，但是占个位置吧
        int level = findLevel(Stm->child[0]);
        // 2.2、动态连指针
        outOC << "  " << line++ << ":ST 6,0,5" << endl;

        // 2.3  返回地址
        outOC << "  " << line++ << ":ST 0," << 1023 - tempVarNumber << ",3" << endl;
        tempVarNumber++;
        outOC << "  " << line++ << ":LDC 0," << level + 1 << ",0" << endl;
        outOC << "  " << line++ << ":ST 0,2,5" << endl;
        int size = findFuncSize(Stm->child[0]);
        outOC << "  " << line++ << ":LD 0," << level + 1 << ",3" << endl;
        outOC << "  " << line++ << ":ST 0," << size + 8 << ",5" << endl;
        // 这里要将新的AR地址放到全局display中
        outOC << "  " << line++ << ":LDC 3," << level + 1 << ",0" << endl;
        outOC << "  " << line++ << ":ST 5,0,3" << endl;
        outOC << "  " << line++ << ":SUB 3,3,3" << endl;

        outOC << "  " << line++ << ":LDA 0,5,7" << endl;
        outOC << "  " << line++ << ":ST 0,1,5" << endl;
        tempVarNumber--;
        outOC << "  " << line++ << ":LD 0," << 1023 - tempVarNumber << ",3" << endl;
        // 2.4、改变sp和top      //sp指向主函数AR低，top指向AR高处
        // 动态链指针  返回地址  过程层数  机器状态（0，1，2，4）  返回值 形参变量区  局部变量区  上个同层函数sp位置
        outOC << "  " << line++ << ":LDA 6,0,5" << endl;
        outOC << "  " << line++ << ":LDA 5," << size + 9 << ",5" << endl;
        // 2.6  生成跳转函数指令  主函数不用生成
        outOC << "  " << line++ << ":JEQ 3," << index << ",3" << endl;
        outOC << "  " << line++ << ":SUB 3,3,3" << endl;
    }
}

void conditionalStmG(treenode *tree)
{
    // E
    outOC << "  " << line++ << ":ST 0," << 1023 - tempVarNumber << ",3" << endl;
    tempVarNumber++;
    outOC << "  " << line++ << ":ST 1," << 1023 - tempVarNumber << ",3" << endl;
    tempVarNumber++;
    outOC << "  " << line++ << ":ST 2," << 1023 - tempVarNumber << ",3" << endl;
    tempVarNumber++;
    ExpG(tree->child[1], 0, 0);
    ExpG(tree->child[3], 1, 0);
    outOC << "  " << line++ << ":SUB 2,0,1" << endl;
    // 把reg2放入栈中
    // fprintf(fpGoal, "  %d:ST 2,%d,3", line++, 1023 - tempVarNumber);
    // fprintf(fpGoal, "  %d:ADD  4, 2, 3\n", line++); // 结果在reg2和reg4中

    int tempLine1 = line++;
    long int tempLine1Ptr = outOC.tellp();
    outOC << "                    " << endl;
    // 必须有else
    // S1
    genStmt(tree->child[5]);
    int tempLine2 = line++;
    long int tempLine2Ptr = outOC.tellp();
    outOC << "                    " << endl;
    long int tempLine21Ptr = outOC.tellp();
    outOC.seekp(tempLine1Ptr, ios::beg);
    // fprintf(fpGoal, "  %d:LD 0,%d,3", line++, 1023 - tempVarNumber);
    if (tree->child[2]->token->Sem == "<")
    {
        outOC << "  " << tempLine1 << ":JGE 2," << line << ",3";
    }
    else if (tree->child[2]->token->Sem == ">")
    {
        outOC << "  " << tempLine1 << ":JLE 2," << line << ",3";
    }
    else if (tree->child[2]->token->Sem == "=")
    {
        outOC << "  " << tempLine1 << ":JNE 2," << line << ",3";
    }
    // S2
    outOC.seekp(tempLine21Ptr, ios::beg);
    genStmt(tree->child[7]);
    long int tempLine3Ptr = outOC.tellp();
    outOC.seekp(tempLine2Ptr, ios::beg);
    outOC << "  " << tempLine2 << ":JEQ 3," << line << ",3";
    outOC.seekp(tempLine3Ptr, ios::beg);

    tempVarNumber--;
    outOC << "  " << line++ << ":LD 2," << 1023 - tempVarNumber << ",3" << endl;
    tempVarNumber--;
    outOC << "  " << line++ << ":LD 1," << 1023 - tempVarNumber << ",3" << endl;
    tempVarNumber--;
    outOC << "  " << line++ << ":LD 0," << 1023 - tempVarNumber << ",3" << endl;
}

void LoopStmG(treenode *tree)
{

    outOC << "  " << line++ << ":ST 0," << 1023 - tempVarNumber << ",3" << endl;
    tempVarNumber++;
    outOC << "  " << line++ << ":ST 1," << 1023 - tempVarNumber << ",3" << endl;
    tempVarNumber++;
    outOC << "  " << line++ << ":ST 2," << 1023 - tempVarNumber << ",3" << endl;
    tempVarNumber++;
    int tempLine0 = line;
    ExpG(tree->child[1], 0, 0);
    ExpG(tree->child[3], 1, 0);
    outOC << "  " << line++ << ":SUB 2,0,1" << endl;

    int tempLine1 = line++;
    long int tempLine1Ptr = outOC.tellp();
    outOC << "                         " << endl;
    // 循环体
    genStmt(tree->child[5]);
    outOC << "  " << line++ << ":JEQ 3," << tempLine0 << ",3" << endl;

    long int tempLine2Ptr = outOC.tellp();
    outOC.seekp(tempLine1Ptr, ios::beg);
    if (tree->child[2]->token->Sem == "<")
    {
        outOC << "  " << tempLine1 << ":JGE 2," << line << ",3";
    }
    else if (tree->child[2]->token->Sem == ">")
    {
        outOC << "  " << tempLine1 << ":JLE 2," << line << ",3";
    }
    else if (tree->child[2]->token->Sem == "=")
    {
        outOC << "  " << tempLine1 << ":JNE 2," << line << ",3";
    }
    outOC.seekp(tempLine2Ptr, ios::beg);
    tempVarNumber--;
    outOC << "  " << line++ << ":LD 2," << 1023 - tempVarNumber << ",3" << endl;
    tempVarNumber--;
    outOC << "  " << line++ << ":LD 1," << 1023 - tempVarNumber << ",3" << endl;
    tempVarNumber--;
    outOC << "  " << line++ << ":LD 0," << 1023 - tempVarNumber << ",3" << endl;
}

void InputStmG(treenode *tree)
{
    // 找到这个变量的位置，并改变它的值

    // 这里没有考虑数组
    outOC << "  " << line++ << ":ST 0," << 1023 - tempVarNumber << ",3" << endl;
    tempVarNumber++;
    outOC << "  " << line++ << ":IN 0,1,1" << endl;
    putInDisplay(tree->child[2], 0);
    tempVarNumber--;
    outOC << "  " << line++ << ":LD 0," << 1023 - tempVarNumber << ",3" << endl;
}

void OutputStmG(treenode *tree)
{
    outOC << "  " << line++ << ":ST 0," << 1023 - tempVarNumber << ",3" << endl;
    tempVarNumber++;
    ExpG(tree->child[2], 0, 0);
    outOC << "  " << line++ << ":OUT 0,0,0" << endl;
    tempVarNumber--;
    outOC << "  " << line++ << ":LD 0," << 1023 - tempVarNumber << ",3" << endl;
}

void genStmt(treenode *stmList) // 语句类型语法树节点代码生成函
{
    treenode *Stm = stmList->child[0];
    if (Stm == nullptr)
        return;
    // 条件语句  ConditionalStm

    if (Stm->childnum == 1 && Stm->child[0]->str == "ConditionalStm")
    {
        conditionalStmG(Stm->child[0]);
    }
    // 循环语句  LoopStm
    if (Stm->childnum == 1 && Stm->child[0]->str == "LoopStm")
    {
        LoopStmG(Stm->child[0]);
    }
    // 赋值语句  AssignmentRest   过程调用语句  CallStmRest
    if (Stm->childnum == 2 && Stm->child[1]->str == "AssCall")
    {
        AssCall(Stm);
    }
    // 读语句   InputStm
    if (Stm->childnum == 1 && Stm->child[0]->str == "InputStm")
    {
        InputStmG(Stm->child[0]);
    }
    // 写语句  OutputStm
    if (Stm->childnum == 1 && Stm->child[0]->str == "OutputStm")
    {
        OutputStmG(Stm->child[0]);
    }
    if (stmList->child[1] != nullptr)
    {
        genStmt(stmList->child[1]->child[1]);
    }
    // 返回语句  ReturnStm
}

int findFuncSize(treenode *fun)
{
    if (fun == nullptr)
    {
        int size = 0;
        const int scope_size = scope.size();
        for (int ix = 0; ix < scope_size; ix++)
        {
            const int scope_size_2 = scope[ix].size();
            for (int jx = 0; jx < scope_size_2; jx++)
            {
                if (scope[ix][jx].attrIR.kind == '1' && scope[ix][jx].attrIR.More.VarAttr.level == 1)
                {
                    size += scope[ix][jx].attrIR.idtype->size;
                }
            }
        }
        return size;
    }
    else
    {
        const int scope_size = scope.size();
        for (int ix = 0; ix < scope_size; ix++)
        {
            const int scope_size_2 = scope[ix].size();
            for (int jx = 0; jx < scope_size_2; jx++)
            {
                if (scope[ix][jx].idname == fun->token->Sem)
                {
                    return scope[ix][jx].attrIR.More.ProcAttr.size;
                }
            }
        }
    }
    return 0;
}

void codeGen(treenode *syntaxTree) // 代码生成器主函数
{
    line = 0;
    treenode *declarePart = syntaxTree->child[1];
    if (declarePart->child[2] != nullptr)
    {

        long int startLine = outOC.tellp();
        outOC << "                     " << endl;
        line = 1;
        genProc(declarePart->child[2]->child[0]);
        long int mainLine = outOC.tellp();
        outOC.seekp(startLine, ios::beg);
        outOC << "  0:JEQ 3," << line << ",3";
        outOC.seekp(mainLine, ios::beg);
    }
    int size = findFuncSize(nullptr);
    outOC << "  " << line++ << ":LDC 4," << 10 + size + 8 << ",0" << endl;
    outOC << "  " << line++ << ":LDC 5," << 10 + size + 9 << ",0" << endl;
    outOC << "  " << line++ << ":LDC 6,10,0" << endl;
    outOC << "  " << line++ << ":SUB 3,3,3" << endl;
    outOC << "  " << line++ << ":ST 6,1,3" << endl;
    genStmt(syntaxTree->child[2]->child[1]);
    outOC.close();
}