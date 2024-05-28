#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

typedef struct ASTNode {
  char *type;
  void *data;
} ASTNode;

typedef struct {
  char *var_type;
  char *var_name;
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

typedef struct {
  Token *tokens;
  int pos;
  int token_count;
} Parser;

ASTNode* parse(Parser *parser);

ASTNode* statement(Parser *parser);

ASTNode* var_decl(Parser *parser);

ASTNode* func_decl(Parser *parser);

ASTNode* func_call(Parser *parser);

ASTNode* block(Parser *parser);

ASTNode* expr(Parser *parser);

ASTNode* term(Parser *parser);

void error(Token token) {
  fprintf(stderr, "Unexpected token: %s on line %d\n", token.value, token.line);
  exit(EXIT_FAILURE);
}

ASTNode* parse(Parser *parser) {
  ASTNode **statements = malloc(100 * sizeof(ASTNode*));
  int statement_count = 0;

  while (parser->pos < parser->token_count) {
    statements[statement_count++] = statement(parser);
  }

  ASTNode *program = malloc(sizeof(ASTNode));
  program->type = "Program";
  Block *block = malloc(sizeof(Block));
  block->statements = statements;
  block->statement_count = statement_count;
  program->data = block;

  return program;
}

ASTNode* statement(Parser *parser) {
  Token token = parser->tokens[parser->pos];

  if (strcmp(token.type, "LET") == 0) {
    return var_decl(parser);
  } else if (strcmp(token.type, "FUNC") == 0) {
    return func_decl(parser);
  } else if (strcmp(token.type, "ID") == 0) {
    if (strcmp(parser->tokens[parser->pos + 1].type, "LPAREN") == 0) {
      return func_call(parser);
    } else {
      error(token);
    }
  } else {
    error(token);
  }
  return NULL;
}

ASTNode* var_decl(Parser *parser) {
  parser->pos++; // skip 'let'
  char *var_name = parser->tokens[parser->pos++].value; // skip variable name
  parser->pos++; // skip ':='
  ASTNode *value = expr(parser);
  parser->pos++; // skip ';'

  VarDecl *var_decl = malloc(sizeof(VarDecl));
  var_decl->var_type = NULL;
  var_decl->var_name = var_name;
  var_decl->value = value;

  ASTNode *node = malloc(sizeof(ASTNode));
  node->type = "VarDecl";
  node->data = var_decl;

  return node;
}

ASTNode* func_decl(Parser *parser) {
  parser->pos++; // skip 'func'
  char *func_name = parser->tokens[parser->pos++].value; // skip function name
  parser->pos++; // skip '('
  char **params = malloc(10 * sizeof(char*));
  int param_count = 0;

  while (strcmp(parser->tokens[parser->pos].type, "RPAREN") != 0) {
    params[param_count++] = parser->tokens[parser->pos++].value;
    if (strcmp(parser->tokens[parser->pos].type, "COMMA") == 0) {
      parser->pos++;
    }
  }
  parser->pos++; // skip ')'
  parser->pos++; // skip '{'
  ASTNode *body = block(parser);
  parser->pos++; // skip '}'

  FuncDecl *func_decl = malloc(sizeof(FuncDecl));
  func_decl->name = func_name;
  func_decl->params = params;
  func_decl->param_count = param_count;
  func_decl->body = body;

  ASTNode *node = malloc(sizeof(ASTNode));
  node->type = "FuncDecl";
  node->data = func_decl;

  return node;
}

ASTNode* func_call(Parser *parser) {
  char *func_name = parser->tokens[parser->pos++].value; // Skip function name
  parser->pos++; // Skip '('
  ASTNode **args = malloc(10 * sizeof(ASTNode*));
  int arg_count = 0;

  while (strcmp(parser->tokens[parser->pos].type, "RPAREN") != 0) {
    args[arg_count++] = expr(parser);
    if (strcmp(parser->tokens[parser->pos].type, "COMMA") == 0) {
      parser->pos++;
    }
  }
  parser->pos++; // Skip ')'
  parser->pos++; // Skip ';'

  FuncCall *func_call = malloc(sizeof(FuncCall));
  func_call->func_name = func_name;
  func_call->args = args;
  func_call->arg_count = arg_count;

  ASTNode *node = malloc(sizeof(ASTNode));
  node->type = "FuncCall";
  node->data = func_call;

  return node;
}

ASTNode* block(Parser *parser) {
  ASTNode **statements = malloc(100 * sizeof(ASTNode*));
  int statement_count = 0;

  while (strcmp(parser->tokens[parser->pos].type, "RBRACE") != 0) {
    statements[statement_count++] = statement(parser);
  }

  Block *block = malloc(sizeof(Block));
  block->statements = statements;
  block->statement_count = statement_count;

  ASTNode *node = malloc(sizeof(ASTNode));
  node->type = "Block";
  node->data = block;

  return node;
}

ASTNode* expr(Parser *parser) {
  ASTNode *left = term(parser);

  while (parser->pos < parser->token_count && 
           (strcmp(parser->tokens[parser->pos].type, "PLUS") == 0 || 
            strcmp(parser->tokens[parser->pos].type, "MINUS") == 0)) {
    char *op = parser->tokens[parser->pos++].type;
    ASTNode *right = term(parser);

    BinOp *bin_op = malloc(sizeof(BinOp));
    bin_op->left = left;
    bin_op->op = op;
    bin_op->right = right;

    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = "BinOp";
    node->data = bin_op;

    left = node;
  }

  return left;
}

ASTNode* term(Parser *parser) {
  Token token = parser->tokens[parser->pos++];

  if (strcmp(token.type, "NUMBER") == 0) {
    Num *num = malloc(sizeof(Num));
    num->value = atoi(token.value);

    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = "Num";
    node->data = num;

    return node;
  } else if (strcmp(token.type, "ID") == 0) {
    Var *var = malloc(sizeof(Var));
    var->name = token.value;

    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = "Var";
    node->data = var;

    return node;
  } else {
    error(token);
  }
  return NULL;
}