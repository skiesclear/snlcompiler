#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <fstream>
#include "GrammerAnalysis.h"

using namespace std;

Token *currentToken = NULL; // 当前指向的token节点
int nodeNumber = 0;         // RD树的各节点编号，每次创建节点时累加

treenode *RDAnalysis(Token *tokenhead) // 递归下降分析法主程序，接收token序列头
{
    currentToken = tokenhead;         // 指向token序列的头
    treenode *RDTreeRoot = program(); // 递归下降分析，获得总根节点
    return RDTreeRoot;                // 返回总根节点，下一步进行语义分析
}

treenode *matchToken(LexType tok) // 匹配当前token与终结符，之后移动指针
{
    if (tok == currentToken->Lex) // 终结符匹配上了
    {
        treenode *newnode = new treenode(nodeNumber);
        newnode->token = currentToken;
        currentToken = currentToken->next;
        return newnode;
    }
    else
    {
        cout << "Line " << currentToken->Line << " lack signal \"" << printLex(tok) << "\"" << endl;
        exit(1);
    }
}

// 每个非终结符是一个函数，以下共58个函数（参考书上只给出了58个，虽然有67个非终结符）
treenode *program()
{
    treenode *newnode = new treenode("Program", nodeNumber);
    newnode->addChild(programHead());
    newnode->addChild(declarePart());
    newnode->addChild(programBody());
    return newnode;
}

treenode *programHead()
{
    treenode *newnode = new treenode("ProgramHead", nodeNumber);
    newnode->addChild(matchToken(PROGRAM));
    newnode->addChild(matchToken(ID));
    return newnode;
}

treenode *declarePart()
{
    treenode *newnode = new treenode("DeclarePart", nodeNumber);
    newnode->addChild(typeDecPart());
    newnode->addChild(varDecPart());
    newnode->addChild(procDecpart());
    return newnode;
}

treenode *programBody()
{

    treenode *newnode = new treenode("ProgramBody", nodeNumber);
    newnode->addChild(matchToken(BEGIN));
    newnode->addChild(stmList());
    newnode->addChild(matchToken(END));
    return newnode;
}

treenode *typeDecPart()
{
    treenode *newnode = nullptr;
    if (currentToken->Lex == TYPE) // 判断此处是否有类型声明
    {
        newnode = new treenode("TypeDecPart", nodeNumber);
        newnode->addChild(typeDec());
    }
    return newnode;
}

treenode *varDecPart()
{
    treenode *newnode = NULL;
    if (currentToken->Lex == VAR)
    {
        newnode = new treenode("VarDecPart", nodeNumber);
        newnode->addChild(varDec());
    }
    return newnode;
}

treenode *procDecpart()
{
    treenode *newnode = NULL;
    if (currentToken->Lex == PROCEDURE)
    {
        newnode = new treenode("ProcDecPart", nodeNumber);
        newnode->addChild(procDec());
    }
    return newnode;
}

treenode *typeDec()
{
    treenode *newnode = new treenode("TypeDec", nodeNumber);
    newnode->addChild(matchToken(TYPE));
    newnode->addChild(typeDecList());
    return newnode;
}

treenode *typeDecList()
{
    treenode *newnode = new treenode("TypeDecList", nodeNumber);
    newnode->addChild(matchToken(ID));
    newnode->addChild(matchToken(EQ));
    newnode->addChild(typeDef());
    newnode->addChild(matchToken(SEMI));
    newnode->addChild(typeDecMore());
    return newnode;
}

treenode *typeDef()
{
    treenode *newnode = new treenode("TypeDef", nodeNumber);
    if ((currentToken->Lex == INTEGER) || (currentToken->Lex == CHAR))
    {
        newnode->addChild(baseType());
        return newnode;
    }
    else if ((currentToken->Lex == ARRAY) || (currentToken->Lex == RECORD))
    {
        newnode->addChild(structureType());
        return newnode;
    }
    else
    {
        newnode->addChild(matchToken(ID));
        return newnode;
    }
}

