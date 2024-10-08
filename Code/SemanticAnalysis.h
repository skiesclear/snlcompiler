#pragma once
#include "GrammerAnalysis.h"

// ��ʶ�����Ƴ���
#define IDLENGTH 10
// ��WrongID������ű���ʾ��ʶ�������Ƿ���ֻ���ڹ��̲�����ʶ�����ظ�����
#define WRONGID "*Unknown"
/*
Typekind�����͵�����, ��"char"������ǰ�����̶�����������ģ�����Ϊ���ͱ��ʼ��ʱ��ǰ���������˳����ˣ�
����ʹ��TypeList[INTTY - '0']��ʾINTTY���͵��ڲ���ʾ
        TypeList[CHARTY - '0']��ʾCHARTY���͵��ڲ���ʾ
        TypeList[BOOLTY - '0']��ʾBOOLTY���͵��ڲ���ʾ
���ԣ�����һ�£�������Ը��ĵĻ�������س������������������ʼ������һ�����
*/
#define INTTY '0'
#define CHARTY '1'
#define BOOLTY '2'
#define RECORDTY '3'
#define ARRAYTY '4'
// #define NAMETY '5'

// AttributeIR �е� kind, ��"char"����
#define TYPEKIND '0'
#define VARKIND '1'
#define PROCKIND '2'

// AccessKind, ʹ��"char"����
#define DIR '0'
#define INDIR '1'

// �յ�Ԫ�������Ϣ
#define EMPTYSPACE "/"

struct fieldChain // ��¼���
{
    string idname;           // ������
    struct TypeIR *unitType; // ָ���ʶ���������ڲ���ʾ�����л���5��(intTy, charTy, arrayTy, recordTy, boolTy)���Զ���
    int offset;              // �ڼ�¼�е�ƫ��
    struct fieldChain *next;
};
struct ParamTable // �β���Ϣ��
{
    int entry; // ָ���βα�ʶ���ڷ��ű��е�λ��
    struct ParamTable *next;
};
struct TypeIR
{
    unsigned int serial; // �����Ա��е�λ�����
    int size;            // ������ռ�ռ��С
    char Typekind;       // ���г�Ա�����ͣ�ʹ�ú궨�壬Typekind(intTy: '0', charTy: '1', boolTy: '2', recordTy: '3', arrayTy: '4')
    union
    {
        struct
        {
            struct TypeIR *indexTy; // ����������Ч��ָ�������±�����
            struct TypeIR *elemTy;  // ����������Ч��ָ������Ԫ������
            unsigned int Low;
        };
        fieldChain *body; // ��¼�����е�����
    } More;
};
struct AttributeIR
{
    struct TypeIR *idtype; // ָ���ʶ���������ڲ���ʾ�����л���5��(intTy, charTy, arrayTy, recordTy, boolTy)���Զ���
    char kind;             // ��ʶ�������࣬����3��(typeKind, varKind, procKind),��kind��ʹ�ú궨�壬typeKind:'0', varKind:'1', procKind:'2'
    union
    {
        struct
        {
            char access; // AccessKind, ʹ�ú궨�壬(dir '0', indir '1')
            int level;
            int off;
        } VarAttr; // ������ʶ��������

        struct
        {
            int level;
            struct ParamTable *param; // ������
            int code;
            int size;
        } ProcAttr; // ��������ʶ��������
    } More;         // ��ʶ���Ĳ�ͬ�����в�ͬ������
};
struct SymbTable
{
    string idname;
    AttributeIR attrIR;
};                 // SNL���ű����ݽṹ����
void initialize(); // ��ʼ�����ֻ�������

/******���ű����******/
void CreateTable();
void DestroyTable();
SymbTable *SearchSingleTable(const string &id, int level, const char kind, const int ProcLevel);
SymbTable *FindEntry(const string &id, bool flag, const char kind, const int ProcLevel);
int FindProc(const string &id, bool flag, const int ProcLevel);
bool Enter(const string &id, AttributeIR Attrib, char kind, const int ProcLevel);
string TypeIRToString(char TypeKind);
string KindToString(char TypeKind);

