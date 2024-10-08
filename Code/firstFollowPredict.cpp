#include <iostream>
#include <cstring>
#include <fstream>
#include <iomanip>
#include "firstFollowPredict.h"
using namespace std;

// 67个非终结符
string Non_symbol[NonNum] = {
    "Program", "ProgramHead", "ProgramName", "DeclarePart", "TypeDec", "TypeDeclaration",
    "TypeDecList", "TypeDecMore", "TypeId", "TypeName", "BaseType",
    "StructureType", "ArrayType", "Low", "Top", "RecType",
    "FieldDecList", "FieldDecMore", "IdList", "IdMore", "VarDec",
    "VarDeclaration", "VarDecList", "VarDecMore", "VarIdList", "VarIdMore",
    "ProcDec", "ProcDeclaration", "ProcDecMore", "ProcName", "ParamList",
    "ParamDecList", "ParamMore", "Param", "FormList", "FidMore",
    "ProcDecPart", "ProcBody", "ProgramBody", "StmList", "StmMore", "Stm",
    "AssCall", "AssignmentRest", "ConditionalStm", "LoopStm", "InputStm",
    "Invar", "OutputStm", "ReturnStm", "CallStmRest", "ActParamList",
    "ActParamMore", "RelExp", "OtherRelE", "Exp", "OtherTerm",
    "Term", "OtherFactor", "Factor", "Variable", "VariMore", "FieldVar",
    "FieldVarMore", "CmpOp", "AddOp", "MultOp"};
// 43个保留字和其他符号（包括终结符）
string Reserved_word[ReserveNum] = {
    "ENDFILE", "ERROR",
    "PROGRAM", "PROCEDURE", "TYPE", "VAR", "IF",
    "THEN", "ELSE", "FI", "WHILE", "DO", "ENDWH",
    "BEGIN", "END", "READ", "WRITE", "ARRAY", "OF",
    "RECORD", "RETURN",
    "INTEGER", "CHAR",
    "ID", "INTC", "CHARC",
    "ASSIGN", "EQ", "LT", "PLUS", "MINUS",
    "TIMES", "OVER", "LPAREN", "RPAREN", "DOT",
    "COLON", "SEMI", "COMMA", "LMIDPAREN", "RMIDPAREN",
    "UNDERANGE", "$"};
