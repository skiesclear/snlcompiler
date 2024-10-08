#include <iostream>
#include <fstream>
#include <iomanip>
#include "LexicalAnalysis.h"

ifstream in("D:\\vscode_program\\C++\\SnlCompiler\\Input\\snlCode\\c1.txt");
ofstream out("D:\\vscode_program\\C++\\SnlCompiler\\Output\\token\\token.txt");

Word reservedWords[21] = {{"program", PROGRAM}, {"type", TYPE}, {"var", VAR}, {"procedure", PROCEDURE}, {"begin", BEGIN}, {"end", END}, {"array", ARRAY}, {"of", OF}, {"record", RECORD}, {"if", IF}, {"then", THEN}, {"else", ELSE}, {"fi", FI}, {"while", WHILE}, {"do", DO}, {"endwh", ENDWH}, {"read", READ}, {"write", WRITE}, {"return", RETURN}, {"integer", INTEGER}, {"char", CHAR}}; // 保留字

char getNextChar()
{
    return in.get();
}

int changeState(char ch)
{ //
    if ((ch > '@' && ch < '[') || (ch > '`' && ch < '{'))
        return 1; // 字母
    if (ch >= '0' && ch <= '9')
        return 2; // 数字
    if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '(' || ch == ')' || ch == ';' || ch == '[' || ch == ']' || ch == '=' || ch == '<' || ch == ',')
        return 3; // 单分界符
    if (ch == ':')
        return 4; // 双分界符，接收为：
    if (ch == '{')
        return 5; // 注释头符
    if (ch == '.')
        return 6; // 数组下标
    if (ch == '\'')
        return 7; // 字符状态
    // 以下为测试部分
    if (ch == ' ' || ch == '\t') // 字符为空白
        return 8;
    if (ch == '\n')
        return 9;
    if (ch == EOF)
        return 10;
    return 11;
}

LexType singleDelimiter(char ch)
{
    if (ch == '+')
    {
        return PLUS;
    }
    else if (ch == '-')
    {
        return MINUS;
    }
    else if (ch == '*')
    {
        return TIMES;
    }
    else if (ch == '/')
    {
        return OVER;
    }
    else if (ch == '(')
    {
        return LPAREN;
    }
    else if (ch == ')')
    {
        return RPAREN;
    }
    else if (ch == ';')
    {
        return SEMI;
    }
    else if (ch == '[')
    {
        return LMIDPAREN;
    }
    else if (ch == ']')
    {
        return RMIDPAREN;
    }
    else if (ch == '=')
    {
        return EQ;
    }
    else if (ch == '<')
    {
        return LT;
    }
    else if (ch == ',')
    {
        return COMMA;
    }
    else if (ch == EOF)
    {
        return ENDFILE;
    }
}

LexType identifierOrReservedWord(string &str1)
{ // ID分类函数
    for (int i = 0; i < 21; i++)
    {
        if (reservedWords[i].Sem == str1)
        {
            return reservedWords[i].Lex;
        }
    }
    return ID;
}