treenode *typeDecMore()
{
    treenode *newnode = nullptr;
    if (currentToken->Lex == ID)
    {
        newnode = new treenode("TypeDecMore", nodeNumber);
        newnode->addChild(typeDecList());
    }
    return newnode;
}

treenode *baseType()
{
    treenode *newnode = nullptr;
    if (currentToken->Lex == INTEGER)
    {
        newnode = new treenode("BaseType", nodeNumber);
        newnode->addChild(matchToken(INTEGER));
        return newnode;
    }
    else if (currentToken->Lex == CHAR)
    {
        newnode = new treenode("BaseType", nodeNumber);
        newnode->addChild(matchToken(CHAR));
        return newnode;
    }
    cout << "Line " << currentToken->Line << " " << currentToken->Sem << " basetype error" << endl;
    exit(1);
}

treenode *structureType()
{
    treenode *newnode = nullptr;
    if (currentToken->Lex == ARRAY)
    {
        newnode = new treenode("StructureType", nodeNumber);
        newnode->addChild(arrayType());
        return newnode;
    }
    else if (currentToken->Lex == RECORD)
    {
        newnode = new treenode("StructureType", nodeNumber);
        newnode->addChild(recType());
        return newnode;
    }
    return newnode;
}

treenode *arrayType()
{
    treenode *newnode = new treenode("ArrayType", nodeNumber);
    newnode->addChild(matchToken(ARRAY));
    newnode->addChild(matchToken(LMIDPAREN));
    newnode->addChild(matchToken(INTC));
    newnode->addChild(matchToken(UNDERANGE));
    newnode->addChild(matchToken(INTC));
    newnode->addChild(matchToken(RMIDPAREN));
    newnode->addChild(matchToken(OF));
    newnode->addChild(baseType());
    return newnode;
}

treenode *recType()
{
    treenode *newnode = new treenode("RecType", nodeNumber);
    newnode->addChild(matchToken(RECORD));
    newnode->addChild(fieldDecList());
    newnode->addChild(matchToken(END));
    return newnode;
}

treenode *fieldDecList()
{
    treenode *newnode = nullptr;
    if (currentToken->Lex == INTEGER || currentToken->Lex == CHAR)
    {
        newnode = new treenode("FieldDecList", nodeNumber);
        newnode->addChild(baseType());
        newnode->addChild(IDList());
        newnode->addChild(matchToken(SEMI));
        newnode->addChild(fieldDecMore());
        return newnode;
    }
    else if (currentToken->Lex == ARRAY)
    {
        newnode = new treenode("FieldDecList", nodeNumber);
        newnode->addChild(arrayType());
        newnode->addChild(IDList());
        newnode->addChild(matchToken(SEMI));
        newnode->addChild(fieldDecMore());
        return newnode;
    }
    cout << "Line " << currentToken->Line << " FieldDeclareList Error." << endl;
    exit(1);
    return nullptr;
}

treenode *IDList()
{
    treenode *newnode = new treenode("IDList", nodeNumber);
    newnode->addChild(matchToken(ID));
    newnode->addChild(IDMore());
    return newnode;
}

treenode *fieldDecMore()
{
    treenode *newnode = nullptr;
    if (currentToken->Lex == INTEGER || currentToken->Lex == CHAR || currentToken->Lex == ARRAY)
    {
        newnode = new treenode("FieldDecMore", nodeNumber);
        newnode->addChild(fieldDecList());
        return newnode;
    }
    return newnode;
}

treenode *IDMore()
{
    treenode *newnode = nullptr;
    if (currentToken->Lex == COMMA)
    {
        newnode = new treenode("IDMore", nodeNumber);
        newnode->addChild(matchToken(COMMA));
        newnode->addChild(IDList());
        return newnode;
    }
    return newnode;
}

treenode *varDec()
{
    treenode *newnode = new treenode("VarDec", nodeNumber);
    newnode->addChild(matchToken(VAR));
    newnode->addChild(varDecList());
    return newnode;
}