// SNL的104条产生式
production Productions[ProductNum] = {
    {"Program", {"ProgramHead", "DeclarePart", "ProgramBody", "DOT", "0"}},
    {"ProgramHead", {"PROGRAM", "ProgramName", "0"}},
    {"ProgramName", {"ID", "0"}},
    {"DeclarePart", {"TypeDec", "VarDec", "ProcDec", "0"}},
    {"TypeDec", {"$", "0"}},
    {"TypeDec", {"TypeDeclaration", "0"}},
    {"TypeDeclaration", {"TYPE", "TypeDecList", "0"}},
    {"TypeDecList", {"TypeId", "EQ", "TypeName", "SEMI", "TypeDecMore", "0"}},
    {"TypeDecMore", {"$", "0"}},
    {"TypeDecMore", {"TypeDecList", "0"}},
    {"TypeId", {"ID", "0"}},
    {"TypeName", {"BaseType", "0"}},
    {"TypeName", {"StructureType", "0"}},
    {"TypeName", {"ID", "0"}},
    {"BaseType", {"INTEGER", "0"}},
    {"BaseType", {"CHAR", "0"}},
    {"StructureType", {"ArrayType", "0"}},
    {"StructureType", {"RecType", "0"}},
    {"ArrayType", {"ARRAY", "LMIDPAREN", "Low", "UNDERANGE", "Top", "RMIDPAREN", "OF", "BaseType", "0"}},
    {"Low", {"INTC", "0"}},
    {"Top", {"INTC", "0"}},
    {"RecType", {"RECORD", "FieldDecList", "END", "0"}},
    {"FieldDecList", {"BaseType", "IdList", "SEMI", "FieldDecMore", "0"}},
    {"FieldDecList", {"ArrayType", "IdList", "SEMI", "FieldDecMore", "0"}},
    {"FieldDecMore", {"$", "0"}},
    {"FieldDecMore", {"FieldDecList", "0"}},
    {"IdList", {"ID", "IdMore", "0"}},
    {"IdMore", {"$", "0"}},
    {"IdMore", {"COMMA", "IdList", "0"}},
    {"VarDec", {"$", "0"}},
    {"VarDec", {"VarDeclaration", "0"}},
    {"VarDeclaration", {"VAR", "VarDecList", "0"}},
    {"VarDecList", {"TypeName", "VarIdList", "SEMI", "VarDecMore", "0"}},
    {"VarDecMore", {"$", "0"}},
    {"VarDecMore", {"VarDecList", "0"}},
    {"VarIdList", {"ID", "VarIdMore", "0"}},
    {"VarIdMore", {"$", "0"}},
    {"VarIdMore", {"COMMA", "VarIdList", "0"}},
    {"ProcDec", {"$", "0"}},
    {"ProcDec", {"ProcDeclaration", "0"}},
    {"ProcDeclaration", {"PROCEDURE", "ProcName", "LPAREN", "ParamList", "RPAREN", "SEMI", "ProcDecPart", "ProcBody", "ProcDecMore", "0"}},
    {"ProcDecMore", {"$", "0"}},
    {"ProcDecMore", {"ProcDeclaration", "0"}},
    {"ProcName", {"ID", "0"}},
    {"ParamList", {"$", "0"}},
    {"ParamList", {"ParamDecList", "0"}},
    {"ParamDecList", {"Param", "ParamMore", "0"}},
    {"ParamMore", {"$", "0"}},
    {"ParamMore", {"SEMI", "ParamDecList", "0"}},
    {"Param", {"TypeName", "FormList", "0"}},
    {"Param", {"VAR", "TypeName", "FormList", "0"}},
    {"FormList", {"ID", "FidMore", "0"}},
    {"FidMore", {"$", "0"}},
    {"FidMore", {"COMMA", "FormList", "0"}},
    {"ProcDecPart", {"DeclarePart", "0"}},
    {"ProcBody", {"ProgramBody", "0"}},
    {"ProgramBody", {"BEGIN", "StmList", "END", "0"}},
    {"StmList", {"Stm", "StmMore", "0"}},
    {"StmMore", {"$", "0"}},
    {"StmMore", {"SEMI", "StmList", "0"}},
    {"Stm", {"ConditionalStm", "0"}},
    {"Stm", {"LoopStm", "0"}},
    {"Stm", {"InputStm", "0"}},
    {"Stm", {"OutputStm", "0"}},
    {"Stm", {"ReturnStm", "0"}},
    {"Stm", {"ID", "AssCall", "0"}},
    {"AssCall", {"AssignmentRest", "0"}},
    {"AssCall", {"CallStmRest", "0"}},
    {"AssignmentRest", {"VariMore", "ASSIGN", "Exp", "0"}},
    {"ConditionalStm", {"IF", "RelExp", "THEN", "StmList", "ELSE", "StmList", "FI", "0"}},
    {"LoopStm", {"WHILE", "RelExp", "DO", "StmList", "ENDWH", "0"}},
    {"InputStm", {"READ", "LPAREN", "Invar", "RPAREN", "0"}},
    {"Invar", {"ID", "0"}},
    {"OutputStm", {"WRITE", "LPAREN", "Exp", "RPAREN", "0"}},
    {"ReturnStm", {"RETURN", "LPAREN", "Exp", "RPAREN", "0"}},
    {"CallStmRest", {"LPAREN", "ActParamList", "RPAREN", "0"}},
    {"ActParamList", {"$", "0"}},
    {"ActParamList", {"Exp", "ActParamMore", "0"}},
    {"ActParamMore", {"$", "0"}},
    {"ActParamMore", {"COMMA", "ActParamList", "0"}},
    {"RelExp", {"Exp", "OtherRelE", "0"}},
    {"OtherRelE", {"CmpOp", "Exp", "0"}},
    {"Exp", {"Term", "OtherTerm", "0"}},
    {"OtherTerm", {"$", "0"}},
    {"OtherTerm", {"AddOp", "Exp", "0"}},
    {"Term", {"Factor", "OtherFactor", "0"}},
    {"OtherFactor", {"$", "0"}},
    {"OtherFactor", {"MultOp", "Term", "0"}},
    {"Factor", {"LPAREN", "Exp", "RPAREN", "0"}},
    {"Factor", {"INTC", "0"}},
    {"Factor", {"Variable", "0"}},
    {"Variable", {"ID", "VariMore", "0"}},
    {"VariMore", {"$", "0"}},
    {"VariMore", {"LMIDPAREN", "Exp", "RMIDPAREN", "0"}},
    {"VariMore", {"DOT", "FieldVar", "0"}},
    {"FieldVar", {"ID", "FieldVarMore", "0"}},
    {"FieldVarMore", {"$", "0"}},
    {"FieldVarMore", {"LMIDPAREN", "Exp", "RMIDPAREN", "0"}},
    {"CmpOp", {"LT", "0"}},
    {"CmpOp", {"EQ", "0"}},
    {"AddOp", {"PLUS", "0"}},
    {"AddOp", {"MINUS", "0"}},
    {"MultOp", {"TIMES", "0"}},
    {"MultOp", {"OVER", "0"}}};

