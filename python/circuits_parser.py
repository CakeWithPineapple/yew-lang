class ASTNode:
  pass

class Program(ASTNode):
  def __init__(self, statements):
    self.statements = statements

class VarDecl(ASTNode):
  def __init__(self, var_type, var_name, value):
    self.var_type = var_type
    self.var_name = var_name
    self.value = value

class FuncDecl(ASTNode):
  def __init__(self, name, params, body):
    self.name = name
    self.params = params
    self.body = body

class Block(ASTNode):
  def __init__(self, statements):
    self.statements = statements

class Assign(ASTNode):
    def __init__(self, var_name, value):
        self.var_name = var_name
        self.value = value

class BinOp(ASTNode):
    def __init__(self, left, op, right):
        self.left = left
        self.op = op
        self.right = right

class Num(ASTNode):
    def __init__(self, value):
        self.value = value

class Var(ASTNode):
    def __init__(self, name):
        self.name = name

class FuncCall(ASTNode):
    def __init__(self, func_name, args):
        self.func_name = func_name
        self.args = args

class Parser:
  def __init__(self, tokens):
    self.tokens = tokens
    self.pos = 0

  def parse(self):
    statements = []
    while self.pos < len(self.tokens):
      statements.append(self.statement())
    return Program(statements)
  
  def statement(self):
    token = self.tokens[self.pos]
    if token[0] == 'LET':
      return self.var_decl()
    elif token[0] == 'FUNC':
      return self.func_decl()
    elif token[0] == 'ID':
      if self.tokens[self.pos + 1][0] == 'LPAREN':
        return self.func_call()
      else:
        self.error(token)
    else:
      self.error(token)

  def var_decl(self):
    self.pos += 1  # skip 'let'
    var_name = self.tokens[self.pos][1]
    self.pos += 1  # skip variable name
    self.pos += 1  # skip ':='
    value = self.expr()
    self.pos += 1  # skip ';'
    return VarDecl(None, var_name, value)
  
  def func_decl(self):
    self.pos += 1  # skip 'func'
    func_name = self.tokens[self.pos][1]
    self.pos += 1  # skip function name
    self.pos += 1  # skip '('
    params = []
    while self.tokens[self.pos][0] != 'RPAREN':
      params.append(self.tokens[self.pos][1])
      self.pos += 1
      if self.tokens[self.pos][0] == 'COMMA':
        self.pos += 1
    self.pos += 1  # skip ')'
    self.pos += 1  # skip '{'
    body = self.block()
    self.pos += 1  # skip '}'
    return FuncDecl(func_name, params, body)
  
  def func_call(self):
    func_name = self.tokens[self.pos][1]
    self.pos += 2  # Skip function name and '('
    args = []
    while self.tokens[self.pos][0] != 'RPAREN':
      args.append(self.expr())
      if self.tokens[self.pos][0] == 'COMMA':
        self.pos += 1
    self.pos += 1  # Skip ')'
    self.pos += 1  # Skip ';'
    return FuncCall(func_name, args)
  
  def block(self):
    statements = []
    while self.tokens[self.pos][0] != 'RBRACE':
      statements.append(self.statement())
    return Block(statements)
  
  def expr(self):
    left = self.term()
    while self.pos < len(self.tokens) and self.tokens[self.pos][0] in {'PLUS', 'MINUS'}:
      op = self.tokens[self.pos][0]
      self.pos += 1
      right = self.term()
      left = BinOp(left, op, right)
    return left
  
  def term(self):
    token = self.tokens[self.pos]
    self.pos += 1
    if token[0] == 'NUMBER':
      return Num(token[1])
    elif token[0] == 'ID':
      return Var(token[1])
    else:
      self.error(token)

  def error(self, token):
    raise SyntaxError(f"Unexpected token: {token[1]} on line {token[2]}")
