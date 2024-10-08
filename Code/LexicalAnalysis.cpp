#include <iostream>
#include <fstream>
#include <iomanip>
#include "LexicalAnalysis.h"

ifstream in("D:\\vscode_program\\C++\\SnlCompiler\\Input\\snlCode\\c1.txt");
ofstream out("D:\\vscode_program\\C++\\SnlCompiler\\Output\\token\\token.txt");

Word reservedWords[21] = {{"program", PROGRAM}, {"type", TYPE}, {"var", VAR}, {"procedure", PROCEDURE}, {"begin", BEGIN}, {"end", END}, {"array", ARRAY}, {"of", OF}, {"record", RECORD}, {"if", IF}, {"then", THEN}, {"else", ELSE}, {"fi", FI}, {"while", WHILE}, {"do", DO}, {"endwh", ENDWH}, {"read", READ}, {"write", WRITE}, {"return", RETURN}, {"integer", INTEGER}, {"char", CHAR}}; // ������

char getNextChar()
{
    return in.get();
}

int changeState(char ch)
{ //
    if ((ch > '@' && ch < '[') || (ch > '`' && ch < '{'))
        return 1; // ��ĸ
    if (ch >= '0' && ch <= '9')
        return 2; // ����
    if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '(' || ch == ')' || ch == ';' || ch == '[' || ch == ']' || ch == '=' || ch == '<' || ch == ',')
        return 3; // ���ֽ��
    if (ch == ':')
        return 4; // ˫�ֽ��������Ϊ��
    if (ch == '{')
        return 5; // ע��ͷ��
    if (ch == '.')
        return 6; // �����±�
    if (ch == '\'')
        return 7; // �ַ�״̬
    // ����Ϊ���Բ���
    if (ch == ' ' || ch == '\t') // �ַ�Ϊ�հ�
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
{ // ID���ຯ��
    for (int i = 0; i < 21; i++)
    {
        if (reservedWords[i].Sem == str1)
        {
            return reservedWords[i].Lex;
        }
    }
    return ID;
}

// δ����:��ӿ��ַ���token����?�α��д����޿�token
Token *getTokenList()
{ // δ��ɣ���״̬ת�ƹ����е�tokenͬʱ����;�д���Ĵ�����
    if (!in.is_open())
    {
        printf("lexical: input file open fail\n");
        return NULL;
    }
    // �����㴦��
    // ��������
    State state = START;
    int Line = 1;

    Token *head = new Token();
    Token *current = head;
    Token *temp = nullptr;
    char ch = getNextChar();

    // DFA�ַ�������
    string buffer;

    // δ��Ӷ����ַ���ѭ��
    // ��ӳ���:�������������ļ��ĳ���->INRANGE_ERROR
    // δ���������ʶ��
    while (true)
    {
        switch (state)
        {
        case START: // ��ʼ״̬���������ͣ�INASSIGN_ERROR��
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
                break; // ע��״̬
            case 6:
                buffer += ch;
                ch = getNextChar();
                if (ch == '.')
                {
                    state = INRANGE;
                    buffer += ch;
                    break; // �����±����״̬
                }
                else
                { // ��������
                    current->Line = Line;
                    current->Lex = DOT;
                    current->Sem = ".";
                    temp = new Token(current);
                    current = temp;
                    state = START;
                    break;
                }
            case 7: // ����¼��
                state = INCHAR;
                break;
            case 8: // �հ���Ϊ����һ������״̬�Ľ�ֹ,�ո���л��еȶ��ڴ˴�������״̬������ո�Ҳ�˻ص��˴�����
                ch = getNextChar();
                break;
            case 9: // ����
                ch = getNextChar();
                Line++;
                break;
            case 10: // ����ʶ���ļ�������
                current->Lex = ENDFILE;
                current->Sem = "ENDFILE";
                current->next = NULL;
                current->Line = Line;
                return head;
            default: // �����޷�ʶ��
                cout << "Line " << Line << " has unrecognized characters";
                exit(1);
            }
            break;
        case INID:
            ch = getNextChar();
            // �����������ĸ������ʱ����������
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
            // �������������ʱ����������
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
        case INASSIGN: // ��ֵ״̬���������ͣ���
            current->Line = Line;
            current->Lex = ASSIGN;
            current->Sem = ":=";
            temp = new Token(current);
            current = temp;
            ch = getNextChar();
            state = START;
            break;
        case INCOMMENT: // �����������ڣ�������token
            ch = getNextChar();
            while (ch != '}')
                ch = getNextChar();
            ch = getNextChar();
            // printf("ʶ��������ע��");//��Ӱ����һ��token����
            state = START;
            break;
        case INRANGE: // �����±�
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
            { // ���ַ�ʶ��
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
            { // ���ֽ��
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
{ // ��ӡ����ʷ���Ϣ
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