first firsts[NonNum];
follow follows[NonNum];
predict predicts[ProductNum];

int nonterminalSymbolIndex(const string &s) // 返回非终结符s在非终结符集合中的下标（判断s是否为非终结符）
{
    for (int i = 0; i < NonNum; i++)
        if (s == Non_symbol[i])
            return i;
    return -1;
}

int terminalSymbolIndex(const string &s) // 返回终结符s在终结符集合中的下标
{
    for (int i = 0; i < ReserveNum; i++)
        if (s == Reserved_word[i])
            return i;
    return -1;
}

void calFirst(string &s) // 计算first集
{
    int countEmpty = 0;
    int isEmpty = 0;
    int lenRight = 0;                      // 产生式右部长度（右部符号个数）
    int index = nonterminalSymbolIndex(s); // 非终结符s在集合中的下标
    for (int i = 0; i < ProductNum; i++)   // 遍历每一条产生式
    {
        if (s == Productions[i].left) // 产生式左部匹配上
        {
            for (int j = 0; Productions[i].right[j] != "0"; j++) // 产生式右部以"0"作为结束符
            {
                // 1.right[j]是终结符，直接加入first集
                if (nonterminalSymbolIndex(Productions[i].right[j]) == -1)
                {
                    int NonIndex = nonterminalSymbolIndex(Productions[i].left);                       // 获取当前产生式左部在非终结符集合中的下标
                    if (firsts[NonIndex].flag[terminalSymbolIndex(Productions[i].right[j])] == false) // right[j]未被加入过first集
                    {
                        firsts[NonIndex].ptr[firsts[NonIndex].num] = Productions[i].right[j];
                        firsts[NonIndex].flag[terminalSymbolIndex(Productions[i].right[j])] = true;
                        firsts[NonIndex].num++;
                        break;
                    }
                    else // 已经被加入过first集，直接break
                        break;
                }

                // 2.right[j]是非终结符，则递归求解
                calFirst(Productions[i].right[j]);                                 // 继续算right[j]的first集
                int rightjIndex = nonterminalSymbolIndex(Productions[i].right[j]); // 获得非终结符right[j]在集合中的下标
                for (int k = 0; k < firsts[rightjIndex].num; k++)                  // 将first(right[j])中的非$终结符加入first(s)
                {
                    if (firsts[rightjIndex].ptr[k] == "$") // first(right[j])中是否有空产生式
                        isEmpty = 1;
                    else
                    {
                        if (firsts[index].flag[terminalSymbolIndex(firsts[rightjIndex].ptr[k])] == false) // temp未被加入过first集
                        {
                            firsts[index].ptr[firsts[index].num] = firsts[rightjIndex].ptr[k];
                            firsts[index].flag[terminalSymbolIndex(firsts[rightjIndex].ptr[k])] = true;
                            firsts[index].num++;
                        }
                    }
                }
                if (isEmpty == 0) // right[j]不能为$，迭代结束
                    break;
                else
                {
                    countEmpty += isEmpty;
                    isEmpty = 0;
                }
            }
            for (int j = 0; Productions[i].right[j] != "0"; j++) // 统计产生式右部长度
                lenRight++;
            if (countEmpty == lenRight) // 即右部的所有符号都能推导或就是$，则将$加入到first(s)中
            {
                if (firsts[index].flag[terminalSymbolIndex("$")] == false) // $未被加入过first集
                {
                    firsts[index].ptr[firsts[index].num] = "$";
                    firsts[index].flag[terminalSymbolIndex("$")] = true;
                    firsts[index].num++;
                }
            }
        }
    }
}

