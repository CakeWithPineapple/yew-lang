from circuits_lexer import tokenize
from circuits_parser import Parser
from circuits_interpreter import Interpreter

code = """
  delay(1000);
"""
#func blink(pin) {
#delay(1000);
#      digital_write(pin, LOW);
#      delay(1000);

tokens = tokenize(code)
# for token in tokens:
#   print(token)

parser = Parser(tokens)
ast = parser.parse()
interpreter = Interpreter()

# Running the interpreter
interpreter.visit(ast)