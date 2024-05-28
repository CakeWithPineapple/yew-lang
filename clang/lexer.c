#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
  char *type;
  char *value;
  int line;
} Token;

typedef struct {
  Token *tokens;
  int count;
  int capacity;
} TokenList;

TokenList* create_token_list(int capacity) {
  TokenList *token_list = malloc(sizeof(TokenList));
  token_list->tokens = malloc(capacity * sizeof(Token));
  token_list->count = 0;
  token_list->capacity = capacity;
  return token_list;
}

void add_token(TokenList *token_list, const char *type, const char *value, int line) {
  if (token_list->count >= token_list->capacity) {
    token_list->capacity *= 2;
    token_list->tokens = realloc(token_list->tokens, token_list->capacity * sizeof(Token));
  }
  token_list->tokens[token_list->count].type = strdup(type);
  token_list->tokens[token_list->count].value = strdup(value);
  token_list->tokens[token_list->count].line = line;
  token_list->count++;
}

int is_keyword(const char *str) {
  const char *keywords[] = {"func", "let", "if", "else", "elif", "while", "for", "print"};
  for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
    if (strcmp(str, keywords[i]) == 0) {
      return 1;
    }
  }
  return 0;
}

TokenList* tokenize(const char *code) {
  TokenList *tokens = create_token_list(100);
  int line_number = 1;
  int i = 0;
  while (code[i] != '\0') {
    if (isdigit(code[i])) {
      int start = i;
      while (isdigit(code[i])) i++;
      char number[16];
      strncpy(number, &code[start], i - start);
      number[i - start] = '\0';
      add_token(tokens, "NUMBER", number, line_number);
    } else if (isalpha(code[i]) || code[i] == '_') {
      int start = i;
      while (isalnum(code[i]) || code[i] == '_') i++;
      char id[32];
      strncpy(id, &code[start], i - start);
      id[i - start] = '\0';
      if (is_keyword(id)) {
        add_token(tokens, id, id, line_number);
      } else {
        add_token(tokens, "ID", id, line_number);
      }
    } else if (code[i] == ':') {
      if (code[i+1] == '=') {
        add_token(tokens, "ASSIGN", ":=", line_number);
        i++;
      } else if (code[i] == ';') {
        add_token(tokens, "END", ";", line_number);
        i++;
      } else if (code[i] == '(') {
        add_token(tokens, "LPAREN", "(", line_number);
        i++;
      } else if (code[i] == ')') {
        add_token(tokens, "RPAREN", ")", line_number);
      } else if (code[i] == '{') {
        add_token(tokens, "LBRACE", "{", line_number);
        i++;
      } else if (code[i] == '}') {
        add_token(tokens, "RBRACE", "}", line_number);
        i++;
      } else if (code[i] == ',') {
        add_token(tokens, "COMMA", ",", line_number);
        i++;
      } else if (code[i] == '+') {
          add_token(tokens, "PLUS", "+", line_number);
          i++;
      } else if (code[i] == '-') {
          add_token(tokens, "MINUS", "-", line_number);
          i++;
      } else if (code[i] == '*') {
          add_token(tokens, "TIMES", "*", line_number);
          i++;
      } else if (code[i] == '/') {
          add_token(tokens, "DIVIDE", "/", line_number);
          i++;
      } else if (code[i] == '\n') {
          line_number++;
          i++;
      } else if (isspace(code[i])) {
        i++;
      } else {
        fprintf(stderr, "Unexpected character '%c' on line %d\n", code[i], line_number);
        exit(EXIT_FAILURE);
      }
    }
    return tokens;
  }
}