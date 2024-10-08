#pragma once
#include <string>
using namespace std;
typedef enum // 词法类型定义
{            // 簿记单词符
    ENDFILE,
    ERROR,

    // 保留字,保留字和类型属于标识符范畴，标识符（ID）状态课识别出保留字，类型，和ID
    PROGRAM,
    PROCEDURE,
    TYPE,
    VAR,
    IF,
    THEN,
    ELSE,
    FI,
    WHILE,
    DO,
    ENDWH,
    BEGIN,
    END,
    READ,
    WRITE,
    ARRAY,
    OF,
    RECORD,
    RETURN,
    // 类型
    INTEGER,
    CHAR,

    // 多字符单词符号
    ID,
    INTC,
    CHARC,

    // 特殊符号
    ASSIGN,
    EQ,
    LT,
    PLUS,
    MINUS, //=
    TIMES,
    OVER,
    LPAREN,
    RPAREN,
    DOT, //
    COLON,
    SEMI,
    COMMA,
    LMIDPAREN,
    RMIDPAREN,
    UNDERANGE // 数组下标
} LexType;

// 类型声明
enum State
{
    START = 1,
    INID,
    INNUM,
    DONE,
    INASSIGN,
    INCOMMENT,
    INRANGE,
    INCHAR
}; // DFA状态

// 头文件放声明，定义放在函数中，不可以放在函数之外
struct Word // 保留字表，便于查阅
{
    string Sem;  // 语义信息
    LexType Lex; // 词法信息
};               // 保留字

struct Token // 链表结点定义
{
    int Line; // 单词所在行数
    LexType Lex;
    string Sem; // 单词语义信息
    Token *pre;
    Token *next;
    Token()
    {
        this->pre = nullptr;
        this->next = nullptr;
    }
    Token(Token *tail)
    {
        tail->next = this;
        this->pre = tail;
        this->next = nullptr;
    }
    ~Token()
    {
        delete this;
    }
};

// 函数声明
char getNextChar(); // 从源文件中读取一个字符
int changeState(char ch);
LexType singleDelimiter(char ch);
LexType identifierOrReservedWord(string &str1);
string printLex(LexType Lex);
void printToken(Token *head); // 打印输出token表
Token *getTokenList();