treenode *varDecList()
{
    treenode *newnode = new treenode("VarDecList", nodeNumber);
    newnode->addChild(typeDef());
    newnode->addChild(varIDList());
    newnode->addChild(matchToken(SEMI));
    newnode->addChild(varDecMore());
    return newnode;
}

treenode *varIDList()
{
    treenode *newnode = new treenode("VarIDList", nodeNumber);
    newnode->addChild(matchToken(ID));
    newnode->addChild(varIDMore());
    return newnode;
}

treenode *varDecMore()
{
    treenode *newnode = nullptr;
    if (currentToken->Lex == INTEGER || currentToken->Lex == CHAR || currentToken->Lex == ARRAY || currentToken->Lex == RECORD || currentToken->Lex == ID)
    {
        newnode = new treenode("VarDecMore", nodeNumber);
        newnode->addChild(varDecList());
        return newnode;
    }
    return newnode;
}

treenode *varIDMore()
{
    treenode *newnode = nullptr;
    if (currentToken->Lex == COMMA)
    {
        newnode = new treenode("VarIDMore", nodeNumber);
        newnode->addChild(matchToken(COMMA));
        newnode->addChild(varIDList());
        return newnode;
    }
    return newnode;
}

treenode *procDec()
{
    treenode *newnode = new treenode("ProcDec", nodeNumber);
    newnode->addChild(matchToken(PROCEDURE));
    newnode->addChild(matchToken(ID));
    newnode->addChild(matchToken(LPAREN));
    newnode->addChild(paramList());
    newnode->addChild(matchToken(RPAREN));
    newnode->addChild(matchToken(SEMI));
    newnode->addChild(procDecPart()); // 更高一个level的子函数
    newnode->addChild(procBody());
    newnode->addChild(procDecpart()); // 同一个level的兄弟函数
    return newnode;
}

treenode *paramList()
{
    treenode *newnode = NULL;
    if (currentToken->Lex == INTEGER || currentToken->Lex == CHAR || currentToken->Lex == ARRAY || currentToken->Lex == RECORD || currentToken->Lex == VAR || currentToken->Lex == ID)
    {
        newnode = new treenode("ParamList", nodeNumber);
        newnode->addChild(paramDecList());
        return newnode;
    }
    return newnode;
}

treenode *procDecPart()
{
    treenode *newnode = new treenode("ProcDecPart", nodeNumber);
    newnode->addChild(declarePart());
    return newnode;
}

treenode *procBody()
{
    treenode *newnode = new treenode("ProcBody", nodeNumber);
    newnode->addChild(programBody());
    return newnode;
}

treenode *paramDecList()
{
    treenode *newnode = new treenode("ParamDecList", nodeNumber);
    newnode->addChild(param());
    newnode->addChild(paramMore());
    return newnode;
}

treenode *param()
{
    treenode *newnode = nullptr;
    if (currentToken->Lex == INTEGER || currentToken->Lex == CHAR || currentToken->Lex == ARRAY || currentToken->Lex == RECORD || currentToken->Lex == ID)
    {
        newnode = new treenode("Param", nodeNumber);
        newnode->addChild(typeDef());
        newnode->addChild(formList());
        return newnode;
    }
    else if (currentToken->Lex == VAR)
    {
        newnode = new treenode("Param", nodeNumber);
        newnode->addChild(matchToken(VAR));
        newnode->addChild(typeDef());
        newnode->addChild(formList());
        return newnode;
    }
    cout << "Line " << currentToken->Line << " " << currentToken->Sem << " Param Error" << endl;
    exit(1);
    return newnode;
}

treenode *paramMore()
{
    treenode *newnode = nullptr;
    if (currentToken->Lex == SEMI)
    {
        newnode = new treenode("ParamMore", nodeNumber);
        newnode->addChild(matchToken(SEMI));
        newnode->addChild(paramDecList());
        return newnode;
    }
    return newnode;
}

treenode *formList()
{
    treenode *newnode = new treenode("FormList", nodeNumber);
    newnode->addChild(matchToken(ID));
    newnode->addChild(fidMore());
    return newnode;
}

