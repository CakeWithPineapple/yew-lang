#ifndef AST_H
#define AST_H

typedef struct ASTNode {
    char *type;
    void *data;
} ASTNode;

typedef struct {
    char *name;
    ASTNode *value;
} VarDecl;

typedef struct {
    char *name;
    char **params;
    int param_count;
    ASTNode *body;
} FuncDecl;

typedef struct {
    ASTNode **statements;
    int statement_count;
} Block;

typedef struct {
    char *var_name;
    ASTNode *value;
} Assign;

typedef struct {
    ASTNode *left;
    char *op;
    ASTNode *right;
} BinOp;

typedef struct {
    int value;
} Num;

typedef struct {
    char *name;
} Var;

typedef struct {
    char *func_name;
    ASTNode **args;
    int arg_count;
} FuncCall;

#endif