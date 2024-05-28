#ifndef LEXER_H
#define LEXER_H

typedef struct {
  char *type;
  char *value;
  int line;
} Token;

#endif