treenode *fidMore()
{
    treenode *newnode = nullptr;
    if (currentToken->Lex == COMMA)
    {
        newnode = new treenode("FidMore", nodeNumber);
        newnode->addChild(matchToken(COMMA));
        newnode->addChild(formList());
        return newnode;
    }
    return newnode;
}

treenode *stmList()
{
    treenode *newnode = new treenode("StmList", nodeNumber);
    newnode->addChild(stm());
    newnode->addChild(stmMore());
    return newnode;
}

treenode *stm()
{
    treenode *newnode = new treenode("Stm", nodeNumber);

    if (currentToken->Lex == IF)
    {
        newnode->addChild(conditionalStm());
        return newnode;
    }
    else if (currentToken->Lex == WHILE)
    {
        newnode->addChild(loopStm());
        return newnode;
    }
    else if (currentToken->Lex == READ)
    {
        newnode->addChild(inputStm());
        return newnode;
    }
    else if (currentToken->Lex == WRITE)
    {
        newnode->addChild(outputStm());
        return newnode;
    }
    else if (currentToken->Lex == RETURN)
    {
        newnode->addChild(returnStm());
        return newnode;
    }
    else if (currentToken->Lex == ID)
    {
        newnode->addChild(matchToken(ID));
        newnode->addChild(assCall());
        return newnode;
    }
    return nullptr;
}

treenode *stmMore()
{
    treenode *newnode = nullptr;
    if (currentToken->Lex == SEMI)
    {
        newnode = new treenode("StmMore", nodeNumber);
        newnode->addChild(matchToken(SEMI));
        newnode->addChild(stmList());
        return newnode;
    }
    return newnode;
}

treenode *conditionalStm()
{
    treenode *newnode = new treenode("ConditionalStm", nodeNumber);
    newnode->addChild(matchToken(IF));
    newnode->addChild(exp());
    if (currentToken->Lex == LT)
        newnode->addChild(matchToken(LT));
    else if (currentToken->Lex == EQ)
        newnode->addChild(matchToken(EQ));
    else
    {
        cout << "Line " << currentToken->Line << " " << currentToken->Sem << " conditionalStm error" << endl;
        exit(1);
    }
    newnode->addChild(exp());
    newnode->addChild(matchToken(THEN));
    newnode->addChild(stmList());
    newnode->addChild(matchToken(ELSE));
    newnode->addChild(stmList());
    newnode->addChild(matchToken(FI));
    return newnode;
}

treenode *loopStm()
{
    treenode *newnode = new treenode("LoopStm", nodeNumber);
    newnode->addChild(matchToken(WHILE));
    newnode->addChild(exp());
    if (currentToken->Lex == LT)
        newnode->addChild(matchToken(LT));
    else if (currentToken->Lex == EQ)
        newnode->addChild(matchToken(EQ));
    else
    {
        cout << "Line " << currentToken->Line << " " << currentToken->Sem << " loopStm error" << endl;
        exit(1);
    }
    newnode->addChild(exp());
    newnode->addChild(matchToken(DO));
    newnode->addChild(stmList());
    newnode->addChild(matchToken(ENDWH));
    return newnode;
}

treenode *inputStm()
{
    treenode *newnode = new treenode("InputStm", nodeNumber);
    newnode->addChild(matchToken(READ));
    newnode->addChild(matchToken(LPAREN));
    newnode->addChild(matchToken(ID));
    newnode->addChild(matchToken(RPAREN));
    return newnode;
}

treenode *outputStm()
{
    treenode *newnode = new treenode("OutputStm", nodeNumber);
    newnode->addChild(matchToken(WRITE));
    newnode->addChild(matchToken(LPAREN));
    newnode->addChild(exp());
    newnode->addChild(matchToken(RPAREN));
    return newnode;
}

treenode *returnStm()
{
    treenode *newnode = new treenode("ReturnStm", nodeNumber);
    newnode->addChild(matchToken(RETURN));
    return newnode;
}

