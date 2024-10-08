#include "SemanticAnalysis.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <unordered_map>
using namespace std;
vector<struct TypeIR *> TypeList; // ������Ͷ��壬ǰ�����̶����ֱ�Ϊ[0]:int, [1]:char, [2]bool,����ʹ�� TypeList[INTTY - '0'] ������int
vector<vector<SymbTable>> scope;  // ʹ��vector�����ʾscopeջ
vector<bool> exit_region;         // ��Ӧ�ֲ������Ƿ��Ѿ��˳���true:�Ѿ��˳���false:δ�˳�
int ValidTableCount = 0;

vector<Token *> Tok;
vector<Token *> Tok_ID;   // �洢IDList�ڵ�����Token
vector<Token *> Tok_form; // �洢formList������Token

/******���������Ϣ�����غ���******/
/***��ʶ��***/
void Error_IdentifierDuplicateDec(int line, string sem)
{ // 1.��ʶ�����ظ����壬��Ӧ�����������(1)
    string ErrorMessage = "Line:";
    ErrorMessage += to_string(line);
    ErrorMessage += ",   signal\"";
    ErrorMessage += sem;
    ErrorMessage += "\"duplicate definition\n";
    cout << ErrorMessage << endl;
    exit(1);
}

void Error_IdentifierDuplicateDecRecord(int line, string sem)
{ // 2.Record��ʶ���еı�ʶ�����ظ����壬��Ӧ�����������(1)
    string ErrorMessage = "Line:";
    ErrorMessage += to_string(line);
    ErrorMessage += ",   define signal in the definition of record signal\"";
    ErrorMessage += sem;
    ErrorMessage += "\"duplicate definition\n";
    cout << ErrorMessage << endl;
    exit(1);
}

void Error_IdentifierUndeclared(int line, string sem)
{ // 3.�������ı�ʶ������Ӧ�����������(2)
    string ErrorMessage = "Line:";
    ErrorMessage += to_string(line);
    ErrorMessage += ",   signal\"";
    ErrorMessage += sem;
    ErrorMessage += "\"non-declare\n";
    cout << ErrorMessage << endl;
    exit(1);
}

void Error_IdentifierUndeclaredVar(int line, string sem)
{ // 4.�������ı�����ʶ�������������������(3)
    string ErrorMessage = "Line:";
    ErrorMessage += to_string(line);
    ErrorMessage += ",   non-declare\"";
    ErrorMessage += sem;
    ErrorMessage += "\"var signal\n";
    cout << ErrorMessage << endl;
    exit(1);
}

void Error_IdentifierUndeclaredType(int line, string sem)
{ // 5.�����������ͱ�ʶ�������������������(3)
    string ErrorMessage = "Line:";
    ErrorMessage += to_string(line);
    ErrorMessage += ",   signal\"";
    ErrorMessage += sem;
    ErrorMessage += "\"is not type signal\n";
    cout << ErrorMessage << endl;
    exit(1);
}

/***����***/
void Error_ArraySubscriptLessThanZero(int line, string sem)
{ // 1. ��������ʱ�±�С��0����������������ʷ�������������������ʱ�±�С��0��
    string ErrorMessage = "Line:";
    ErrorMessage += to_string(line);
    ErrorMessage += ",   array index\"";
    ErrorMessage += sem;
    ErrorMessage += "\"less than 0\n";
    cout << ErrorMessage << endl;
    exit(1);
}
void Error_ArraySubscriptOutBounds(int line, string sem1, string sem2)
{ // 2. �����±�Խ�磬��Ӧ�����������(4)
    string ErrorMessage = "Line:";
    ErrorMessage += to_string(line);
    ErrorMessage += ",   array index is illegal(\"";
    ErrorMessage += sem1;
    ErrorMessage += "\" > \"";
    ErrorMessage += sem2;
    ErrorMessage += "\")\n";
    cout << ErrorMessage << endl;
    exit(1);
}

void Error_ArrayNotArrayType(int line, string sem)
{ // 3.body��ʹ�õı�ʶ�����������ʶ�������������������(3)
    string ErrorMessage = "Line:";
    ErrorMessage += to_string(line);
    ErrorMessage += ",   var\"";
    ErrorMessage += sem;
    ErrorMessage += "\"is not array type\n";
    cout << ErrorMessage << endl;
    exit(1);
}

void Error_ArrayGroupInvalid(int line, string sem)
{ // 3.�����Ա���������ò��Ϸ������������������(5)
    string ErrorMessage = "Line:";
    ErrorMessage += to_string(line);
    ErrorMessage += ",   array\"";
    ErrorMessage += sem;
    ErrorMessage += "\"usage is illegal\n";
    cout << ErrorMessage << endl;
    exit(1);
}

/***��¼***/
void Error_RecordFieldInvalid(int line, string sem1, string sem2)
{ // 1.��¼�����Ա���������ò��Ϸ������������������(5)
    string ErrorMessage = "Line:";
    ErrorMessage += to_string(line);
    ErrorMessage += ",   record\"";
    ErrorMessage += sem1;
    ErrorMessage += "\"feild var is not exist\"";
    ErrorMessage += sem2;
    ErrorMessage += "\"\n";
    cout << ErrorMessage << endl;
    exit(1);
}

void Error_RecordFieldArrayInvalid(int line, string Rsem1, string Asem2)
{ // 2.��¼�������������Ա���������ò��Ϸ������������������(5)
    string ErrorMessage = "Line:";
    ErrorMessage += to_string(line);
    ErrorMessage += ",   record\"";
    ErrorMessage += Rsem1;
    ErrorMessage += "\"array member\"";
    ErrorMessage += Asem2;
    ErrorMessage += "\" usage is illegal\n";
    cout << ErrorMessage << endl;
    exit(1);
}

void Error_RecordFieldNotArrayType(int line, string Rsem1, string Asem2)
{ // 3.body��ʹ�õ����Ա�������������ʶ�������������������(5)
    string ErrorMessage = "Line:";
    ErrorMessage += to_string(line);
    ErrorMessage += ",   record\"";
    ErrorMessage += Rsem1;
    ErrorMessage += "\"the member\"";
    ErrorMessage += Asem2;
    ErrorMessage += "\"is not array type , is illegal\n";
    cout << ErrorMessage << endl;
    exit(1);
}

void Error_RecordNotRecordType(int line, string sem)
{ // 4.body��ʹ�õı�ʶ�����Ǽ�¼��ʶ�������������������(3)
    string ErrorMessage = "Line:";
    ErrorMessage += to_string(line);
    ErrorMessage += ",  var\"";
    ErrorMessage += sem;
    ErrorMessage += "\"is not record type\n";
    cout << ErrorMessage << endl;
    exit(1);
}

/***���̵���***/
void Error_ProcParamType(int line, int ParamSerial, string ProcName)
{ // 1.���̵�������У���ʵ�����಻ƥ�䣬��Ӧ�����������(8)
    string ErrorMessage = "Line:";
    ErrorMessage += to_string(line);
    ErrorMessage += ",   procedure invoke\"";
    ErrorMessage += ProcName;
    ErrorMessage += "\"��, �� ";
    ErrorMessage += to_string(ParamSerial);
    ErrorMessage += " parameters number is not right\n";
    cout << ErrorMessage << endl;
    exit(1);
}

void Error_ProcParamAmount(int line, string sem)
{ // 2.���̵�������У���ʵ�θ�������ͬ����Ӧ�����������(9)
    string ErrorMessage = "Line:";
    ErrorMessage += to_string(line);
    ErrorMessage += ",   procedure invoke\"";
    ErrorMessage += sem;
    ErrorMessage += "\"parameter number is not right\n";
    cout << ErrorMessage << endl;
    exit(1);
}

void Error_ProcNotProcIdentifier(int line, string sem)
{ // 3.���̵�������У���ʶ�����ǹ��̱�ʶ������Ӧ�����������(10)
    string ErrorMessage = "Line:";
    ErrorMessage += to_string(line);
    ErrorMessage += ",   signal\"";
    ErrorMessage += sem;
    ErrorMessage += "\"is not a procedure signal\n";
    cout << ErrorMessage << endl;
    exit(1);
}

/***��ֵ***/
void Error_AssignContentIncompatible(int line, string sem)
{ // 1.��ֵ����У������������Ͳ����ݣ���Ӧ�����������(6)
    string ErrorMessage = "Line:";
    ErrorMessage += to_string(line);
    ErrorMessage += ",   signal\"";
    ErrorMessage += sem;
    ErrorMessage += "\"is not consistant with the object to assign\n";
    cout << ErrorMessage << endl;
    exit(1);
}

void Error_AssignNotVarIdentifier(int line, string sem)
{ // 2.��ֵ����У���ʶ�����Ǳ�����ʶ������Ӧ�����������(7)
    string ErrorMessage = "Line:";
    ErrorMessage += to_string(line);
    ErrorMessage += ",   signal\"";
    ErrorMessage += sem;
    ErrorMessage += "\"is not var signal\n";
    cout << ErrorMessage << endl;
    exit(1);
}

/***���ʽ***/
void Error_StmOpComponentIncompatibility(int line, string sem)
{ // 1.�����ֵ�α��ʽ��������ķ��������Ͳ����ݣ���Ӧ�����������(12)
    string ErrorMessage = "Line:";
    ErrorMessage += to_string(line);
    ErrorMessage += ",  computer signal\"";
    ErrorMessage += sem;
    ErrorMessage += "\"type is not right\n";
    cout << ErrorMessage << endl;
    exit(1);
}

/***�������***/
void Error_StmCompareComponentIncompatibility(int line, string sem)
{ // 1.�������ʽ��������ķ��������Ͳ����ݣ���Ӧ�����������(11)
    string ErrorMessage = "Line:";
    ErrorMessage += to_string(line);
    ErrorMessage += ",  compare signal\"";
    ErrorMessage += sem;
    ErrorMessage += "\"the result is illegal\n";
    cout << ErrorMessage << endl;
    exit(1);
}