// 未处理:添加空字符的token处理?课本中处理并无空token
Token *getTokenList()
{ // 未完成：在状态转移过程中的token同时生成;有错误的处理方法
    if (!in.is_open())
    {
        printf("lexical: input file open fail\n");
        return NULL;
    }
    // 链表结点处理
    // 变量定义
    State state = START;
    int Line = 1;

    Token *head = new Token();
    Token *current = head;
    Token *temp = nullptr;
    char ch = getNextChar();

    // DFA字符接收器
    string buffer;

    // 未添加读新字符的循环
    // 添加出口:读完整个程序文件的出口->INRANGE_ERROR
    // 未添加行数的识别
    while (true)
    {
        switch (state)
        {
        case START: // 初始状态；错误类型：INASSIGN_ERROR，
            buffer.clear();
            switch (changeState(ch))
            {
            case 1:
                state = INID;
                buffer += ch;
                break;
            case 2:
                state = INNUM;
                buffer += ch;
                break;
            case 3:
                state = DONE;
                buffer += ch;
                break;
            case 4: // INASSIGN_ERROR
                buffer += ch;
                ch = getNextChar();
                if (ch == '=')
                {
                    state = INASSIGN;
                    buffer += ch;
                    break;
                }
                else
                {
                    cout << Line << " :Assignment statement error" << endl;
                    exit(1);
                }
            case 5:
                state = INCOMMENT;
                buffer += ch;
                break; // 注释状态
            case 6:
                buffer += ch;
                ch = getNextChar();
                if (ch == '.')
                {
                    state = INRANGE;
                    buffer += ch;
                    break; // 数组下标界限状态
                }
                else
                { // 其他符号
                    current->Line = Line;
                    current->Lex = DOT;
                    current->Sem = ".";
                    temp = new Token(current);
                    current = temp;
                    state = START;
                    break;
                }
            case 7: // 无需录入
                state = INCHAR;
                break;
            case 8: // 空白认为是上一个输入状态的截止,空格空行换行等都在此处理，其余状态的输入空格也退回到此处处理
                ch = getNextChar();
                break;
            case 9: // 换行
                ch = getNextChar();
                Line++;
                break;
            case 10: // 正常识别到文件结束符
                current->Lex = ENDFILE;
                current->Sem = "ENDFILE";
                current->next = NULL;
                current->Line = Line;
                return head;
            default: // 输入无法识别
                cout << "Line " << Line << " has unrecognized characters";
                exit(1);
            }
            break;
        case INID:
            ch = getNextChar();
            // 当读入的是字母和数字时，继续读入
            while (changeState(ch) == 1 || changeState(ch) == 2)
            {
                buffer += ch;
                ch = getNextChar();
            }
            current->Line = Line;
            current->Lex = identifierOrReservedWord(buffer);
            current->Sem = buffer;
            temp = new Token(current);
            current = temp;
            state = START;
            break;
        case INNUM:
            ch = getNextChar();
            // 当读入的是数字时，继续读入
            while (changeState(ch) == 2)
            {
                buffer += ch;
                ch = getNextChar();
            }
            current->Line = Line;
            current->Sem = buffer;
            current->Lex = INTC;
            temp = new Token(current);
            current = temp;
            state = START;
            break;
        case INASSIGN: // 赋值状态；错误类型：无
            current->Line = Line;
            current->Lex = ASSIGN;
            current->Sem = ":=";
            temp = new Token(current);
            current = temp;
            ch = getNextChar();
            state = START;
            break;
        case INCOMMENT: // 包含符号在内，不生成token
            ch = getNextChar();
            while (ch != '}')
                ch = getNextChar();
            ch = getNextChar();
            // printf("识别并已跳过注释");//不影响下一轮token生成
            state = START;
            break;
        case INRANGE: // 数组下标
            current->Lex = UNDERANGE;
            current->Sem = "..";
            current->Line = Line;
            temp = new Token(current);
            current = temp;
            ch = getNextChar();
            state = START;
            break;
        case INCHAR:
            ch = getNextChar();
            if (changeState(ch) == 1 || changeState(ch) == 2)
            {
                buffer += ch;
                current->Line = Line;
                current->Lex = CHARC;
                current->Sem = buffer;
                temp = new Token(current);
                current = temp;
                state = DONE;
            }
            else
            {
                cout << "Line " << Line << " has unrecognized characters";
                exit(1);
            }
            break;
        case DONE:
            if (changeState(ch) == 1 || changeState(ch) == 2)
            { // 单字符识别
                ch = getNextChar();
                if (ch == '\'')
                {
                    state = START;
                    ch = getNextChar();
                }
                else
                {
                    cout << "Line " << Line << " has single character input error";
                    exit(1);
                }
            }
            else
            { // 单分界符
                current->Line = Line;
                current->Lex = singleDelimiter(ch);
                current->Sem = buffer;
                temp = new Token(current);
                current = temp;
                state = START;
                ch = getNextChar();
            }
            break;
        default:
            break;
        }
    }
    in.close();
    cout << "Lexical analysis is performed successfully" << endl;
    return head;
}

string printLex(LexType Lex)
{ // 打印输出词法信息
    if (Lex == 0)
        return "ENDFILE";
    else if (Lex == 1)
        return "ERROR";
    else if (Lex == 2)
        return "PROGRAM";
    else if (Lex == 3)
        return "PROCEDURE";
    else if (Lex == 4)
        return "TYPE";
    else if (Lex == 5)
        return "VAR";
    else if (Lex == 6)
        return "IF";
    else if (Lex == 7)
        return "THEN";
    else if (Lex == 8)
        return "ELSE";
    else if (Lex == 9)
        return "FI";
    else if (Lex == 10)
        return "WHILE";
    else if (Lex == 11)
        return "DO";
    else if (Lex == 12)
        return "ENDWH";
    else if (Lex == 13)
        return "BEGIN";
    else if (Lex == 14)
        return "END";
    else if (Lex == 15)
        return "READ";
    else if (Lex == 16)
        return "WRITE";
    else if (Lex == 17)
        return "ARRAY";
    else if (Lex == 18)
        return "OF";
    else if (Lex == 19)
        return "RECORD";
    else if (Lex == 20)
        return "RETURN";
    else if (Lex == 21)
        return "INTEGER";
    else if (Lex == 22)
        return "CHAR";
    else if (Lex == 23)
        return "ID";
    else if (Lex == 24)
        return "INTC";
    else if (Lex == 25)
        return "CHARC";
    else if (Lex == 26)
        return "ASSIGN";
    else if (Lex == 27)
        return "EQ";
    else if (Lex == 28)
        return "LT";
    else if (Lex == 29)
        return "PLUS";
    else if (Lex == 30)
        return "MINUS";
    else if (Lex == 31)
        return "TIMES";
    else if (Lex == 32)
        return "OVER";
    else if (Lex == 33)
        return "LPAREN";
    else if (Lex == 34)
        return "RPAREN";
    else if (Lex == 35)
        return "DOT";
    else if (Lex == 36)
        return "COLON";
    else if (Lex == 37)
        return "SEMI";
    else if (Lex == 38)
        return "COMMA";
    else if (Lex == 39)
        return "LMIDPAREN";
    else if (Lex == 40)
        return "RMIDPAREN";
    else
        return "UNDERANGE";
}

void printToken(Token *head)
{
    if (!out.is_open())
    {
        printf("\nThe lexical :token.txt file cannot be opened, and the token information cannot be output to the file!\n");
        return;
    }
    out << std::left << std::setw(15) << "Lineshow";
    out << std::left << std::setw(15) << "Lexical";
    out << std::left << std::setw(15) << "Semantic" << endl;

    while (head != NULL)
    {
        out << std::left << std::setw(15) << head->Line;

        out << std::left << std::setw(15) << printLex(head->Lex);

        out << std::left << std::setw(15) << head->Sem << endl;
        head = head->next;
    }
    out.close();
}