treenode *assCall()
{
    treenode *newnode = nullptr;
    if (currentToken->Lex == LMIDPAREN || currentToken->Lex == DOT || currentToken->Lex == ASSIGN)
    {
        newnode = new treenode("AssCall", nodeNumber);
        newnode->addChild(assignmentRest());
        return newnode;
    }
    else if (currentToken->Lex == LPAREN)
    {
        newnode = new treenode("AssCall", nodeNumber);
        newnode->addChild(callStmRest());
        return newnode;
    }
    cout << "Line " << currentToken->Line << " " << currentToken->Sem << " asscall error" << endl;
    exit(1);
    return newnode;
}

treenode *assignmentRest()
{
    treenode *newnode = new treenode("AssignmentRest", nodeNumber);
    if (currentToken->Lex == LMIDPAREN || currentToken->Lex == DOT)
        newnode->addChild(variMore());
    newnode->addChild(matchToken(ASSIGN));
    newnode->addChild(exp());
    return newnode;
}

treenode *callStmRest()
{
    treenode *newnode = new treenode("CallStmRest", nodeNumber);
    newnode->addChild(matchToken(LPAREN));
    newnode->addChild(actparamList());
    newnode->addChild(matchToken(RPAREN));
    return newnode;
}

treenode *variMore()
{
    treenode *newnode = nullptr;
    if (currentToken->Lex == LMIDPAREN)
    {
        newnode = new treenode("VariMore", nodeNumber);
        newnode->addChild(matchToken(LMIDPAREN));
        newnode->addChild(exp());
        newnode->addChild(matchToken(RMIDPAREN));
    }
    else if (currentToken->Lex == DOT)
    {
        newnode = new treenode("VariMore", nodeNumber);
        newnode->addChild(matchToken(DOT));
        newnode->addChild(fieldVar());
    }
    return newnode;
}

treenode *exp()
{
    treenode *newnode = new treenode("Exp", nodeNumber);
    newnode->addChild(term());
    newnode->addChild(otherTerm());
    return newnode;
}

treenode *actparamList()
{
    treenode *newnode = nullptr;
    if (currentToken->Lex == LPAREN || currentToken->Lex == INTC || currentToken->Lex == ID)
    {
        newnode = new treenode("ActParamList", nodeNumber);
        newnode->addChild(exp());
        newnode->addChild(actparamMore());
    }
    return newnode;
}

treenode *actparamMore()
{
    treenode *newnode = nullptr;
    if (currentToken->Lex == COMMA)
    {
        newnode = new treenode("ActParamMore", nodeNumber);
        newnode->addChild(matchToken(COMMA));
        newnode->addChild(actparamList());
    }
    return newnode;
}

treenode *term()
{
    treenode *newnode = new treenode("Term", nodeNumber);
    newnode->addChild(factor());
    newnode->addChild(otherFactor());
    return newnode;
}

treenode *otherTerm()
{
    treenode *newnode = nullptr;
    if (currentToken->Lex == PLUS || currentToken->Lex == MINUS)
    {
        newnode = new treenode("OtherTerm", nodeNumber);
        newnode->addChild(addOp());
        newnode->addChild(exp());
    }
    return newnode;
}

treenode *addOp()
{
    treenode *newnode = nullptr;
    if (currentToken->Lex == PLUS)
    {
        newnode = new treenode("AddOp", nodeNumber);
        newnode->addChild(matchToken(PLUS));
        return newnode;
    }
    else if (currentToken->Lex == MINUS)
    {
        newnode = new treenode("AddOp", nodeNumber);
        newnode->addChild(matchToken(MINUS));
        return newnode;
    }
    cout << "Line " << currentToken->Line << " " << currentToken->Sem << " compareOp error" << endl;
    exit(1);
    return newnode;
}

treenode *factor()
{
    treenode *newnode = new treenode("Factor", nodeNumber);
    if (currentToken->Lex == INTC)
    {
        newnode->addChild(matchToken(INTC));
        return newnode;
    }
    else if (currentToken->Lex == LPAREN)
    {
        newnode->addChild(matchToken(LPAREN));
        newnode->addChild(exp());
        newnode->addChild(matchToken(RPAREN));
        return newnode;
    }
    else if (currentToken->Lex == ID)
    {
        newnode->addChild(variable());
        return newnode;
    }
    cout << "Line " << currentToken->Line << " " << currentToken->Sem << " factor error" << endl;
    exit(1);
    return nullptr;
}