// ��дD�� procDecPart, ����������еı�ʶ�������ͱ�ʶ����������ʶ�������̱�ʶ����
void procDecPartParsing(treenode *root)
{ // ���ڵ�����Ϊ"ProcDecPart"����ӦRD�е�"procDecPart����"
    if (root == NULL)
    {
        return;
    }
    // root->child[0], procDec()
    declarePartParsing(root->child[0]);
}
void procBodyParsing(treenode *root)
{ // ���ڵ�����Ϊ"ProcBody"����ӦRD�е�"procBody����"
    if (root == NULL)
    {
        return;
    }
    // root->child[0], programBody()
    programBodyParsing(root->child[0]);
}
void fidMoreParsing(treenode *root)
{ // ���ڵ�����Ϊ"FidMore"����ӦRD�е�"fidMore����"
    if (root == NULL)
    {
        return;
    }
    // root->child[0]: COMMA, �޲���;
    // root->child[1]: formList(),������ȡID��Token
    formListParsing(root->child[1]);
}
// ���̲�����ID���õ�Token
void formListParsing(treenode *root)
{ // ���ڵ�����Ϊ"FormList"����ӦRD�е�"formList����"
    if (root == NULL)
    {
        return;
    }
    // root->child[0]: ID
    Tok_form.push_back(root->child[0]->token);
    // root->child[1]: fidMore());
    fidMoreParsing(root->child[1]);
}

// �˺�������һ�����̲���������Ϊ��Ҫ���ǵ�������Ĺ��̵���ʱ����������ƥ�䣬��������˴���TypeΪ�Ƿ���Ҳ��ѹ����ű�Type������ΪNULL��������Щ��ʶ������ʾ�ڷ��ű���
struct ParamTable *paramParsing(treenode *root)
{ // ���ڵ�����Ϊ"Param"����ӦRD�е�"param����"
    if (root == NULL)
    {
        return NULL;
    }
    unsigned int VarExist = 0;
    if (root->child[0]->str == "TypeDef")
    {
        VarExist = 0; // DIR
    }
    else
    {
        VarExist = 1; // INDIR
    }
    // root->child[0+ VarExist]: typeDef()
    AttributeIR *tempAttr = typeDefParsing(root->child[0 + VarExist]); // �ҵ�����Ҫ������
    if (tempAttr == NULL)
    { // ���û���ҵ���Ӧ��ʶ�����򴴽�һ��TypeΪNULL������
        tempAttr = new AttributeIR;
        tempAttr->idtype = NULL;
    }
    tempAttr->kind = VARKIND;
    tempAttr->More.VarAttr.level = ValidTableCount;
    if (VarExist == 0)
    {
        tempAttr->More.VarAttr.access = DIR;
    }
    else
    {
        tempAttr->More.VarAttr.access = INDIR;
    }

    SymbTable *Symtemp = FindEntry("*", false, VARKIND, -1); // �ҵ���ǰ���ű����µ�һ��������ʶ������ƫ�Ƽ���

    int Off = 0; // ���㵱ǰ���ű��ڵ���ƫ�ƣ���������ڹ��̵�ƫ��=����size+��ƫ��
    if (Symtemp != NULL)
    {
        unsigned int size = 0;
        if (Symtemp->attrIR.idtype != NULL)
        { // ���idtypeΪNULL�� �����ΪsizeΪ0
            if (Symtemp->attrIR.More.VarAttr.access == DIR)
            {
                size = Symtemp->attrIR.idtype->size;
            }
            else
            {
                size = TypeList[INTTY - '0']->size; // ���̵ļ�δ�С�̶�Ϊ�������ʹ�С
            }
        }
        else
        {
            size = 0;
        }
        Off = Symtemp->attrIR.More.VarAttr.off + size;
    }
    else
    {
        Off = 0;
    }
    struct ParamTable *temp = NULL;
    struct ParamTable *rear = NULL;
    // root->child[1 + VarExist]: formList()
    formListParsing(root->child[1 + VarExist]); // Tok_form.size()һ������0�����򲻿���ͨ���﷨����
    const int size_origion = scope[scope.size() - 1].size();
    const int Token_size = Tok_form.size();
    int Token_Off = 0; // Token�����е�ƫ��
    int VarSize = 0;   // �����ʶ����size
    if (tempAttr->idtype != NULL)
    {
        if (VarExist == 0)
        {
            VarSize = tempAttr->idtype->size;
        }
        else
        {
            VarSize = TypeList[INTTY - '0']->size; // ���̵ļ�δ�С�̶�Ϊ�������ʹ�С
        }
    }
    for (int ix = 0; ix < Token_size; ix++)
    {
        tempAttr->More.VarAttr.off = Off + Token_Off; // ���ű���ƫ�Ƽ��ϱ�ʶ����Token�����е�ƫ��
        Token_Off += VarSize;                         // Tokenƫ�Ƽ��ϴ����ʶ���Ĵ�С

        if (Enter(Tok_form[ix]->Sem, *tempAttr, VARKIND, -1) == false)
        {                                                                        // ѹ�����ջ
            Error_IdentifierDuplicateDec(Tok_form[ix]->Line, Tok_form[ix]->Sem); // ��ʶ��������
            Enter(WRONGID, *tempAttr, VARKIND, -1);                              // ��WrongID������ű���ʾ��ʶ�������Ƿ�
        }

        struct ParamTable *tempptr = new struct ParamTable;
        tempptr->next = NULL;
        if (temp == NULL)
        {
            temp = tempptr;
        }
        else
        {
            rear->next = tempptr;
        }
        rear = tempptr;
        rear->entry = ix + size_origion; // �˷���ջ��Ԫ�ص�������ΪĿǰ�����ھֲ������е�λ��(��Token�е�λ��ix + �ֲ�������ԭ���ı�ʶ��������)
    }
    Tok_form.clear();
    return temp;
}
struct ParamTable *paramMoreParsing(treenode *root)
{ // ���ڵ�����Ϊ"ParamMore"����ӦRD�е�"paramMore����"
    if (root == NULL)
    {
        return NULL;
    }
    // root->child[0]: SEM, �޲���;
    // root->child[1]: paramDecList()
    return paramDecListParsing(root->child[1]);
}
struct ParamTable *paramDecListParsing(treenode *root)
{ // ���ڵ�����Ϊ"ParamDecList"����ӦRD�е�"paramDecList����"
    if (root == NULL)
    {
        return NULL;
    }
    // root->child[0]: param()
    struct ParamTable *temp = paramParsing(root->child[0]);
    // root->child[1]: paramMore()
    if (temp != NULL)
    { // tmep���趨Ϊ������Ϊ�գ�paramParsing��������ֵ��Ϊ���Ϊ�ǿգ����˴������������
        struct ParamTable *rearptr = temp;
        while (rearptr->next != NULL)
        { // �ҵ���������һ���ڵ㣬Ȼ��������paramMoreParsing()
            rearptr = rearptr->next;
        }
        rearptr->next = paramMoreParsing(root->child[1]);
    }
    else
    {
        temp = paramMoreParsing(root->child[1]);
    }

    return temp;
}
// �����̲���ѹ��scopeջ�������ع��̲�����
struct ParamTable *paramListParsing(treenode *root)
{ // ���ڵ�����Ϊ"ParamList"����ӦRD�е�"paramList����"
    if (root == NULL)
    {
        return NULL;
    }
    return paramDecListParsing(root->child[0]);
}

// ����������
void procDecParsing(treenode *root)
{ // ���ڵ�����Ϊ"ProcDec"����ӦRD�е�"procDec����"
    if (root == NULL)
    {
        return;
    }
    SymbTable temp;
    // root->child[0]: PROCEDURE, �޲���
    // root->child[1]: ID
    temp.idname = root->child[1]->token->Sem;              // idname
    temp.attrIR.idtype = NULL;                             // Typeptr
    temp.attrIR.kind = PROCKIND;                           // kind
    temp.attrIR.More.ProcAttr.level = ValidTableCount - 1; // level��������������һ��

    if (Enter(temp.idname, temp.attrIR, PROCKIND, temp.attrIR.More.ProcAttr.level) == false)
    {                                                                                          // ѹ�����ջ���迼�ǹ��̵�Level
        Error_IdentifierDuplicateDec(root->child[1]->token->Line, root->child[1]->token->Sem); // ��ʶ��������
        const int scope_origin_size = scope.size() - 1;                                        // ��Ϊ��ǰ�Ѿ������˴˹��̵ķ��ű�����scopeԭ����size�������ڵ�size-1
        // �ȼ����̶����е��е�����������Ƴ��ֲ�����
        // root->child[6]: procDecPart()
        procDecPartParsing(root->child[6]);
        // root->child[7]: procBody()
        procBodyParsing(root->child[7]);
        // root->child[8]: procDecpart()
        procDecpartParsing(root->child[8]);
        // ������̱�־���������µľֲ�����Ϊ������̵ı�ʶ�������Դ˹��̵ľֲ������Լ���body�ж����ȫ���ֲ�����Ӧ��ȫ����ɾ��
        const int times = scope.size() - scope_origin_size; // ͨ��scopeԭ����size����scopeջ��ԭ�ؼ���˾ֲ�����ԭ�������ӣ���Ҫ��scopeջ�Ĵ���
        for (int ix = 0; ix < times; ix++)
        {
            scope.pop_back();
            exit_region.pop_back();
        }
    }
    else
    {
        // root->child[2]: LPAREN, �޲���
        // root->child[3]: paramList()
        const int scope_size = scope.size();
        scope[scope_size - 1][0].attrIR.More.ProcAttr.param = paramListParsing(root->child[3]);
        scope[scope_size - 1][0].attrIR.More.ProcAttr.code = 0; // Code,���̵�Ŀ������ַ���������ɽ׶���д�����ڳ�ʼ��Ϊ0��
        // root->child[4]: RPAREN, �޲���
        // root->child[5]: SEMI, �޲���
        // root->child[6]: procDecPart()
        procDecPartParsing(root->child[6]);
        // ����Size
        // scope[scope_size - 1][0]��ʾ������ű�Ĺ��̱�ʶ��
        scope[scope_size - 1][0].attrIR.More.ProcAttr.size = 0;
        for (int ix = scope[scope_size - 1].size() - 1; ix >= 1; ix--)
        {
            if (scope[scope_size - 1][ix].attrIR.kind == VARKIND)
            {
                int size = 0; // ��ʾ������̵����һ��������ʶ���Ĵ�С
                if (scope[scope_size - 1][ix].attrIR.idtype != NULL)
                {
                    size = scope[scope_size - 1][ix].attrIR.idtype->size;
                }
                scope[scope_size - 1][0].attrIR.More.ProcAttr.size += size;
            }
        };
        /*SymbTable* SymPtr = FindEntry("*", false, scope, exit_region, VARKIND, -1);//�ҵ���ǰ���ű����µ�һ��������ʶ������size����
        if (SymPtr == NULL) {//�˱�ʶ��Ϊ��ǰ���ű����µ�һ��������ʶ����size����Ϊ0
            scope[scope_size - 1][0].attrIR.More.ProcAttr.size = 0;
        }
        else {//������ڵ�ǰ���ű����µ�һ��������ʶ����size+ƫ��
            unsigned int size = 0;
            if (SymPtr->attrIR.idtype != NULL) {//���idtypeΪNULL�� �����ΪsizeΪ0
                size = SymPtr->attrIR.idtype->size;
            }
            else {
                size = 0;
            }
            scope[scope_size - 1][0].attrIR.More.ProcAttr.size = SymPtr->attrIR.More.VarAttr.off + size;
        }*/
        // root->child[7]: procBody()
        procBodyParsing(root->child[7]);
        // root->child[8]: procDecpart()
        procDecpartParsing(root->child[8]);
    }
}
void procDecpartParsing(treenode *root)
{ // ���ڵ�����Ϊ"ProcDecPart"����ӦRD�е�"procDecpart����"
    if (root == NULL)
    {
        return;
    }
    CreateTable(); // procDecpart,�½�һ���շ��ű�
    procDecParsing(root->child[0]);
    return;
}

