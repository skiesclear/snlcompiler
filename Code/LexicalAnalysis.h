#pragma once
#include <string>
using namespace std;
typedef enum // �ʷ����Ͷ���
{            // ���ǵ��ʷ�
    ENDFILE,
    ERROR,

    // ������,�����ֺ��������ڱ�ʶ�����룬��ʶ����ID��״̬��ʶ��������֣����ͣ���ID
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
    // ����
    INTEGER,
    CHAR,

    // ���ַ����ʷ���
    ID,
    INTC,
    CHARC,

    // �������
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
    UNDERANGE // �����±�
} LexType;

// ��������
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
}; // DFA״̬

// ͷ�ļ���������������ں����У������Է��ں���֮��
struct Word // �����ֱ����ڲ���
{
    string Sem;  // ������Ϣ
    LexType Lex; // �ʷ���Ϣ
};               // ������

struct Token // �����㶨��
{
    int Line; // ������������
    LexType Lex;
    string Sem; // ����������Ϣ
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

// ��������
char getNextChar(); // ��Դ�ļ��ж�ȡһ���ַ�
int changeState(char ch);
LexType singleDelimiter(char ch);
LexType identifierOrReservedWord(string &str1);
string printLex(LexType Lex);
void printToken(Token *head); // ��ӡ���token��
Token *getTokenList();