treenode *otherFactor()
{
    treenode *newnode = nullptr;
    if (currentToken->Lex == TIMES || currentToken->Lex == OVER)
    {
        newnode = new treenode("OtherFactor", nodeNumber);
        newnode->addChild(multOp());
        newnode->addChild(term());
    }
    return newnode;
}

treenode *multOp()
{
    treenode *newnode = nullptr;
    if (currentToken->Lex == TIMES)
    {
        newnode = new treenode("MultOp", nodeNumber);
        newnode->addChild(matchToken(TIMES));
        return newnode;
    }
    else if (currentToken->Lex == OVER)
    {
        newnode = new treenode("MultOp", nodeNumber);
        newnode->addChild(matchToken(OVER));
        return newnode;
    }
    cout << "Line " << currentToken->Line << " " << currentToken->Sem << " compareOf error" << endl;
    exit(1);
    return newnode;
}

treenode *variable()
{
    treenode *newnode = new treenode("Variable", nodeNumber);
    newnode->addChild(matchToken(ID));
    newnode->addChild(variMore());
    return newnode;
}

treenode *fieldVar()
{
    treenode *newnode = new treenode("FieldVar", nodeNumber);
    newnode->addChild(matchToken(ID));
    newnode->addChild(fieldDecMore());
    return newnode;
}

treenode *fieldVarMore()
{
    treenode *newnode = nullptr;
    if (currentToken->Lex == LMIDPAREN)
    {
        newnode = new treenode("FieldVar", nodeNumber);
        newnode->addChild(matchToken(LMIDPAREN));
        newnode->addChild(exp());
        newnode->addChild(matchToken(RMIDPAREN));
    }
    return newnode;
}

// 保留字数量43
const char *Reserved_word[43] = {
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

void printTree(treenode *root)
{
    ofstream out("D:\\vscode_program\\C++\\SnlCompiler\\Output\\grammerTree\\tree.dot");
    out << "digraph graphRD{\n";
    int nullNodeNum = 0;
    deque<treenode *> q;
    q.push_back(root);
    while (!q.empty())
    {
        treenode *temp = q.front();
        q.pop_front();
        if (temp == nullptr)
            continue;
        for (int i = 0; i < temp->child.size(); i++)
        {
            if (temp->child[i] == nullptr)
            {
                out << temp->str << "_" << temp->index << "->NULL_" << nullNodeNum << endl;
                // fprintf(treefp, "%s_%d->NULL_%d\n", temp->str, temp->index, numnull);
                nullNodeNum++;
            }
            else if (temp->child[i]->token == nullptr)
            {
                // fprintf(treefp, "%s_%d->%s_%d\n", temp->str, temp->index, temp->child[i]->str, temp->child[i]->index);
                out << temp->str << "_" << temp->index
                    << "->"
                    << temp->child[i]->str << "_" << temp->child[i]->index << endl;
            }
            else
            {
                if (temp->child[i]->token->Lex == ID || temp->child[i]->token->Lex == CHARC) // 直接输出Sem
                {
                    out << temp->str << "_" << temp->index << "->" << temp->child[i]->token->Sem << "_" << temp->child[i]->index << endl;
                }
                else if (temp->child[i]->token->Lex == INTC) // 数字要加上int前缀，因为dot语言不允许数字开头
                {
                    out << temp->str << "_" << temp->index << "->int" << temp->child[i]->token->Sem << "_" << temp->child[i]->index << endl;
                }
                else // 是分号、括号这种的，dot语言不允许输出，输出对应的终结符即可
                {
                    out << temp->str << "_" << temp->index << "->" << Reserved_word[temp->child[i]->token->Lex] << "_" << temp->child[i]->index << endl;
                }
            }
            q.push_back(temp->child[i]);
        }
    }
    out << "}";
    out.close();
}