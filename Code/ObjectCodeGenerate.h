#include "SemanticAnalysis.h"
void genProc(treenode *t);
void codeGen(treenode *syntaxTree);
void genStmt(treenode *tree);
void ExpG(treenode *tree, int regNumber, int choose);
void conditionalStmG(treenode *tree);
void LoopStmG(treenode *tree);
void AssCall(treenode *tree);
void InputStmG(treenode *tree);
void OutputStmG(treenode *tree);
void AssCall(treenode *tree);
int assignmentRestG(treenode *);
void searchInDisplay(treenode *varNode, int regNumber, int choose = 0);
int findFuncSize(treenode *fun);
void putInDisplay(treenode *varNode, int choose);