void varIDMoreParsing(treenode *root)
{ // ���ڵ�����Ϊ"VarIDMore"����ӦRD�е�"varIDMore����"
    if (root == NULL)
    {
        return;
    }
    // root->child[0]: COMMA, �޲���
    // root->child[1]: IDList()
    varIDListParsing(root->child[1]);
    return;
}
void varIDListParsing(treenode *root)
{ // ���ڵ�����Ϊ"VarIDList"����ӦRD�е�"varIDList����"
    // root->child[0]: ID
    Tok_ID.push_back(root->child[0]->token);
    // root->child[1]: varIDMore()
    varIDMoreParsing(root->child[1]);
    return;
}
void varDecMoreParsing(treenode *root)
{ // ���ڵ�����Ϊ"VarDecMore"����ӦRD�е�"varDecMore����"
    if (root == NULL)
    {
        return;
    }
    varDecListParsing(root->child[0]);
}
// �����������
void varDecListParsing(treenode *root)
{ // ���ڵ�����Ϊ"VarDecList"����ӦRD�е�"varDecList����"
    if (root == NULL)
    {
        return;
    }
    // root->child[0]: typeDef()
    AttributeIR *Attr = typeDefParsing(root->child[0]);
    if (Attr != NULL)
    {
        Attr->kind = VARKIND;                       // ������ʶ��
        Attr->More.VarAttr.access = DIR;            // ֱ�ӱ���
        Attr->More.VarAttr.level = ValidTableCount; // ����
        // root->child[1]: varIDList()

        varIDListParsing(root->child[1]);
        int Token_size = Tok_ID.size();
        for (int ix = 0; ix < Token_size; ix++)
        {
            SymbTable *Symtemp = FindEntry("*", false, VARKIND, -1); // �ҵ���ǰ���ű����µ�һ��������ʶ������ƫ�Ƽ���
            if (Symtemp == NULL)
            { // �˱�ʶ��Ϊ��ǰ���ű����µ�һ��������ʶ����ƫ������Ϊ0
                Attr->More.VarAttr.off = 0;
            }
            else
            { // ������ڵ�ǰ���ű����µ�һ��������ʶ����size+ƫ��
                int size = 0;
                if (Symtemp->attrIR.idtype != NULL)
                {
                    size = Symtemp->attrIR.idtype->size;
                }
                Attr->More.VarAttr.off = Symtemp->attrIR.More.VarAttr.off + size;
            }
            // cout << Tok_ID[ix]->Sem << "  out" << endl;
            // ����ű��ڲ������
            if (Enter(Tok_ID[ix]->Sem, *Attr, VARKIND, -1) == false)
            {
                // cout << Tok_ID[ix]->Sem << "  in" << endl;
                Error_IdentifierDuplicateDec(Tok_ID[ix]->Line, Tok_ID[ix]->Sem); // ��ʶ��������
            }
        }
    }
    Tok_ID.clear();
    // root->child[2]: SEMI, �޲���
    // root->child[3]: varDecMore()
    varDecMoreParsing(root->child[3]);
}
void varDecParsing(treenode *root)
{ // ���ڵ�����Ϊ"VarDec"����ӦRD�е�"varDec����"
    if (root == NULL)
    {
        return;
    }
    // root->child[0]: VAR, �޲���
    // root->child[1]: varDecList()
    varDecListParsing(root->child[1]);
}
void varDecPartParsing(treenode *root)
{ // ���ڵ�����Ϊ"VarDecPart"����ӦRD�е�"varDecPart����"
    if (root == NULL)
    {
        return;
    }
    varDecParsing(root->child[0]);
    return;
}

AttributeIR *baseTypeParsing(treenode *root)
{ // ���ڵ�����Ϊ"BaseType"����ӦRD�е�"baseType����"
    if (root == NULL)
    {
        return NULL;
    }
    AttributeIR *temp = NULL;
    if (root->child[0]->token->Lex == INTEGER)
    {
        temp = new AttributeIR;
        temp->kind = TYPEKIND;
        temp->idtype = TypeList[INTTY - '0'];
    }
    else if (root->child[0]->token->Lex == CHAR)
    {
        temp = new AttributeIR;
        temp->kind = TYPEKIND;
        temp->idtype = TypeList[CHARTY - '0'];
    }
    return temp;
}

// �����ͱ������Type�����Ƿ��Ѿ����ڴ��ڣ����ڵĻ��������ַ�����򷵻ؿ�
// Typekind�Ǵ����ʶ�������ͣ��������ͻ��߼�¼���ͣ�
struct TypeIR *WhetherTypeDuplicate(struct TypeIR *Type, char Typekind)
{
    if (Typekind == ARRAYTY)
    {
        for (int ix = TypeList.size() - 1; ix >= 0; ix--)
        {
            // �������ͣ��������Է�ȫ����ͬ
            if (TypeList[ix]->size == Type->size && TypeList[ix]->Typekind == Type->Typekind)
            {
                if (TypeList[ix]->More.elemTy == Type->More.elemTy && TypeList[ix]->More.indexTy == Type->More.indexTy && TypeList[ix]->More.Low == Type->More.Low)
                {
                    return TypeList[ix];
                }
            }
        }
        return NULL;
    }
    else if (Typekind == RECORDTY)
    {
        for (int ix = TypeList.size() - 1; ix >= 0; ix--)
        {
            // ��¼���ͣ��������Է�ȫ����ͬ
            if (TypeList[ix]->size == Type->size && TypeList[ix]->Typekind == Type->Typekind)
            {
                fieldChain *body1 = TypeList[ix]->More.body;
                fieldChain *body2 = Type->More.body;
                bool flag = true;
                // ����body�����Ƿ�ȫ����ͬ
                while (body1 != NULL && body2 != NULL && flag == true)
                {
                    if (body1->idname != body2->idname || body1->offset != body2->offset || body1->unitType != body2->unitType)
                    {
                        flag = false;
                    }
                    body1 = body1->next;
                    body2 = body2->next;
                }
                if (body1 == NULL && body2 == NULL && flag == true)
                {
                    return TypeList[ix];
                }
            }
        }
        return NULL;
    }
    else
    {
        return NULL;
    }
}
AttributeIR *arrayTypeParsing(treenode *root)
{ // ���ڵ�����Ϊ"ArrayType"����ӦRD�е�"arrayType����"
    if (root == NULL)
    {
        return NULL;
    }
    AttributeIR *temp = new AttributeIR;
    temp->kind = TYPEKIND;
    // START �½�һ��TypeIR������Ǵ����͵Ļ���������TypeList�Ļ��������TypeList�������TypeIR��ֵ�Ѿ�������TypeList�����͵ĵ�ַ------------------------------------------------------------------------------------
    TypeIR *tempTypeIR = new TypeIR;
    tempTypeIR->Typekind = ARRAYTY;                   // ��������Typeind
    tempTypeIR->More.indexTy = TypeList[INTTY - '0']; // �����±�����indexTYһ������������
    // root->child[0]: ARRAY, �޲���
    // root->child[1]: LMIDPAREN('['), �޲���
    // root->child[2]:INTC
    string str1 = root->child[2]->token->Sem;
    auto str1_n = atoi(str1.c_str());
    if (str1_n < 0)
    {
        Error_ArraySubscriptLessThanZero(root->child[2]->token->Line, root->child[2]->token->Sem); // ��������ʱ�±�С��0
        delete temp;
        return NULL;
    }
    else
    {
        tempTypeIR->More.Low = str1_n; // ��������Low
    }
    // root->child[3]: UNDERANGE(".."), �޲���
    // root->child[4]: ���ں��漸�в���
    // root->child[5]: RMIDPAREN(']'), �޲���
    // root->child[6]: OF, �޲���
    // root->child[7]: baseType()
    if (root->child[7]->child[0]->token->Lex == INTEGER)
    { // ��������elemTY
        tempTypeIR->More.elemTy = TypeList[INTTY - '0'];
    }
    else if (root->child[7]->child[0]->token->Lex == CHAR)
    {
        tempTypeIR->More.elemTy = TypeList[CHARTY - '0'];
    }
    // root->child[4]:INTCstring str2 = root->child[2]->Token->Sem;
    string str2 = root->child[4]->token->Sem;
    auto str2_n = std::strtol(str2.data(), nullptr, 10); // �ַ���ת ����Ϊ10 ������
    if (str2_n < 0)
    {
        Error_ArraySubscriptLessThanZero(root->child[4]->token->Line, root->child[4]->token->Sem); // ��������ʱ�±�С��0
        delete temp;
        return NULL;
    }
    else if (str2_n < str1_n)
    {
        Error_ArraySubscriptOutBounds(root->child[2]->token->Line, root->child[2]->token->Sem, root->child[4]->token->Sem); // �����±�Խ��
        delete temp;
        return NULL;
    }
    else
    {
        tempTypeIR->size = (str2_n - str1_n + 1) * tempTypeIR->More.elemTy->size; // ��������size = ����Ԫ�ظ��� * ����Ԫ�سߴ�
    }
    tempTypeIR->serial = TypeList.size();

    struct TypeIR *existTy = WhetherTypeDuplicate(tempTypeIR, ARRAYTY);
    if (existTy == NULL)
    {
        TypeList.push_back(tempTypeIR);
        temp->idtype = TypeList[TypeList.size() - 1];
    }
    else
    {
        temp->idtype = existTy;
    }
    // END �½�һ��TypeIR������Ǵ����͵Ļ���������TypeList�Ļ��������TypeList�������TypeIR��ֵ�Ѿ�������TypeList�����͵ĵ�ַ------------------------------------------------------------------------------------

    return temp;
}

