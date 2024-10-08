#include <iostream>
#include <cstring>
#include <fstream>
#include <iomanip>
#include "firstFollowPredict.h"
using namespace std;

// 67�����ս��
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
// 43�������ֺ��������ţ������ս����
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
// SNL��104������ʽ
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

int nonterminalSymbolIndex(const string &s) // ���ط��ս��s�ڷ��ս�������е��±꣨�ж�s�Ƿ�Ϊ���ս����
{
    for (int i = 0; i < NonNum; i++)
        if (s == Non_symbol[i])
            return i;
    return -1;
}

int terminalSymbolIndex(const string &s) // �����ս��s���ս�������е��±�
{
    for (int i = 0; i < ReserveNum; i++)
        if (s == Reserved_word[i])
            return i;
    return -1;
}

void calFirst(string &s) // ����first��
{
    int countEmpty = 0;
    int isEmpty = 0;
    int lenRight = 0;                      // ����ʽ�Ҳ����ȣ��Ҳ����Ÿ�����
    int index = nonterminalSymbolIndex(s); // ���ս��s�ڼ����е��±�
    for (int i = 0; i < ProductNum; i++)   // ����ÿһ������ʽ
    {
        if (s == Productions[i].left) // ����ʽ��ƥ����
        {
            for (int j = 0; Productions[i].right[j] != "0"; j++) // ����ʽ�Ҳ���"0"��Ϊ������
            {
                // 1.right[j]���ս����ֱ�Ӽ���first��
                if (nonterminalSymbolIndex(Productions[i].right[j]) == -1)
                {
                    int NonIndex = nonterminalSymbolIndex(Productions[i].left);                       // ��ȡ��ǰ����ʽ���ڷ��ս�������е��±�
                    if (firsts[NonIndex].flag[terminalSymbolIndex(Productions[i].right[j])] == false) // right[j]δ�������first��
                    {
                        firsts[NonIndex].ptr[firsts[NonIndex].num] = Productions[i].right[j];
                        firsts[NonIndex].flag[terminalSymbolIndex(Productions[i].right[j])] = true;
                        firsts[NonIndex].num++;
                        break;
                    }
                    else // �Ѿ��������first����ֱ��break
                        break;
                }

                // 2.right[j]�Ƿ��ս������ݹ����
                calFirst(Productions[i].right[j]);                                 // ������right[j]��first��
                int rightjIndex = nonterminalSymbolIndex(Productions[i].right[j]); // ��÷��ս��right[j]�ڼ����е��±�
                for (int k = 0; k < firsts[rightjIndex].num; k++)                  // ��first(right[j])�еķ�$�ս������first(s)
                {
                    if (firsts[rightjIndex].ptr[k] == "$") // first(right[j])���Ƿ��пղ���ʽ
                        isEmpty = 1;
                    else
                    {
                        if (firsts[index].flag[terminalSymbolIndex(firsts[rightjIndex].ptr[k])] == false) // tempδ�������first��
                        {
                            firsts[index].ptr[firsts[index].num] = firsts[rightjIndex].ptr[k];
                            firsts[index].flag[terminalSymbolIndex(firsts[rightjIndex].ptr[k])] = true;
                            firsts[index].num++;
                        }
                    }
                }
                if (isEmpty == 0) // right[j]����Ϊ$����������
                    break;
                else
                {
                    countEmpty += isEmpty;
                    isEmpty = 0;
                }
            }
            for (int j = 0; Productions[i].right[j] != "0"; j++) // ͳ�Ʋ���ʽ�Ҳ�����
                lenRight++;
            if (countEmpty == lenRight) // ���Ҳ������з��Ŷ����Ƶ������$����$���뵽first(s)��
            {
                if (firsts[index].flag[terminalSymbolIndex("$")] == false) // $δ�������first��
                {
                    firsts[index].ptr[firsts[index].num] = "$";
                    firsts[index].flag[terminalSymbolIndex("$")] = true;
                    firsts[index].num++;
                }
            }
        }
    }
}