void calFollow(const string &s, bool *flag) // 计算follow集
{
    int index = nonterminalSymbolIndex(s); // 非终结符s在集合中的下标
    if (flag[index] == false)              // 标记这个非终结符已经被找过
        flag[index] = true;
    else
        return;
    for (int i = 0; i < ProductNum; i++)
    {
        int lenRight = 0; // 产生式右部长度（右部符号个数）
        int tempindex = -1;

        for (int j = 0; Productions[i].right[j] != "0"; j++) // 统计产生式右部长度
            lenRight++;

        int s_index[20] = {0}; // 记录s在产生式右部出现的所有位置
        int s_index_num = 0;   // 上述数组的尾巴下标（方便索引）

        for (int j = 0; Productions[i].right[j] != "0"; j++)
        {
            if (Productions[i].right[j] == s) // 右部有s，记录在产生式右部的下标
            {
                s_index[s_index_num] = j;
                s_index_num++;
            }
        }

        // 1.存在一个产生式A→αBβ，那么first(β)中除$外的所有符号都在follow(B)中
        for (int j = 0; j < s_index_num; j++)
        {
            tempindex = s_index[j];

            if (tempindex != -1 && tempindex < (lenRight - 1))
            {
                bool hasEmpty = true; // 式β中是否包含$
                while ((tempindex < lenRight - 1) && hasEmpty)
                {
                    tempindex += 1;
                    const string &beta = Productions[i].right[tempindex];
                    if (nonterminalSymbolIndex(beta) == -1) // β是终结符
                    {
                        if (follows[index].flag[terminalSymbolIndex(beta)] == false) // β未被加入过follow集
                        {
                            follows[index].ptr[follows[index].num] = beta;
                            follows[index].flag[terminalSymbolIndex(beta)] = true;
                            follows[index].num++;
                        }
                        hasEmpty = false;
                        break;
                    }
                    else // β是非终结符
                    {
                        hasEmpty = false;
                        int betaindex = nonterminalSymbolIndex(beta);   // beta在非终结符集合中的下标
                        for (int k = 0; k < firsts[betaindex].num; k++) // 将first(β)中所有除了$的符号加入到follow(B)中
                        {
                            if (firsts[betaindex].ptr[k] == "$")
                                hasEmpty = true;
                            else
                            {
                                if (follows[index].flag[terminalSymbolIndex(firsts[betaindex].ptr[k])] == false) // 这个终结符未加入过follow集
                                {
                                    follows[index].ptr[follows[index].num] = firsts[betaindex].ptr[k];
                                    follows[index].flag[terminalSymbolIndex(firsts[betaindex].ptr[k])] = true;
                                    follows[index].num++;
                                }
                            }
                        }
                    }
                }
                // 2.存在一个产生式A→αBβ，且first(β)包含$，那么follow(A)中的所有符号都在follow(B)中
                if (hasEmpty && Productions[i].left != s)
                {
                    calFollow(Productions[i].left, flag);
                    int index_A = nonterminalSymbolIndex(Productions[i].left); // 非终结符A在非终结符集合中的下标
                    for (int k = 0; k < follows[index_A].num; k++)
                    {
                        if (follows[index].flag[terminalSymbolIndex(follows[index_A].ptr[k])] == false) // 即将加入follow(B)的终结符follows[index_A].ptr[k]此前未加入过
                        {
                            follows[index].ptr[follows[index].num] = follows[index_A].ptr[k];
                            follows[index].flag[terminalSymbolIndex(follows[index_A].ptr[k])] = true;
                            follows[index].num++;
                        }
                    }
                }
            }
            // 3.存在一个产生式A→αB，那么follow(A)中的所有符号都在follow(B)中
            else if (tempindex != -1 && tempindex == lenRight - 1 && Productions[i].left != s)
            {
                calFollow(Productions[i].left, flag);
                int index_A = nonterminalSymbolIndex(Productions[i].left); // 非终结符A在非终结符集合中的下标
                for (int k = 0; k < follows[index_A].num; k++)
                {
                    if (follows[index].flag[terminalSymbolIndex(follows[index_A].ptr[k])] == false) // 即将加入follow(B)的终结符follows[index_A].ptr[k]此前未加入过
                    {
                        follows[index].ptr[follows[index].num] = follows[index_A].ptr[k];
                        follows[index].flag[terminalSymbolIndex(follows[index_A].ptr[k])] = true;
                        follows[index].num++;
                    }
                }
            }
        }
    }
}