void IDMoreParsing(treenode *root)
{ // ���ڵ�����Ϊ"IDMore"����ӦRD�е�"IDMore����"
    if (root == NULL)
    {
        return;
    }
    // root->child[0]: COMMA, �޲���
    // root->child[1]: IDList()
    IDListParsing(root->child[1]);
    return;
}
void IDListParsing(treenode *root)
{ // ���ڵ�����Ϊ"IDList"����ӦRD�е�"IDList����"��StrVec���������Ҫ��ID��
    if (root == NULL)
    {
        return;
    }
    // root->child[0]: ID
    Tok.push_back(root->child[0]->token);
    // root->child[1]: IDMore()
    IDMoreParsing(root->child[1]);
    return;
}
fieldChain *fieldDecMoreParsing(treenode *root)
{ // ���ڵ�����Ϊ"FieldDecMore"����ӦRD�е�"fieldDecMore����"
    if (root == NULL)
    {
        return NULL;
    }
    // root->child[0]:fieldDecList()
    return fieldDecListParsing(root->child[0]);
}
fieldChain *fieldDecListParsing(treenode *root)
{ // ���ڵ�����Ϊ"FieldDecList"����ӦRD�е�"fieldDecList����"
    if (root == NULL)
    {
        return NULL;
    }
    fieldChain *temp = NULL;
    // root->child[0]: BaseType �� ArrayType,����unitType
    if (root->child[0]->child[0]->token->Lex == INTEGER || root->child[0]->child[0]->token->Lex == CHAR)
    {
        temp = new fieldChain;
        if (root->child[0]->child[0]->token->Lex == INTEGER)
        {
            temp->unitType = TypeList[INTTY - '0']; // ָ��INTGER����
        }
        else
        {                                            // root->child[0]->child[0]->Token->Lex == CHAR
            temp->unitType = TypeList[CHARTY - '0']; // ָ��CHAR����
        }
    }
    else if (root->child[0]->child[0]->token->Lex == ARRAY)
    {
        temp = new fieldChain;
        AttributeIR *tempAttr = arrayTypeParsing(root->child[0]);
        if (tempAttr == NULL)
        {
            delete tempAttr;
            delete temp;
            return NULL;
        }
        else
        {
            temp->unitType = tempAttr->idtype;
            delete tempAttr;
        }
    }
    else
    {
        return NULL;
    }
    // root->child[1]:IDList(),����idname
    int Token_size1 = Tok.size();  // Tokenδ����ǰ
    IDListParsing(root->child[1]); // �õ�IDList�е�����Token
    if (Tok.size() - Token_size1 == 0)
    { // ID����Ϊ0
        delete temp;
        temp = NULL;
    }

    if (temp == NULL)
    { // ID����Ϊ0
        return fieldDecMoreParsing(root->child[3]);
    }
    else
    {
        temp->idname = Tok[Token_size1 + 0]->Sem;
        fieldChain *rear = temp;
        for (int ix = Token_size1 + 1; ix < Tok.size(); ix++)
        { // ������Token������ΪfieldChain������������
            fieldChain *tempID = new fieldChain;
            tempID->unitType = temp->unitType; // ����ID������ͬ������
            tempID->idname = Tok[ix]->Sem;
            rear->next = tempID;
            rear = tempID;
        }
        // root->child[2]:SEMI���޲���
        // root->child[3]:fieldDecMore()
        rear->next = fieldDecMoreParsing(root->child[3]);
        return temp;
    }
}
AttributeIR *recTypeParsing(treenode *root)
{ // ���ڵ�����Ϊ"RecType"����ӦRD�е�"recType����"
    if (root == NULL)
    {
        return NULL;
    }
    AttributeIR *temp = new AttributeIR;
    temp->kind = TYPEKIND;
    // START �½�һ��TypeIR������Ǵ����͵Ļ���������TypeList�Ļ��������TypeList�������TypeIR��ֵ�Ѿ�������TypeList�����͵ĵ�ַ------------------------------------------------------------------------------------
    TypeIR *tempTypeIR = new TypeIR;
    tempTypeIR->Typekind = RECORDTY; // ��¼����Typeind
    // root->child[0]: RECORD, �޲���
    // root->child[1]: fieldDecList()
    fieldChain *body = fieldDecListParsing(root->child[1]);
    // root->child[2]: END, �޲���
    if (body == NULL)
    {
        delete temp;
        temp = NULL;
        return NULL;
    }
    fieldChain *head = body;
    unordered_map<string, int> UnMap;
    bool flag = false;
    for (int ix = 0; head != NULL && ix < Tok.size(); ix++)
    {                              // Token��body�ڵ�һ��һһ��Ӧ
        string str = head->idname; // ���ַ�����ת����string����
        if (UnMap.find(str) != UnMap.end())
        {                                                                    // ID����Ѿ���map�ڣ����ʾRecord�б�ʶ���ظ�����
            Error_IdentifierDuplicateDecRecord(Tok[ix]->Line, Tok[ix]->Sem); // Record�б�ʶ���ظ�����
            flag = true;
        }
        else
        {
            UnMap[str] = 0; // ID��Map
        }
        head = head->next;
    }
    if (flag == true)
    {
        delete temp;
        temp = NULL;
        return NULL;
    }
    tempTypeIR->More.body = body; // ��¼����body
    int size = 0;
    int offset = 0;
    while (body != NULL)
    {
        size += body->unitType->size;
        body->offset = offset;
        offset = size;
        body = body->next;
    }
    tempTypeIR->size = size; // ��¼����size
    tempTypeIR->serial = TypeList.size();

    struct TypeIR *existTy = WhetherTypeDuplicate(tempTypeIR, RECORDTY);
    if (existTy == NULL)
    {
        TypeList.push_back(tempTypeIR);
        temp->idtype = TypeList[TypeList.size() - 1];
    }
    else
    {
        temp->idtype = existTy;
    }
    // END �½�һ��TypeIR������Ǵ����͵Ļ���������TypeList�Ļ��������TypeList�������TypeIR��ֵ�Ѿ�������TypeList�����͵ĵ�ַ------------------------------------------------------------------------------------
    Tok.clear();
    return temp;
}

AttributeIR *structureTypeParsing(treenode *root)
{ // ���ڵ�����Ϊ"StructureType"����ӦRD�е�"structureType����"
    if (root == NULL)
    {
        return NULL;
    }
    AttributeIR *temp = NULL;
    if (root->child[0]->child[0]->token->Lex == ARRAY)
    {
        // root->child[0]: arrayType();
        temp = arrayTypeParsing(root->child[0]);
    }
    else if (root->child[0]->child[0]->token->Lex == RECORD)
    {
        // root->child[0]: recType();
        temp = recTypeParsing(root->child[0]);
    }
    return temp;
}

SymbTable *SearchSingleTable(const string &id, int level, const char kind, const int ProcLevel)
{ // ���ұ�ʶ���Ƿ�������Ӧ����,kind��ʾ�����ʶ��������('*'��ʾȫ��ƥ��),�����ǹ��̱�ʶ����Level�Ļ���Ҫ��ProcLevelС����
    int position = level - 1;
    SymbTable *temp = NULL;
    for (int ix = scope[position].size() - 1; ix >= 0; ix--)
    {
        if (scope[position][ix].attrIR.kind != PROCKIND)
        { // 1. id ����"*"����idname; 2. kind����'*'����attrIR.kind; 3.���ǹ��̱�ʶ��
            if ((id == "*" || scope[position][ix].idname == id) && (scope[position][ix].attrIR.kind == kind || kind == '*'))
            {
                temp = &(scope[position][ix]);
                return temp; // ���������ʶ���ĵ�ַ
            }
        }
        else
        { // 1. �ǹ��̱�ʶ��; 2. id ����"*"����idname; 3. kind����'*'����attrIR.kind;  4.  Level��ͬ����ProcLevel<0
            if (("*" == id || scope[position][ix].idname == id) && (scope[position][ix].attrIR.kind == kind || kind == '*') && (scope[position][ix].attrIR.More.ProcAttr.level == ProcLevel || ProcLevel < 0))
            {
                temp = &(scope[position][ix]);
                return temp; // ���������ʶ���ĵ�ַ
            }
        }
    }
    return temp; // NULL: �����ڣ� �ǿ�: ��Ӧ��ʶ����ַ��Ϣ
}
SymbTable *FindEntry(const string &id, bool flag, const char kind, const int ProcLevel)
{                                   // flag == false:�ڵ�ǰ���ű��в��ң� flag == true:������scopeջ�в���
    int level = exit_region.size(); // ��ʾ�ڼ��㣬level == 0�൱��scopeջ����Ԫ�ء�ʹ�� n = level-1 ����scope[n][x]����exit_region[n]��
    SymbTable *temp = NULL;
    // ���ڵ�ǰ���ű��в���
    while (level > 0)
    {
        if (exit_region[level - 1] == false)
        { // �ҵ���һ��δ�˳��ľֲ������Ĳ���
            break;
        }
        else
        {
            level--;
        }
    }
    if (level <= 0)
    { // ������һ����Ч�ľֲ�����
        return NULL;
    }
    temp = SearchSingleTable(id, level, kind, ProcLevel); // ���ҵ�ǰ���ű�
    if (flag == true && temp == NULL)
    { // ���flag == true���ҵ�ǰ���ű���δ���ҵ������ʶ��id���������������scopeջ
        level--;
        while (level > 0)
        {
            if (exit_region[level - 1] == false || (kind == PROCKIND && ProcLevel >= 0))
            {                                                         // �ҵ�δ�˳��ľֲ������Ĳ���������ǲ��ҹ��̱�ʶ�����ض��������kind == PROCKIND && ProcLevel>=0������һֱ�������
                temp = SearchSingleTable(id, level, kind, ProcLevel); // ���Ҵ˷��ű�
                if (temp != NULL)
                {
                    break;
                }
            }
            level--;
        }
    }
    return temp; // NULL: �����ڣ� �ǿ�: ��Ӧ��ʶ����ַ��Ϣ
}
AttributeIR *typeDefParsing(treenode *root)
{ // ���ڵ�����Ϊ"TypeDef"����ӦRD�е�"typeDef����"
    if (root == NULL)
    {
        return NULL;
    }
    AttributeIR *temp = NULL;
    if (root->child[0]->str == "BaseType")
    {
        temp = baseTypeParsing(root->child[0]);
    }
    else if (root->child[0]->str == "StructureType")
    {
        temp = structureTypeParsing(root->child[0]);
    }
    else
    {
        SymbTable *Sym = FindEntry(root->child[0]->token->Sem, true, TYPEKIND, -1);
        if (Sym == NULL)
        { // ���δ�ҵ��˱�ʶ��
            if (FindEntry(root->child[0]->token->Sem, true, '*', -1) == NULL)
            { // δ������ʶ��
                Error_IdentifierUndeclared(root->child[0]->token->Line, root->child[0]->token->Sem);
            }
            else
            {
                Error_IdentifierUndeclaredType(root->child[0]->token->Line, root->child[0]->token->Sem); // δ�������ͱ�ʶ��
            }
        }
        else
        {
            temp = new AttributeIR;
            *temp = Sym->attrIR;
        }
    }
    return temp;
}

