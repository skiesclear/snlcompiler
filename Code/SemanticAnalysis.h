#pragma once
#include "GrammerAnalysis.h"

// 标识符名称长度
#define IDLENGTH 10
// 将WrongID插入符号表，表示标识符命名非法，只用于过程参数标识符的重复声明
#define WRONGID "*Unknown"
/*
Typekind，类型的种类, 用"char"声明，前三个固定（不允许更改），因为类型表初始化时，前三个和这个顺序绑定了，
并且使用TypeList[INTTY - '0']表示INTTY类型的内部表示
        TypeList[CHARTY - '0']表示CHARTY类型的内部表示
        TypeList[BOOLTY - '0']表示BOOLTY类型的内部表示
所以，警告一下，如果擅自更改的话，结果必出错，除非连语义分析初始化函数一起更改
*/
#define INTTY '0'
#define CHARTY '1'
#define BOOLTY '2'
#define RECORDTY '3'
#define ARRAYTY '4'
// #define NAMETY '5'

// AttributeIR 中的 kind, 用"char"声明
#define TYPEKIND '0'
#define VARKIND '1'
#define PROCKIND '2'

// AccessKind, 使用"char"声明
#define DIR '0'
#define INDIR '1'

// 空单元格填充信息
#define EMPTYSPACE "/"

struct fieldChain // 记录域表
{
    string idname;           // 变量名
    struct TypeIR *unitType; // 指向标识符的类型内部表示，共有基本5种(intTy, charTy, arrayTy, recordTy, boolTy)和自定义
    int offset;              // 在记录中的偏移
    struct fieldChain *next;
};
struct ParamTable // 形参信息表
{
    int entry; // 指向形参标识符在符号表中的位置
    struct ParamTable *next;
};
struct TypeIR
{
    unsigned int serial; // 在属性表中的位置序号
    int size;            // 类型所占空间大小
    char Typekind;       // 域中成员的类型，使用宏定义，Typekind(intTy: '0', charTy: '1', boolTy: '2', recordTy: '3', arrayTy: '4')
    union
    {
        struct
        {
            struct TypeIR *indexTy; // 数组类型有效，指向数组下标类型
            struct TypeIR *elemTy;  // 数组类型有效，指向数组元素类型
            unsigned int Low;
        };
        fieldChain *body; // 记录类型中的域链
    } More;
};
struct AttributeIR
{
    struct TypeIR *idtype; // 指向标识符的类型内部表示，共有基本5种(intTy, charTy, arrayTy, recordTy, boolTy)和自定义
    char kind;             // 标识符的种类，共有3种(typeKind, varKind, procKind),的kind，使用宏定义，typeKind:'0', varKind:'1', procKind:'2'
    union
    {
        struct
        {
            char access; // AccessKind, 使用宏定义，(dir '0', indir '1')
            int level;
            int off;
        } VarAttr; // 变量标识符的属性

        struct
        {
            int level;
            struct ParamTable *param; // 参数表
            int code;
            int size;
        } ProcAttr; // 过程名标识符的属性
    } More;         // 标识符的不同类型有不同的属性
};
struct SymbTable
{
    string idname;
    AttributeIR attrIR;
};                 // SNL符号表数据结构声明
void initialize(); // 初始化三种基本类型

/******符号表操作******/
void CreateTable();
void DestroyTable();
SymbTable *SearchSingleTable(const string &id, int level, const char kind, const int ProcLevel);
SymbTable *FindEntry(const string &id, bool flag, const char kind, const int ProcLevel);
int FindProc(const string &id, bool flag, const int ProcLevel);
bool Enter(const string &id, AttributeIR Attrib, char kind, const int ProcLevel);
string TypeIRToString(char TypeKind);
string KindToString(char TypeKind);

/***************函数声明区****************/
void semanticAnalysis(treenode *root);
void RDTreeParsing(treenode *root);
void programHeadParsing(treenode *root);
void declarePartParsing(treenode *root);

/***************类型****************/
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

/***************变量****************/
void varDecPartParsing(treenode *root);
void varDecParsing(treenode *root);
void varDecListParsing(treenode *root);
void varIDListParsing(treenode *root);
void varIDMoreParsing(treenode *root);
void varDecMoreParsing(treenode *root);

/***************过程****************/
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

// 在类型表里，参数Type类型是否已经存在存在，存在的话返回其地址，否则返回空; Typekind是待测标识符的类型（数组类型或者记录类型）
struct TypeIR *WhetherTypeDuplicate(TypeIR *Type, char Typekind);
struct TypeIR *WhetherResaultValid(TypeIR *TypeP1, struct TypeIR *TypeP2, Token *tok);
void WhetherCompareValid(TypeIR *TypeP1, struct TypeIR *TypeP2, Token *tok);
