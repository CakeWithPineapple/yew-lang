from circuits_parser import FuncDecl

class Environment:
  def __init__(self):
    self.vars = {}
    self.funcs = {}

  def get_var(self, name):
    return self.vars.get(name, None)
  
  def set_var(self, name, value):
    self.vars[name] = value

  def get_func(self, name):
    return self.funcs.get(name, None)
  
  def set_func(self, name, func):
    self.funcs[name] = func

class GPIO:
  HIGH = 1
  LOW = 0

  def __init__(self):
    self.pins = {}

  def digital_write(self, pin, value):
    self.pins[pin] = value
    print(f'Pin {pin} set to {value}')

  def digital_read(self, pin):
    return self.pins.get(pin, GPIO.LOW)
  
  def pin_mode(self, pin, mode):
    print(f'Pin {pin} set to mode {mode}')

# Extending the interpreter with GPIO
gpio = GPIO()

def digital_write(pin, value):
  gpio.digital_write(pin, value)

def digital_read(pin):
  return gpio.digital_read(pin)

def pin_mode(pin, mode):
  gpio.pin_mode(pin, mode)

def delay(ms):
  import time
  time.sleep(ms / 1000)


class Interpreter:
  def __init__(self):
    self.env = Environment()
    self.env.set_func('digital_write', digital_write)
    self.env.set_func('digital_read', digital_read)
    self.env.set_func('pin_mode', pin_mode)
    self.env.set_func('delay', delay)

  def visit(self, node):
    method_name = 'visit_' + type(node).__name__
    method = getattr(self, method_name, self.generic_visit)
    return method(node)
  
  def generic_visit(self, node):
    raise Exception(f'No visit_{type(node).__name__} method')
  
  def visit_Program(self, node):
    for stmt in node.statements:
      self.visit(stmt)

  def visit_VarDecl(self, node):
    value = self.visit(node.value)
    self.env.set_var(node.var_name, value)
  
  def visit_FuncDecl(self, node):
    self.env.set_func(node.name, node)

  def visit_Block(self, node):
    for stmt in node.statements:
      self.visit(stmt)

  def visit_Assign(self, node):
    value = self.visit(node.value)
    self.env.set_var(node.var_name, value)

  def visit_BinOp(self, node):
    left = self.visit(node.left)
    right = self.visit(node.right)
    if node.op == 'PLUS':
      return left + right
    elif node.op == 'MINUS':
      return left - right
    elif node.op == 'TIMES':
      return left * right
    elif node.op == 'DIVIDE':
      return left / right
    
  def visit_Num(self, node):
    return node.value
  
  def visit_Var(self, node):
    return self.env.get_var(node.name)
  
  def visit_FuncCall(self, node):
    func = self.env.get_func(node.func_name)
    print(callable(func))
    if isinstance(func, FuncDecl):
        # If the function is a FuncDecl object, call it with arguments
        args = [self.visit(arg) for arg in node.args]
        local_env = Environment()
        for param, arg in zip(func.params, args):
            local_env.set_var(param, arg)
        interpreter = Interpreter()
        interpreter.env = local_env
        return interpreter.visit(func.body)
    elif callable(func):
        # If the function is a Python function, call it directly
        args = [self.visit(arg) for arg in node.args]
        return func(*args)
    else:
        raise Exception(f'Function {node.func_name} is not defined')
  
# Registering built-in functions
interpreter = Interpreter()

env = interpreter.env