void calFollow(const string &s, bool *flag) // ����follow��
{
    int index = nonterminalSymbolIndex(s); // ���ս��s�ڼ����е��±�
    if (flag[index] == false)              // ���������ս���Ѿ����ҹ�
        flag[index] = true;
    else
        return;
    for (int i = 0; i < ProductNum; i++)
    {
        int lenRight = 0; // ����ʽ�Ҳ����ȣ��Ҳ����Ÿ�����
        int tempindex = -1;

        for (int j = 0; Productions[i].right[j] != "0"; j++) // ͳ�Ʋ���ʽ�Ҳ�����
            lenRight++;

        int s_index[20] = {0}; // ��¼s�ڲ���ʽ�Ҳ����ֵ�����λ��
        int s_index_num = 0;   // ���������β���±꣨����������

        for (int j = 0; Productions[i].right[j] != "0"; j++)
        {
            if (Productions[i].right[j] == s) // �Ҳ���s����¼�ڲ���ʽ�Ҳ����±�
            {
                s_index[s_index_num] = j;
                s_index_num++;
            }
        }

        // 1.����һ������ʽA����B�£���ôfirst(��)�г�$������з��Ŷ���follow(B)��
        for (int j = 0; j < s_index_num; j++)
        {
            tempindex = s_index[j];

            if (tempindex != -1 && tempindex < (lenRight - 1))
            {
                bool hasEmpty = true; // ʽ�����Ƿ����$
                while ((tempindex < lenRight - 1) && hasEmpty)
                {
                    tempindex += 1;
                    const string &beta = Productions[i].right[tempindex];
                    if (nonterminalSymbolIndex(beta) == -1) // �����ս��
                    {
                        if (follows[index].flag[terminalSymbolIndex(beta)] == false) // ��δ�������follow��
                        {
                            follows[index].ptr[follows[index].num] = beta;
                            follows[index].flag[terminalSymbolIndex(beta)] = true;
                            follows[index].num++;
                        }
                        hasEmpty = false;
                        break;
                    }
                    else // ���Ƿ��ս��
                    {
                        hasEmpty = false;
                        int betaindex = nonterminalSymbolIndex(beta);   // beta�ڷ��ս�������е��±�
                        for (int k = 0; k < firsts[betaindex].num; k++) // ��first(��)�����г���$�ķ��ż��뵽follow(B)��
                        {
                            if (firsts[betaindex].ptr[k] == "$")
                                hasEmpty = true;
                            else
                            {
                                if (follows[index].flag[terminalSymbolIndex(firsts[betaindex].ptr[k])] == false) // ����ս��δ�����follow��
                                {
                                    follows[index].ptr[follows[index].num] = firsts[betaindex].ptr[k];
                                    follows[index].flag[terminalSymbolIndex(firsts[betaindex].ptr[k])] = true;
                                    follows[index].num++;
                                }
                            }
                        }
                    }
                }
                // 2.����һ������ʽA����B�£���first(��)����$����ôfollow(A)�е����з��Ŷ���follow(B)��
                if (hasEmpty && Productions[i].left != s)
                {
                    calFollow(Productions[i].left, flag);
                    int index_A = nonterminalSymbolIndex(Productions[i].left); // ���ս��A�ڷ��ս�������е��±�
                    for (int k = 0; k < follows[index_A].num; k++)
                    {
                        if (follows[index].flag[terminalSymbolIndex(follows[index_A].ptr[k])] == false) // ��������follow(B)���ս��follows[index_A].ptr[k]��ǰδ�����
                        {
                            follows[index].ptr[follows[index].num] = follows[index_A].ptr[k];
                            follows[index].flag[terminalSymbolIndex(follows[index_A].ptr[k])] = true;
                            follows[index].num++;
                        }
                    }
                }
            }
            // 3.����һ������ʽA����B����ôfollow(A)�е����з��Ŷ���follow(B)��
            else if (tempindex != -1 && tempindex == lenRight - 1 && Productions[i].left != s)
            {
                calFollow(Productions[i].left, flag);
                int index_A = nonterminalSymbolIndex(Productions[i].left); // ���ս��A�ڷ��ս�������е��±�
                for (int k = 0; k < follows[index_A].num; k++)
                {
                    if (follows[index].flag[terminalSymbolIndex(follows[index_A].ptr[k])] == false) // ��������follow(B)���ս��follows[index_A].ptr[k]��ǰδ�����
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

void outFitstFollow() // ���first����follow��������
{
    // NonNum �Ƿ��ռ���������
    for (int i = 0; i < NonNum; i++) // ��ʼ��
    {
        firsts[i].num = 0;
        memset(firsts[i].flag, false, sizeof(firsts[i].flag));
        follows[i].num = 0;
        memset(follows[i].flag, false, sizeof(follows[i].flag));
    }

    for (int i = 0; i < NonNum; i++) // �������з��ս����first��
        calFirst(Non_symbol[i]);

    for (int i = 0; i < NonNum; i++) // �������з��ս����follow��
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
void calPredict() // ����predict�� - LL1�ķ�
{
    for (int i = 0; i < ProductNum; i++) // ��ʼ��
    {
        predicts[i].num = 0;
        memset(predicts[i].flag, false, sizeof(predicts[i].flag));
    }

    for (int i = 0; i < ProductNum; i++)
    {
        int leftindex = nonterminalSymbolIndex(Productions[i].left); // ���ڷ��ս�������е��±�
        int countEmpty = 0;
        // 1.���������Ƴ�$����predict(A����)=first(��)
        for (int j = 0; Productions[i].right[j] != "0"; j++)
        {
            if (nonterminalSymbolIndex(Productions[i].right[j]) == -1) // �Ҳ����ս���������գ�
            {
                if (Productions[i].right[j] != "$") // ����ս������$
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
            else // �Ҳ��Ƿ��ս��
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
                bool haveEmpty = false; // first(index)����û��$
                for (int k = 0; k < firsts[index].num; k++)
                    if (firsts[index].ptr[k] == "$")
                        haveEmpty = true;
                if (haveEmpty == true) // first(right[j])����$�����������е���һ������
                    countEmpty++;
                else
                    break;
            }
        }
        // 2.�������Ƴ�$����predict(A����)={first(��)-$}��follow(A)
        int lenRight = 0;
        for (int j = 0; Productions[i].right[j] != "0"; j++) // ͳ�Ʋ���ʽ�Ҳ�����
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
