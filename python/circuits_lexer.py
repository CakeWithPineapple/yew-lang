import re

# Token specifications
token_specification = [
  ('NUMBER',    r'\d+'),          # Integer
  ('ID',        r'[A-Za-z_]\w*'), # Identifiers
  ('ASSIGN',    r':='),           # Assignment operator
  ('END',       r';'),            # Statement terminator
  ('LPAREN',    r'\('),           # Left Parenthesis
  ('RPAREN',    r'\)'),           # Right Parenthesis
  ('LBRACE',    r'\{'),           # Left Brace
  ('RBRACE',    r'\}'),           # Right Brace
  ('COMMA',     r','),            # Comma
  ('PLUS',      r'\+'),           # Addition operator
  ('MINUS',     r'-'),            # Subtraction operator
  ('TIMES',     r'\*'),           # Multiplication operator
  ('DIVIDE',    r'/'),            # Division operator
  ('NEWLINE',   r'\n'),           # Line endings
  ('SKIP',      r'[ \t]+'),       # Skip spaces and tabs
  ('MISMATCH',  r'.'),            # Any other character
]

# Tokenizer function
def tokenize(code):
  tokens = []
  line_number = 1
  token_regex = '|'.join('(?P<%s>%s)' % pair for pair in token_specification)
  for mo in re.finditer(token_regex, code):
    kind = mo.lastgroup
    value = mo.group()
    if kind == 'NUMBER':
      value = int(value)
    elif kind == 'ID' and value in {'func', 'let', 'if', 'else', 'elif', 'while', 'for', 'print'}:
      kind = value.upper()
    elif kind == 'NEWLINE':
      line_number += 1
      continue
    elif kind == 'SKIP':
      continue
    elif kind == 'MISMATCH':
      raise RuntimeError(f'{value!r} unexpected on line {line_number}')
    tokens.append((kind, value, line_number))
  return tokens