bool Enter(const string &id, AttributeIR Attrib, char kind, const int ProcLevel)
{                         // �ǼǱ�ʶ���������Ե����ű�,ע��������ǹ��̱�ʶ���Ļ�, ProcLevel�����壬����ֱ�Ӵ�ProcLevel = -1
    bool present = false; // ��¼�Ǽ��Ƿ�ɹ�
    bool flag = false;    // FindEntry�Ĵ��Σ���ʾֻ��ѯ��ǰ���ű���ȫ�����ű�
    if (kind == PROCKIND && ProcLevel >= 0)
    {
        flag = true;
    }
    else
    {
        flag = false;
    }
    if (FindEntry(id, flag, kind, ProcLevel) == NULL || id == WRONGID)
    { // �����WrongID������ű���ʾ��ʶ�������Ƿ�������Ҳ������ű������ֻ���ڹ��̲�����ʶ�����ظ�����
        // cout << id << endl;
        int level = exit_region.size(); // ��ʾ�ڼ��㣬level == 0�൱��scopeջ����Ԫ�ء�ʹ�� n = level-1 ����scope[n][x]����exit_region[n]
        // cout << "1    " << level << endl;
        while (level > 0)
        {
            if (exit_region[level - 1] == false)
            { // �ҵ���һ��δ�˳��ľֲ������Ĳ���
                break;
            }
            else
            {
                level--;
            }
        }
        // cout << "2    " << level << endl;
        if (level <= 0)
        { // ������߼�©�����ܻᵼ���������
            cout << "Enter_ERROR (level<=0)\n";
            exit(0);
        }
        SymbTable temp;
        temp.idname = id;
        temp.attrIR = Attrib;
        if (kind == PROCKIND)
        { // ����ǹ��̱�ʶ��������嵽���µķ��ű����ǰ�棬���˹��̱�ʶ��Ӧ��Ϊ�˷��ű�ĵ�һ��
            scope[level - 1].insert(scope[level - 1].begin(), temp);
        }
        else
        {
            // cout << temp.idname << endl;
            scope[level - 1].push_back(temp);
        }
        present = true;
    }
    else
    {
        present = false;
    }
    return present; // true:�ɹ��� false:���ɹ�
}
void typeDecMoreParsing(treenode *root)
{ // ���ڵ�����Ϊ"TypeDecMore"����ӦRD�е�"typeDecMore����"
    if (root == NULL)
    {
        return;
    }
    typeDecListParsing(root->child[0]);
}
void typeDecListParsing(treenode *root)
{ // ���ڵ�����Ϊ"TypeDecList"����ӦRD�е�"typeDecList����"
    if (root == NULL)
    {
        return;
    }
    SymbTable temp; // ��Ҫ������±�ʶ��
    // root->child[0]: ID, ��ʶ�����Ƹ���
    temp.idname = root->child[0]->token->Sem;
    // root->child[1]: EQ,�޲���
    // root->child[2]:typeDef
    AttributeIR *tempAttr = typeDefParsing(root->child[2]);
    if (tempAttr != NULL)
    { // ���ΪNULL�Ļ��������typeDefParsing������typeDefParsing�еĺ����б����������ﲻ��Ҫ����
        temp.attrIR = *tempAttr;
        delete tempAttr;
        // root->child[3]: SEMI, ������ţ��޲���
        temp.attrIR.kind = TYPEKIND;                               // �������
        bool flag = Enter(temp.idname, temp.attrIR, TYPEKIND, -1); // �����ʶ��
        if (flag == false)
        { // �����ʶ��ʧ�ܣ���ʶ���ظ����壩
            Error_IdentifierDuplicateDec(root->child[0]->token->Line, root->child[0]->token->Sem);
        }
    }
    // root->child[4]: typeDecMore
    typeDecMoreParsing(root->child[4]);
    return;
}
void typeDecParsing(treenode *root)
{ // ���ڵ�����Ϊ"TypeDec"����ӦRD�е�"typeDec����"
    if (root == NULL)
    {
        return;
    }
    // root->child[0]: type���޲���
    // root->child[1]: typeDecList
    typeDecListParsing(root->child[1]);
    return;
}
void typeDecPartParsing(treenode *root)
{ // ���ڵ�����Ϊ"TypeDecPart"����ӦRD�е�"typeDecPart����"
    if (root == NULL)
    {
        return;
    }
    typeDecParsing(root->child[0]);
    return;
}

// �жϼ�¼���ͱ�ʶ���ڵ������������Ա�����ĳ�Ա�����Ƿ�Ϸ����Ϸ��ͷ�������Ԫ�ص�����
struct TypeIR *fieldVarMoreParsing(treenode *root, SymbTable *sym, Token *Arraytok, struct TypeIR *ArrayTy)
{ // ���ڵ�����Ϊ"FieldVarMore"����ӦRD�е�"fieldVarMore����"
    if (root == NULL)
    {
        return NULL;
    }
    // root->child[0]: LMIDPAREN���޲���
    // root->child[1]: exp()���жϼ�¼���ͱ�ʶ���ڵ������������Ա�����ĳ�Ա�����Ƿ�Ϸ�
    if (expParsing(root->child[1]) != TypeList[INTTY - '0'])
    {
        Error_RecordFieldArrayInvalid(Arraytok->Line, sym->idname, Arraytok->Sem);
        return NULL;
    }
    else
    {
        return ArrayTy->More.elemTy; // ����Ԫ�ص�����
    }
    // root->child[2]: RMIDPAREN���޲���
}
// ���ؼ�¼���͵����Ա����,tok��Record���ͱ�����Token��Ϣ��Ŀ���Ǵ��ݴ˱���������
struct TypeIR *fieldVarParsing(treenode *root, SymbTable *sym, Token *tok)
{ // ���ڵ�����Ϊ"FieldVar"����ӦRD�е�"fieldVar����"
    if (root == NULL)
    {
        return NULL;
    }
    // root->child[0]: ID����ID�Ǽ�¼���͵����ԱID
    fieldChain *body = sym->attrIR.idtype->More.body;
    bool flag = false; // �ڴ˼�¼��ʶ���ڴ治���ڶ�Ӧ�����Ա
    while (body != NULL)
    {
        if (body->idname == root->child[0]->token->Sem)
        { // �ҵ��˶�Ӧ�������
            flag = true;
            break;
        }
        body = body->next;
    }
    if (flag == false)
    {
        Error_RecordFieldInvalid(root->child[0]->token->Line, tok->Sem, root->child[0]->token->Sem);
        return NULL;
    }
    else
    {
        if (root->child[1] == NULL)
        { // ���Ա��BaseType
            return body->unitType;
        }
        else
        { // ���Ա��AyyarType
            // root->child[1]: fieldVarMore()
            if (body->unitType->Typekind == ARRAYTY)
            {
                return fieldVarMoreParsing(root->child[1], sym, root->child[0]->token, body->unitType);
            }
            else
            { // ��������������ͣ����൱�ڷ����˷Ƿ�����
                Error_RecordFieldNotArrayType(root->child[0]->token->Line, tok->Sem, root->child[0]->token->Sem);
                return NULL;
            }
        }
    }
}
// sym��structure���ͱ����ķ��ű���Ϣ��tok����Token��Ϣ�����ڴ���������
struct TypeIR *variMoreParsing(treenode *root, SymbTable *sym, Token *tok)
{ // ���ڵ�����Ϊ"VariMore"����ӦRD�е�"variMore����"
    if (root == NULL)
    {
        return NULL;
    }
    if (root->child[0]->token->Lex == LMIDPAREN)
    {
        // root->child[0]: ReadmatchToken(LMIDPAREN));
        // root->child[1]: exp());
        // root->child[2]: ReadmatchToken(RMIDPAREN));
        if (sym->attrIR.idtype->Typekind != ARRAYTY)
        { // ���sym�����������ͱ�ʶ�����򷵻ش���
            Error_ArrayNotArrayType(tok->Line, tok->Sem);
        }
        if (expParsing(root->child[1]) != TypeList[INTTY - '0'])
        {
            Error_ArrayGroupInvalid(tok->Line, tok->Sem);
        }
        else
        {
            return sym->attrIR.idtype->More.elemTy;
        }
    }
    else if (root->child[0]->token->Lex == DOT)
    {
        // root->child[0]: ReadmatchToken(DOT));
        // root->child[1]: fieldVar());
        if (sym->attrIR.idtype->Typekind != RECORDTY)
        { // ���sym���Ǽ�¼���ͱ�ʶ�����򷵻ش���
            Error_RecordNotRecordType(tok->Line, tok->Sem);
            return NULL;
        }
        return fieldVarParsing(root->child[1], sym, tok);
    }
    else
    {
        return NULL;
    }
    return nullptr;
}
// ����������߼�¼�����ͣ�������ź��ӽڵ㲻Ϊ�գ��򷵻�������¼�ĳ�Ա����
struct TypeIR *variableParsing(treenode *root)
{ // ���ڵ�����Ϊ"Variable"����ӦRD�е�"variable����"
    if (root == NULL)
    {
        return NULL;
    }
    // root->child[0]: ID��������ID
    SymbTable *sym = FindEntry(root->child[0]->token->Sem, true, VARKIND, -1);
    if (sym == NULL)
    { // δ�ҵ��˱�ʶ����������ʶ��δ����
        Error_IdentifierUndeclaredVar(root->child[0]->token->Line, root->child[0]->token->Sem);
        return NULL;
    }
    else
    {
        // root->child[1]: variMore());
        if (root->child[1] == NULL)
        { // ID������'[' ���� '.'
            return sym->attrIR.idtype;
        }
        else
        {
            return variMoreParsing(root->child[1], sym, root->child[0]->token);
        }
    }
}
struct TypeIR *factorParsing(treenode *root)
{ // ���ڵ�����Ϊ"Factor"����ӦRD�е�"factor����"
    if (root == NULL)
    {
        return NULL;
    }