/***************����������****************/
void semanticAnalysis(treenode *root);
void RDTreeParsing(treenode *root);
void programHeadParsing(treenode *root);
void declarePartParsing(treenode *root);

/***************����****************/
void typeDecPartParsing(treenode *root);
void typeDecParsing(treenode *root);
void typeDecListParsing(treenode *root);
AttributeIR *typeDefParsing(treenode *root);
AttributeIR *baseTypeParsing(treenode *root);
AttributeIR *structureTypeParsing(treenode *root);
AttributeIR *arrayTypeParsing(treenode *root);
AttributeIR *recTypeParsing(treenode *root);
fieldChain *fieldDecListParsing(treenode *root);
void IDListParsing(treenode *root);
void IDMoreParsing(treenode *root);
fieldChain *fieldDecMoreParsing(treenode *root);
void typeDecMoreParsing(treenode *root);

/***************����****************/
void varDecPartParsing(treenode *root);
void varDecParsing(treenode *root);
void varDecListParsing(treenode *root);
void varIDListParsing(treenode *root);
void varIDMoreParsing(treenode *root);
void varDecMoreParsing(treenode *root);

/***************����****************/
void procDecpartParsing(treenode *root);
void procDecParsing(treenode *root);
struct ParamTable *paramListParsing(treenode *root);
struct ParamTable *paramDecListParsing(treenode *root);
struct ParamTable *paramParsing(treenode *root);
struct ParamTable *paramMoreParsing(treenode *root);
void formListParsing(treenode *root);
void fidMoreParsing(treenode *root);

void procDecPartParsing(treenode *root);

void procBodyParsing(treenode *root);
void programBodyParsing(treenode *root);

void stmListParsing(treenode *root);
void stmParsing(treenode *root);
void stmMoreParsing(treenode *root);

void conditionalStmParsing(treenode *root);
void loopStmParsing(treenode *root);
void inputStmParsing(treenode *root);
void outputStmParsing(treenode *root);
void returnStmParsing();
void assCallParsing(treenode *root, Token *IDtok);
void assignmentRestParsing(treenode *root, SymbTable *VarSym, Token *IDTok);
void callStmRestParsing(treenode *root, const int ProcPosition, Token *IDTok);
struct TypeIR *variableParsing(treenode *root);
struct TypeIR *variMoreParsing(treenode *root, SymbTable *sym, Token *tok);
struct TypeIR *expParsing(treenode *root);
void actparamListParsing(treenode *root, const int ProcPosition, vector<TypeIR *> &TypeVec);
void actparamMoreParsing(treenode *root, const int ProcPosition, vector<TypeIR *> &TypeVec);
struct TypeIR *termParsing(treenode *root);
struct TypeIR *otherTermParsing(treenode *root, Token *&tok);
void addOpParsing();
struct TypeIR *factorParsing(treenode *root);
struct TypeIR *otherFactorParsing(treenode *root, Token *&tok);
void multOpParsing();
struct TypeIR *fieldVarParsing(treenode *root, SymbTable *sym, Token *tok);
struct TypeIR *fieldVarMoreParsing(treenode *root, SymbTable *sym, Token *Arraytok, TypeIR *ArrayTy);

// �����ͱ������Type�����Ƿ��Ѿ����ڴ��ڣ����ڵĻ��������ַ�����򷵻ؿ�; Typekind�Ǵ����ʶ�������ͣ��������ͻ��߼�¼���ͣ�
struct TypeIR *WhetherTypeDuplicate(TypeIR *Type, char Typekind);
struct TypeIR *WhetherResaultValid(TypeIR *TypeP1, struct TypeIR *TypeP2, Token *tok);
void WhetherCompareValid(TypeIR *TypeP1, struct TypeIR *TypeP2, Token *tok);