void outFitstFollow() // 输出first集和follow集到本地
{
    // NonNum 是非终极符的数量
    for (int i = 0; i < NonNum; i++) // 初始化
    {
        firsts[i].num = 0;
        memset(firsts[i].flag, false, sizeof(firsts[i].flag));
        follows[i].num = 0;
        memset(follows[i].flag, false, sizeof(follows[i].flag));
    }

    for (int i = 0; i < NonNum; i++) // 计算所有非终结符的first集
        calFirst(Non_symbol[i]);

    for (int i = 0; i < NonNum; i++) // 计算所有非终结符的follow集
    {
        bool flag[NonNum];
        memset(flag, false, sizeof(flag));
        calFollow(Non_symbol[i], flag);
    }

    ofstream out("D:\\vscode_program\\C++\\SnlCompiler\\Output\\first\\first.txt");
    if (!out.is_open())
    {
        cout << "cannot open the first assemble file" << endl;
        return;
    }
    for (int i = 0; i < NonNum; i++)
    {
        out << Non_symbol[i] << ":  ";
        for (int j = 0; j < firsts[i].num; j++)
            out << firsts[i].ptr[j] << ",";
        out << endl;
    }
    out.close();
    out.open("D:\\vscode_program\\C++\\SnlCompiler\\Output\\follow\\follow.txt");

    if (!out.is_open())
    {
        cout << "cannot open the follow assemble file" << endl;
        return;
    }
    for (int i = 0; i < NonNum; i++)
    {
        out << Non_symbol[i] << ":  ";
        for (int j = 0; j < follows[i].num; j++)
            out << follows[i].ptr[j] << ",";
        out << endl;
    }
    out.close();
    calPredict();
    outPredict();
}

// int predicetSet[NonNum][ReserveNum];
void calPredict() // 计算predict集 - LL1文法
{
    for (int i = 0; i < ProductNum; i++) // 初始化
    {
        predicts[i].num = 0;
        memset(predicts[i].flag, false, sizeof(predicts[i].flag));
    }

    for (int i = 0; i < ProductNum; i++)
    {
        int leftindex = nonterminalSymbolIndex(Productions[i].left); // 左部在非终结符集合中的下标
        int countEmpty = 0;
        // 1.若α不能推出$，则predict(A→α)=first(α)
        for (int j = 0; Productions[i].right[j] != "0"; j++)
        {
            if (nonterminalSymbolIndex(Productions[i].right[j]) == -1) // 右部是终结符（包括空）
            {
                if (Productions[i].right[j] != "$") // 这个终结符不是$
                {
                    if (!predicts[i].flag[terminalSymbolIndex(Productions[i].right[j])])
                    {
                        predicts[i].ptr[predicts[i].num++] = Productions[i].right[j];
                        predicts[i].flag[terminalSymbolIndex(Productions[i].right[j])] = true;
                    }
                }
                else
                    countEmpty++;
                break;
            }
            else // 右部是非终结符
            {
                int index = nonterminalSymbolIndex(Productions[i].right[j]);
                for (int k = 0; k < firsts[index].num; k++)
                {
                    if (!predicts[i].flag[terminalSymbolIndex(firsts[index].ptr[k])])
                    {
                        predicts[i].ptr[predicts[i].num++] = firsts[index].ptr[k];
                        predicts[i].flag[terminalSymbolIndex(firsts[index].ptr[k])] = true;
                    }
                }
                bool haveEmpty = false; // first(index)里有没有$
                for (int k = 0; k < firsts[index].num; k++)
                    if (firsts[index].ptr[k] == "$")
                        haveEmpty = true;
                if (haveEmpty == true) // first(right[j])中有$，继续看α中的下一个符号
                    countEmpty++;
                else
                    break;
            }
        }
        // 2.若α能推出$，则predict(A→α)={first(α)-$}∪follow(A)
        int lenRight = 0;
        for (int j = 0; Productions[i].right[j] != "0"; j++) // 统计产生式右部长度
            lenRight++;
        if (countEmpty == lenRight)
        {
            for (int j = 0; j < follows[leftindex].num; j++)
            {
                if (!predicts[i].flag[terminalSymbolIndex(follows[leftindex].ptr[j])])
                {
                    predicts[i].ptr[predicts[i].num++] = follows[leftindex].ptr[j];
                    predicts[i].flag[terminalSymbolIndex(follows[leftindex].ptr[j])] = true;
                }
            }
        }
    }
}

void outPredict()
{
    ofstream out("D:\\vscode_program\\C++\\SnlCompiler\\Output\\predict\\predict.txt");
    if (!out.is_open())
    {
        cout << "cannot open the predict assemble file" << endl;
        return;
    }
    for (int i = 0; i < ProductNum; i++)
    {
        out << Productions[i].left << ":  ";
        for (int j = 0; j < predicts[i].num; j++)
            out << predicts[i].ptr[j] << ",";
        out << endl;
    }
    out.close();
}