    if (root->child[0]->token == NULL)
    { // variable()
        // root->child[0]: variable()
        return variableParsing(root->child[0]);
    }
    else if (root->child[0]->token->Lex == INTC)
    {
        // root->child[0]: INTC
        return TypeList[INTTY - '0']; // �����������͵��ڲ���ʾ
    }
    else if (root->child[0]->token->Lex == LPAREN)
    {
        // root->child[0]: LPAREN
        // root->child[1]: exp()
        return expParsing(root->child[1]);
        // root->child[2]: RPAREN
    }
    return nullptr;
}
void multOpParsing()
{ // ���ڵ�����Ϊ"MultOp"����ӦRD�е�"multOp����"
    // �˽ڵ����Ƿ��ţ��޲���
    return;
}

// tok�������ڵõ�otherFactor�ڵķ��ŵ�Token��Ϣ
struct TypeIR *otherFactorParsing(treenode *root, Token *&tok)
{ // ���ڵ�����Ϊ"OtherFactor"����ӦRD�е�"otherFactor����"
    if (root == NULL)
    {
        return NULL;
    }
    // root->child[0]: multOp���޲���
    multOpParsing();
    tok = root->child[0]->child[0]->token;
    // root->child[1]: term()
    return termParsing(root->child[1]);
}
// ���ش˱��ʽ��term���ֵļ�����������
struct TypeIR *termParsing(treenode *root)
{ // ���ڵ�����Ϊ"Term"����ӦRD�е�"term����"
    if (root == NULL)
    {
        return NULL;
    }
    // root->child[0]: factor()
    struct TypeIR *TypeP1 = factorParsing(root->child[0]);
    // root->child[1]: otherFactor()
    if (root->child[1] == NULL)
    { // ������otherFactorParsing()���֣���ֱ�ӷ���factorParsing()����
        return TypeP1;
    }
    else
    {
        Token *tok = new Token;
        struct TypeIR *TypeP2 = otherFactorParsing(root->child[1], tok);            // tok�������ڵõ�otherFactor�ڵķ��ŵ�Token��Ϣ
        struct TypeIR *CalculatValidity = WhetherResaultValid(TypeP1, TypeP2, tok); // ����TypeP1��TypeP2�Ƿ�ƥ��
        if (CalculatValidity == NULL)
        { // ���ΪNULL����ƥ��
            Error_StmOpComponentIncompatibility(tok->Line, tok->Sem);
        }
        return CalculatValidity; // ����TypeP1��TypeP2�Ƿ�ƥ��
    }
}
void addOpParsing()
{ // ���ڵ�����Ϊ"AddOp"����ӦRD�е�"addOp����"
    // �˽ڵ����Ƿ��ţ��޲���
    return;
}
// ���ش˱��ʽ��otherTerm���ֵļ����������ͣ�tok�������ڵõ�otherTermParsing�ڵķ��ŵ�Token��Ϣ
TypeIR *otherTermParsing(treenode *root, Token *&tok)
{ // ���ڵ�����Ϊ"OtherTerm"����ӦRD�е�"otherTerm����"
    if (root == NULL)
    {
        return NULL;
    }
    // root->child[0]: addOp(), ֻ�Ǿ����˷��Ŷ��ѣ�����Ӱ�����͵ķ���ֵ��������Ҫ���丳ֵ��tok
    addOpParsing();
    tok = root->child[0]->child[0]->token;
    // root->child[1]: exp());
    return expParsing(root->child[1]);
}
// ���ش˱��ʽ�ļ�����������
TypeIR *expParsing(treenode *root)
{ // ���ڵ�����Ϊ"Exp"����ӦRD�е�"exp����"
    if (root == NULL)
    {
        return NULL;
    }
    // root->child[0]: term()
    struct TypeIR *TypeP1 = termParsing(root->child[0]);
    // root->child[1]: otherTerm()
    if (root->child[1] == NULL)
    { // ������otherTermParsing()���֣���ֱ�ӷ���termParsing()����
        return TypeP1;
    }
    else
    {
        Token *tok = new Token;
        struct TypeIR *TypeP2 = otherTermParsing(root->child[1], tok);              // tok�������ڵõ�otherTermParsing�ڵķ��ŵ�Token��Ϣ
        struct TypeIR *CalculatValidity = WhetherResaultValid(TypeP1, TypeP2, tok); // ����TypeP1��TypeP2�Ƿ�ƥ��
        if (CalculatValidity == NULL)
        { // ���ΪNULL����ƥ��
            Error_StmOpComponentIncompatibility(tok->Line, tok->Sem);
        }
        return CalculatValidity; // ����TypeP1��TypeP2�Ƿ�ƥ��
    }
}
void actparamMoreParsing(treenode *root, const int ProcPosition, vector<struct TypeIR *> &TypeVec)
{ // ���ڵ�����Ϊ"ActParamMore"����ӦRD�е�"actparamMore����"
    if (root == NULL)
    {
        return;
    }
    // root->child[0]: COMMA���޲���
    // root->child[1]: actparamList()
    actparamListParsing(root->child[1], ProcPosition, TypeVec); // �õ����ù���ʱ�Ĳ�����Ϣ
}
void actparamListParsing(treenode *root, const int ProcPosition, vector<struct TypeIR *> &TypeVec)
{ // ���ڵ�����Ϊ"ActParamList"����ӦRD�е�"actparamList����"
    if (root == NULL)
    {
        return;
    }
    // root->child[0]: exp());
    // root->child[1]: actparamMore()
    TypeVec.push_back(expParsing(root->child[0])); // �����������ͽ��������Ϣ��

    actparamMoreParsing(root->child[1], ProcPosition, TypeVec); // �õ����ù���ʱ�Ĳ�����Ϣ
}
// ���� ProcPosition �ǹ��̱�ʶ�����ڵķ��ű���scopeջ��λ��
void callStmRestParsing(treenode *root, const int ProcPosition, Token *IDTok)
{ // ���ڵ�����Ϊ"CallStmRest"����ӦRD�е�"callStmRest����"
    if (root == NULL)
    {
        return;
    }
    // root->child[0]: LPAREN, �޲���
    // root->child[1]: actparamList()
    vector<struct TypeIR *> TypeVec;
    actparamListParsing(root->child[1], ProcPosition, TypeVec); // �õ����ù���ʱ�Ĳ�����Ϣ

    // ʹ��scope[ProcPosition][0]��ʾ�˹��̱�ʶ��
    struct ParamTable *param = scope[ProcPosition][0].attrIR.More.ProcAttr.param; // �õ���������ʱ�Ĳ�����
    struct ParamTable *paramP = param;

    int paramSize = 0;
    while (paramP != NULL)
    {
        paramSize++;
        paramP = paramP->next;
    }
    if (paramSize != TypeVec.size())
    {
        // ��ʵ������������ͬ
        Error_ProcParamAmount(IDTok->Line, IDTok->Sem);
    }
    else
    {
        paramP = param;
        for (int ix = 0; ix < TypeVec.size() && paramP != NULL; ix++)
        {
            // ʹ��scope[ProcPosition][ix+1]���ʴ˹��������ĵ�ix+1������
            // ʹ��TypeList[ix]���ʵ��ù��̵ĵ�ix������������
            if (scope[ProcPosition][ix + 1].attrIR.idtype == NULL || TypeVec[ix] == NULL)
            {
                // ��ʵ���д���NULL����ʾ��������δ���壬����ʵ��������һ����ƥ��
                Error_ProcParamType(IDTok->Line, ix + 1, IDTok->Sem);
            }
            else if (scope[ProcPosition][ix + 1].attrIR.idtype != TypeVec[ix])
            {
                // ��ʵ��������һ����ƥ��
                Error_ProcParamType(IDTok->Line, ix + 1, IDTok->Sem);
            }
        }
    }
    // root->child[2]: RPAREN, �޲���
}
// tok���������Token��Ϣ
struct TypeIR *WhetherResaultValid(struct TypeIR *TypeP1, struct TypeIR *TypeP2, Token *tok)
{ // �ڼ����ֵ���ʽ�У�����TypeP1��TypeP2�Ƿ�ƥ�䣬�������������ƥ������ͻ���NULL(��ƥ��)
    /*ʲô����·���NULL(��ʾ������Ϸ�)
     * 1. TypeP1==NULL || TypeP2==NULL
     * 2. ��TypeP1->Typekind != TypeP2->Typekind ʱ��һ�����Ϸ�
     * 3. ��TypeP1 == TypeP2 ʱ��TypeP1->Typekind == ARRAYTY || TypeP1->Typekind == RECORDTY���������Ͳ��ܺ���������ֱ�Ӽ��㣬��¼����Ҳ��һ����
     * */
    bool flag = true;
    if (TypeP1 == NULL || TypeP2 == NULL)
    {
        flag = false;
    }
    else
    {
        if (TypeP1->Typekind != TypeP2->Typekind)
        {
            flag = false;
        }
        else if (TypeP1->Typekind == ARRAYTY || TypeP1->Typekind == RECORDTY)
        { // ��TypeP1 == TypeP2
            flag = false;
        }
        else
        {
            flag = true;
        }
    }
    if (flag == true)
    {
        return TypeP1;
    }
    else
    {
        return NULL;
    }
}
// tok���������Token��Ϣ
void WhetherCompareValid(struct TypeIR *TypeP1, struct TypeIR *TypeP2, Token *tok)
{ // �ڱȽϱ��ʽ�У����TypeP1��TypeP2�Ƿ�ƥ�䣬�������������ƥ������ͻ���NULL(��ƥ��)
    /*ʲô����·���NULL(��ʾ������Ϸ�)
     * 1. TypeP1==NULL || TypeP2==NULL
     * 2. ��TypeP1->Typekind != TypeP2->Typekind ʱ��һ�����Ϸ�
     * 3. ��TypeP1 == TypeP2 ʱ��TypeP1->Typekind == ARRAYTY || TypeP1->Typekind == RECORDTY���������Ͳ��ܺ���������ֱ�Ӽ��㣬��¼����Ҳ��һ����
     * */
    bool flag = true;
    if (TypeP1 == NULL || TypeP2 == NULL)
    {
        flag = false;
    }
    else
    {
        if (TypeP1->Typekind != TypeP2->Typekind)
        {
            flag = false;
        }
        else if (TypeP1->Typekind == ARRAYTY || TypeP1->Typekind == RECORDTY)
        { // ��TypeP1 == TypeP2
            flag = false;
        }
        else
        {
            flag = true;
        }
    }
    if (flag == false)
    {
        Error_StmCompareComponentIncompatibility(tok->Line, tok->Sem);
    }
}
// VarSym��ָ����ֵ�ı����ķ��ű���Ϣ��IDtok��ָ����ֵ�ı�����Token��Ϣ��Ŀ���Ǵ�������
void assignmentRestParsing(treenode *root, SymbTable *VarSym, Token *IDTok)
{ // ���ڵ�����Ϊ"AssignmentRest"����ӦRD�е�"assignmentRest����"
    if (root == NULL)
    {
        return;
    }
    struct TypeIR *TypeP1 = NULL;
    struct TypeIR *TypeP2 = NULL;
    if (root->child[0]->str == "VariMore")
    { // variMore()
        // root->child[0]: variMore()
        TypeP1 = variMoreParsing(root->child[0], VarSym, IDTok);
        // root->child[1]: ASSIGN���޲���
        // root->child[2]: exp()
        TypeP2 = expParsing(root->child[2]);
    }
    else
    {
        // root->child[0]: ASSIGN���޲���
        TypeP1 = VarSym->attrIR.idtype;
        // root->child[1]: exp()
        TypeP2 = expParsing(root->child[1]);
    }
    if (WhetherResaultValid(TypeP1, TypeP2, root->child[0]->token) == NULL)
    { // ˵����ֵ����������Ͳ�����
        Error_AssignContentIncompatible(IDTok->Line, IDTok->Sem);
    }
}

// FindProc()����body��Ѱ����ȷ�Ĺ��̱�ʶ�������ط��������Ĺ��̱�ʶ����scopeջ��λ�ã�ʹ��scope[level][0]��ʾ�˹��̱�ʶ��
int FindProc(const string &id, bool flag, const int ValidTableCount)
{
    // ProcLevelֻ�ܵݼ���Ѱ�ң���������˵�n��ģ��Ͳ����ٲ��Ҵ���n���Proc��ʶ��
    /*�����������¹��̱�ʶ��
     * p1()        --- level 1
     *   p2()      --- level 2
     *      p3()   --- level 3
     * p4()        --- level 1
     *   p5()      --- level 2
     * ��p5��ʼ����Ѱ�ң�ֻ�ܰ���p5 -> p4 -> p1��˳��Ѱ�ң���Ϊp5���ܵ���p3��p2
     */
    int level = exit_region.size() - 1; // ��ʾ�ڼ��㣬level == 0�൱��scopeջ����Ԫ�ء�ʹ�� n = level-1 ����scope[n][x]����exit_region[n]��
    int level_last = ValidTableCount;
    for (; level >= 1; level--)
    { // ��һ�����ű�������û�ж���Proc��ʶ��
        // levelС�ڵ���1ʱ��scope[level][0]��ʾ�˱�־��Ϊ�˷��ű�Ĺ��̱�ʶ��
        if (scope[level].size() >= 1)
        {
            if (scope[level][0].attrIR.More.ProcAttr.level <= level_last)
            { // ���ܴ�����һ�ε�level
                if (scope[level][0].idname == id)
                {
                    return level;
                }
                level_last = scope[level][0].attrIR.More.ProcAttr.level;
            }
        }
    }
    return 0;
}
// ��ֵ���ߵ��ù��̣�����ID����������ID���ǹ��̱�ʶ���Ļ�����Ҫ���ض�Ӧ������(Int, Char, Array, Record)
// ����IDtoke��ID��Token��Ϣ
void assCallParsing(treenode *root, Token *IDtok)
{ // ���ڵ�����Ϊ"AssCall"����ӦRD�е�"assCall����"
    if (root == NULL)
    {
        return;
    }
    if (root->child[0]->str == "CallStmRest")
    {                                                                   // ���̵���callStmRest()
        int ProcPosition = FindProc(IDtok->Sem, true, ValidTableCount); // �Ҵ����ƵĹ��̱�ʶ�����Ҳ����򷵻�0������ʹ��scope[level][0]��ʾ�˹��̱�ʶ��
        if (ProcPosition == 0)
        {
            if (FindEntry(IDtok->Sem, true, '*', -1) == NULL)
            {
                Error_IdentifierUndeclared(IDtok->Line, IDtok->Sem); // δ������ʶ��
                return;
            }
            else
            {
                Error_ProcNotProcIdentifier(IDtok->Line, IDtok->Sem); // ���ǹ��̱�ʶ��
                return;
            }
        }
        else
        {
            callStmRestParsing(root->child[0], ProcPosition, IDtok);
        }
    }
    else if (root->child[0]->str == "AssignmentRest")
    {                                                             // ��ֵassignmentRest()
        SymbTable *VarSym = FindEntry(IDtok->Sem, true, '*', -1); // �Ҵ����Ƶı�ʶ�����Ҳ����򷵻ؿ�
        if (VarSym == NULL)
        {
            int ProcPosition = FindProc(IDtok->Sem, true, ValidTableCount); // �Ҵ����ƵĹ��̱�ʶ�����Ҳ����򷵻�0
            if (ProcPosition == 0)
            {
                Error_IdentifierUndeclared(IDtok->Line, IDtok->Sem); // δ������ʶ��
                return;
            }
            else
            {
                Error_AssignNotVarIdentifier(IDtok->Line, IDtok->Sem); // ���Ǳ�����ʶ��
                return;
            }
        }
        else
        {
            VarSym = FindEntry(IDtok->Sem, true, VARKIND, -1); // �Ҵ����Ƶı�����ʶ�����Ҳ����򷵻ؿ�
            if (VarSym == NULL)
            {
                Error_AssignNotVarIdentifier(IDtok->Line, IDtok->Sem); // ���Ǳ�����ʶ��
                return;
            }
            else
            {
                assignmentRestParsing(root->child[0], VarSym, IDtok);
            }
        }
    }
}

void returnStmParsing()
{ // ���ڵ�����Ϊ"ReturnStm"����ӦRD�е�"returnStm����"
  // returnStm�ж���Token���޺�����ֻ���﷨���󣬲������������󣬲���Ҫ������
}

void outputStmParsing(treenode *root)
{ // ���ڵ�����Ϊ"OutputStm"����ӦRD�е�"outputStm����"
    if (root == NULL)
    {
        return;
    }
    // root->child[0]: WRITE���޲���
    // root->child[1]: LPAREN���޲���
    // root->child[2]: exp()
    expParsing(root->child[2]);
    // root->child[3]: RPAREN���޲���
}

void inputStmParsing(treenode *root)
{ // ���ڵ�����Ϊ"InputStm"����ӦRD�е�"inputStm����"
    // inputStm�ж���Token���޺�����ֻ���﷨���󣬲������������󣬲���Ҫ������
    // root->child[0]: READ
    // root->child[1]: LPAREN
    // root->child[2]: ID
    if (FindEntry(root->child[2]->token->Sem, true, VARKIND, -1) == NULL)
    {
        Error_IdentifierUndeclaredVar(root->child[2]->token->Line, root->child[2]->token->Sem); // ������ʶ��δ����
    }
    // root->child[3]: RPAREN
}

void loopStmParsing(treenode *root)
{ // ���ڵ�����Ϊ""����ӦRD�е�"loopStm����"********************************************
    if (root == NULL)
    {
        return;
    }
    // root->child[0]: WHILE���޲���
    // root->child[1]: exp()����һ�����ʽ
    struct TypeIR *TypeP1 = expParsing(root->child[1]);
    // root->child[2]: LT����EQ���ȽϷ���('<', '=')���޲���
    // root->child[3]: exp()���ڶ������ʽ
    struct TypeIR *TypeP2 = expParsing(root->child[3]);
    WhetherCompareValid(TypeP1, TypeP2, root->child[2]->token);
    // root->child[4]: DO���޲���
    // root->child[5]: stmList()
    stmListParsing(root->child[5]);
    // root->child[6]: ENDWH���޲���
}

void conditionalStmParsing(treenode *root)
{ // ���ڵ�����Ϊ""����ӦRD�е�"conditionalStm����"********************************************
    if (root == NULL)
    {
        return;
    }
    // root->child[0]: IF���޲���
    // root->child[1]: exp()����һ�����ʽ
    struct TypeIR *TypeP1 = expParsing(root->child[1]);
    // root->child[2]: LT����EQ���ȽϷ���('<', '=')���޲���
    // root->child[3]: exp()���ڶ������ʽ
    struct TypeIR *TypeP2 = expParsing(root->child[3]);
    WhetherCompareValid(TypeP1, TypeP2, root->child[2]->token);
    // root->child[4]: THEN���޲���
    // root->child[5]: stmList()
    stmListParsing(root->child[5]);
    // root->child[6]: ELSE���޲���
    // root->child[7]: stmList()
    stmListParsing(root->child[7]);
    // root->child[8]: FI���޲���
}
void stmParsing(treenode *root)
{ // ���ڵ�����Ϊ"Stm"����ӦRD�е�"stm����"
    if (root == NULL)
    {
        return;
    }
    if (root->child[0]->str == "InputStm")
    {
        inputStmParsing(root->child[0]);
    }
    else if (root->child[0]->str == "OutputStm")
    {
        outputStmParsing(root->child[0]);
    }
    else if (root->child[0]->str == "ReturnStm")
    {
        returnStmParsing();
    }
    else if (root->child[0]->str == "ConditionalStm")
    {
        conditionalStmParsing(root->child[0]); // IF
    }
    else if (root->child[0]->str == "LoopStm")
    {
        loopStmParsing(root->child[0]); // WHILE
    }
    else
    { // ��ֵ���ߵ��ù���
        assCallParsing(root->child[1], root->child[0]->token);
    }
}

void stmMoreParsing(treenode *root)
{ // ���ڵ�����Ϊ"StmMore"����ӦRD�е�"stmMore����"
    if (root == NULL)
    {
        return;
    }
    // root->child[0]: SEMI�� �޲���
    // root->child[1]: stmList());
    stmListParsing(root->child[1]);
}
// body�ڣ����ʽList(List and ListMore)
void stmListParsing(treenode *root)
{ // ���ڵ�����Ϊ"StmList"����ӦRD�е�"stmList����"
    if (root == NULL)
    {
        return;
    }
    // root->child[0]:newnode, stm());
    stmParsing(root->child[0]);
    // root->child[1]:newnode, stmMore());
    stmMoreParsing(root->child[1]);
}

void DestroyTable()
{ // �ϳ����µ�һ����Ч���ű�
    ValidTableCount--;
    if (ValidTableCount < 0)
    { // ������߼�©�����ܻᵼ���������
        cout << "DestroyTable_ERROR (ValidTableCount<0)\n";
        exit(0);
    }
    for (int ix = exit_region.size() - 1; ix >= 0; ix--)
    {
        if (exit_region[ix] == false)
        { // �ϳ����ű�����������ĵ�һ��δ�˳��ľֲ�����
            exit_region[ix] = true;
            break;
        }
    }
}

void programBodyParsing(treenode *root)
{ // ���ڵ�����Ϊ"ProgramBody"����ӦRD�е�"programBody����"
    if (root == NULL)
    {
        return;
    }
    // root->child[0]: Begin,�޲���
    // root->child[1]: stmList
    stmListParsing(root->child[1]);
    DestroyTable(); // root->child[2]: End���ϳ���һ�����ű�
    return;
}

void declarePartParsing(treenode *root)
{ // ���ڵ�����Ϊ"DeclarePart"����ӦRD�е�"declarePart����"
    if (root == NULL)
    {
        return;
    }
    typeDecPartParsing(root->child[0]);
    varDecPartParsing(root->child[1]);
    procDecpartParsing(root->child[2]);
    return;
}

void programHeadParsing(treenode *root)
{ // ���ڵ�����Ϊ"ProgramHead"����ӦRD�е�"programHead����"
    if (root == NULL)
    {
        return;
    }
    // ����ͷ�����������ű�Ļ���û���ô������ﲻ���ǳ���ͷ
    return;
}
void RDTreeParsing(treenode *root)
{ // �����﷨��,���ڵ�����Ϊ"Program"
    if (root == NULL)
    {
        return;
    }
    programHeadParsing(root->child[0]);
    declarePartParsing(root->child[1]);
    programBodyParsing(root->child[2]);
}

string TypeIRToString(char TypeKind)
{ // ����������ת�ַ���
    if (TypeKind == '0')
    {
        return "Int   ";
    }
    else if (TypeKind == '1')
    {
        return "Char  ";
    }
    else if (TypeKind == '2')
    {
        return "Bool  ";
    }
    else if (TypeKind == '3')
    {
        return "Record";
    }
    else if (TypeKind == '4')
    {
        return "Array ";
    }
    return "";
}
string KindToString(char TypeKind)
{ // Kind������ת�ַ���
    if (TypeKind == '0')
    {
        return "TypeKind";
    }
    else if (TypeKind == '1')
    {
        return "VarKind ";
    }
    else if (TypeKind == '2')
    {
        return "ProcKind";
    }
    return "";
}
string AccessToString(char access)
{ // Kind������ת�ַ���
    if (access == '0')
    {
        return "dir";
    }
    else if (access == '1')
    {
        return "indir ";
    }
    return "";
}

void PrintTable()
{
    // cout << "printTable" << endl;
    ofstream outTable("D:\\vscode_program\\C++\\SnlCompiler\\Output\\signalTable\\signalTable.txt");
    if (!outTable.is_open())
    {
        cout << "signalTable.txt is not open" << endl;
    }
    else
    {
        outTable << "type table:" << endl;
        outTable << left << setw(15) << "serial number" << left << setw(15) << "size" << left << setw(15) << "kind" << left << setw(15) << "indexTy" << left << setw(15) << "elemTy" << left << setw(15) << "low" << left << setw(15) << "body{type serial,varName,off}" << endl;
        const int TypeList_size = TypeList.size();
        for (int ix = 0; ix < TypeList_size; ix++)
        { // ��ӡ���ͱ�
            outTable << left << setw(15) << to_string(TypeList[ix]->serial + 1) << left << setw(15) << to_string(TypeList[ix]->size) << left << setw(15) << TypeIRToString(TypeList[ix]->Typekind);
            if (TypeList[ix]->Typekind == ARRAYTY)
            {
                outTable << left << setw(15) << to_string(TypeList[ix]->More.indexTy->serial + 1) << left << setw(15) << to_string(TypeList[ix]->More.elemTy->serial + 1) << left << setw(15) << to_string(TypeList[ix]->More.Low);
                outTable << left << setw(15) << EMPTYSPACE; // Record_body
            }
            else if (TypeList[ix]->Typekind == RECORDTY)
            {
                outTable << left << setw(15) << EMPTYSPACE << left << setw(15) << EMPTYSPACE << left << setw(15) << EMPTYSPACE; // Array_indexTy\t Array_elemTy\t Array_Low
                fieldChain *head = TypeList[ix]->More.body;
                while (head != NULL)
                {
                    string message = "";
                    message += "{";
                    message += to_string(head->unitType->serial + 1);
                    message += "; ";
                    message += head->idname;
                    message += "; ";
                    message += to_string(head->offset);
                    message += "}  ";
                    outTable << message;
                    head = head->next;
                }
            }
            else
            {                                                                                                                                                     // basetype: int\tchar\tbool
                outTable << left << setw(15) << EMPTYSPACE << left << setw(15) << EMPTYSPACE << left << setw(15) << EMPTYSPACE << left << setw(15) << EMPTYSPACE; // Array_indexTy\t Array_elemTy\t Array_Low\t Record_body
            }
            outTable << "\n";
        }
        // ��ӡscopeջ�����ű�
        outTable << "\n signal table"
                 << "\n";
        outTable << left << setw(15) << "IDname" << left << setw(15) << "TypePtr" << left << setw(15) << "Kind" << left << setw(15) << "Level" << left << setw(15) << "Parm" << left << setw(15) << "Code" << left << setw(15) << "Size" << left << setw(15) << "Access" << left << setw(15) << "Off"
                 << "\n";
        const int scope_size = scope.size();
        for (int ix = 0; ix < scope_size; ix++)
        {
            const int scope_size_2 = scope[ix].size();
            for (int jx = 0; jx < scope_size_2; jx++)
            {
                outTable << left << setw(15) << scope[ix][jx].idname; // IDname
                if (scope[ix][jx].attrIR.kind == TYPEKIND)
                {
                    outTable << left << setw(15) << to_string(scope[ix][jx].attrIR.idtype->serial + 1); // TypePtr
                    outTable << left << setw(15) << "TypeKind";                                         // Kind
                    outTable << left << setw(15) << EMPTYSPACE << left << setw(15) << EMPTYSPACE << left << setw(15) << EMPTYSPACE << left << setw(15) << EMPTYSPACE << left << setw(15) << EMPTYSPACE << left << setw(15) << EMPTYSPACE;
                }
                else if (scope[ix][jx].attrIR.kind == VARKIND)
                {
                    if (scope[ix][jx].attrIR.idtype != NULL)
                    { // TypePtr
                        outTable << left << setw(15) << to_string(scope[ix][jx].attrIR.idtype->serial + 1);
                    }
                    else
                    { // KindΪVarKind��TypePtrΪ�յ�����������̵Ĳ��������ʹ�����������������ű�Ŀ���Ǵ������˱�����ֻ�е�������ʶ���ض���ʱ�Ų����˲���������ű�
                        outTable << left << setw(15) << "Unknown";
                    }
                    outTable << left << setw(15) << "VarKind"; // Kind
                    outTable << left << setw(15) << to_string(scope[ix][jx].attrIR.More.VarAttr.level);
                    outTable << left << setw(15) << EMPTYSPACE << left << setw(15) << EMPTYSPACE << left << setw(15) << EMPTYSPACE; // Level
                    outTable << left << setw(15) << AccessToString(scope[ix][jx].attrIR.More.VarAttr.access);                       // Access
                    outTable << left << setw(15) << to_string(scope[ix][jx].attrIR.More.VarAttr.off);                               // Off
                }
                else if (scope[ix][jx].attrIR.kind == PROCKIND)
                {
                    outTable << left << setw(15) << EMPTYSPACE;                                          // TypePtr
                    outTable << left << setw(15) << "ProcKind";                                          // Kind
                    outTable << left << setw(15) << to_string(scope[ix][jx].attrIR.More.ProcAttr.level); // Level
                    struct ParamTable *tempptr = scope[ix][jx].attrIR.More.ProcAttr.param;
                    string param = "{ ";
                    if (tempptr != NULL)
                    {
                        param += to_string(tempptr->entry);
                        tempptr = tempptr->next;
                        while (tempptr != NULL)
                        {
                            param += "; ";
                            param += to_string(tempptr->entry);
                            tempptr = tempptr->next;
                        }
                    }
                    param += " }";
                    outTable << left << setw(15) << param; // Parm
                    if (scope[ix][jx].attrIR.More.ProcAttr.code == 0)
                    {                                               // Code
                        outTable << left << setw(15) << EMPTYSPACE; // ���code���ڳ�ʼ����0�����յ�ַ���򷵻ؿ�
                    }
                    else
                    { // ���򷵻ص�ַ
                        outTable << left << setw(15) << to_string(scope[ix][jx].attrIR.More.ProcAttr.code);
                    }
                    outTable << left << setw(15) << to_string(scope[ix][jx].attrIR.More.ProcAttr.size); // Size
                    outTable << left << setw(15) << EMPTYSPACE << left << setw(15) << EMPTYSPACE;
                }
                outTable << "\n";
            }
        }
        outTable.close();
    }
}

/******���ű����******/
void CreateTable()
{ // ����һ�����ű�������scopeջ
    ValidTableCount++;
    vector<SymbTable> temp_Sym; // �����ձ�
    scope.push_back(temp_Sym);
    exit_region.push_back(false); // ��ʾδ�˳��ֲ�����
    return;
}

void initialize()
{
    TypeIR *int_temp = new TypeIR;
    // int_temp->size = 2; // �������Ϊ1
    int_temp->size = 1;
    int_temp->Typekind = INTTY;
    int_temp->serial = TypeList.size();
    TypeList.push_back(int_temp);

    // ��ʼ���ַ�����
    TypeIR *char_temp = new TypeIR;
    char_temp->size = 1;
    char_temp->Typekind = CHARTY;
    char_temp->serial = TypeList.size();
    TypeList.push_back(char_temp);

    // ��ʼ����������
    TypeIR *bool_temp = new TypeIR;
    bool_temp->size = 1;
    bool_temp->Typekind = BOOLTY;
    bool_temp->serial = TypeList.size();
    TypeList.push_back(bool_temp);

    return;
}

void semanticAnalysis(treenode *root)
{
    // ��ʼ�����ֻ�������
    initialize();
    // �½�һ���շ��ű�
    CreateTable(); // �½�һ���շ��ű�
    // �����﷨��
    RDTreeParsing(root);
    // ��ӡ���ͱ�ͷ��ű��ļ�
    PrintTable();
}