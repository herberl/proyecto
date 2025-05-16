from lexer import Token

class Parser:

    def __init__(self, tokens):
        self.tokens = tokens
        self.current_token = None
        self.token_index = -1
        self.advance()
    
    def advance(self):

        # Se avanza al siguiente token

        self.token_index += 1
        if self.token_index < len(self.tokens):
            self.current_token = self.tokens[self.token_index]
        else:
            self.current_token = None
    
    def eat(self, token_type):
        
        # Se consume el token actual si coincide con el tipo esperado

        if self.current_token and self.current_token.type == token_type:
            self.advance()
        else:
            raise SyntaxError(f"Se esperaba {token_type}, se encontró {self.current_token.type if self.current_token else 'EOF'}")
    
    def parse(self):

        # Método principal para iniciar el análisis sintáctico
        return self.program()
    
    def program(self):

        # Se analiza el programa completo

        functions = []
        while self.current_token and self.current_token.type == 'FUNCTION':
            functions.append(self.function())
        return {'type': 'program', 'functions': functions}
    
    def function(self):

        # Se analiza la definición de una función

        self.eat('FUNCTION')
        name = self.current_token.value
        
        self.eat('ID')  
        self.eat('LPAREN')
        self.eat('RPAREN')
        self.eat('LBRACE')
        
        # Se analiza el cuerpo de la función
        statements = []
        while self.current_token and self.current_token.type != 'RBRACE':
            statements.append(self.statement())
        
        # Se consume la llave de cierre y se retorna la estructura de la función
        self.eat('RBRACE')
        return {'type': 'function', 'name': name, 'body': statements}
    
    def statement(self):

        # Se analiza una declaración o instrucción
        
        if self.current_token.type == 'INT_TYPE' or self.current_token.type == 'BOOL_TYPE':
            return self.declaration()
        elif self.current_token.type == 'ID':
            return self.assignment()
        elif self.current_token.type == 'IF':
            return self.if_statement()
        elif self.current_token.type == 'WHILE':
            return self.while_statement()
        elif self.current_token.type == 'DO':
            return self.do_while_statement()
        elif self.current_token.type == 'FOR':
            return self.for_statement()
        elif self.current_token.type == 'RETURN':
            return self.return_statement()
        else:
            raise SyntaxError(f"Declaración no válida: {self.current_token.type}")

    def declaration(self):
        
        # Se analizan declaraciones de variables (int o bool)
        
        var_type = self.current_token.type
        self.eat(var_type)  # 'INT_TYPE' o 'BOOL_TYPE'
        
        var_name = self.current_token.value
        self.eat('ID')
        
        #se verifica si hay una asignación
        if self.current_token.type == 'ASSIGN':
            self.eat('ASSIGN')
            expr = self.expression()
            self.eat('SEMICOLON')
            return {'type': 'declaration', 'var_type': var_type, 'var_name': var_name, 'init': expr}
        else:
            self.eat('SEMICOLON')
            return {'type': 'declaration', 'var_type': var_type, 'var_name': var_name}

    def assignment(self):
        
        #Se analizan las asignaciones de variables
        
        var_name = self.current_token.value
        self.eat('ID')
        self.eat('ASSIGN')
        
        expr = self.expression()
        self.eat('SEMICOLON')

        return {'type': 'assignment', 'target': var_name, 'expr': expr}

    def if_statement(self):
        
        #Se analizan las declaraciones if-else
        
        self.eat('IF')
        self.eat('LPAREN')
        condition = self.expression()
        self.eat('RPAREN')
        self.eat('LBRACE')
        
        # Se analiza el cuerpo del if
        if_body = []
        while self.current_token and self.current_token.type != 'RBRACE':
            if_body.append(self.statement())
        self.eat('RBRACE')
        
        # Se analiza el cuerpo del else (opcional)
        else_body = []
        if self.current_token and self.current_token.type == 'ELSE':
            self.eat('ELSE')
            self.eat('LBRACE')
            while self.current_token and self.current_token.type != 'RBRACE':
                else_body.append(self.statement())
            self.eat('RBRACE')
        
        # Se retorna la estructura del if-else
        return {
            'type': 'if',
            'condition': condition,
            'if_body': if_body,
            'else_body': else_body
        }

    def while_statement(self):
        
        #Se analizan los bucles while
        
        self.eat('WHILE')
        self.eat('LPAREN')
        condition = self.expression()
        self.eat('RPAREN')
        self.eat('LBRACE')
        
        # Se analiza el cuerpo del while
        body = []
        while self.current_token and self.current_token.type != 'RBRACE':
            body.append(self.statement())
        self.eat('RBRACE')
        
        # Se retorna la estructura del while
        return {'type': 'while', 'condition': condition, 'body': body}

    def do_while_statement(self):
        
        #Se analizan los bucles do-while
        
        #Se abre la estructura do
        self.eat('DO')
        self.eat('LBRACE')
        
        # Se analiza el cuerpo del do
        body = []
        while self.current_token and self.current_token.type != 'RBRACE':
            body.append(self.statement())
        self.eat('RBRACE')
        
        # Se analiza la condición del while
        self.eat('WHILE')
        self.eat('LPAREN')
        condition = self.expression()
        self.eat('RPAREN')
        self.eat('SEMICOLON')
        
        # Se retorna la estructura del do-while
        return {'type': 'do_while', 'condition': condition, 'body': body}

    def for_statement(self):
        
        #Se analizan los bucles for
        
        # Se abre la estructura for
        self.eat('FOR')
        self.eat('LPAREN')
        
        # Inicialización (opcional)
        init = None
        if self.current_token.type in ['INT_TYPE', 'BOOL_TYPE']:
            init = self.declaration()
        elif self.current_token.type == 'ID':
            init = self.assignment()
        else:
            self.eat('SEMICOLON')
        
        # Condición (opcional)
        condition = None
        if self.current_token.type != 'SEMICOLON':
            condition = self.expression()
        self.eat('SEMICOLON')
        
        # Paso (opcional)
        step = None
        if self.current_token.type != 'RPAREN':
            step = self.assignment()
        self.eat('RPAREN')
        
        # Se analiza el cuerpo del for
        self.eat('LBRACE')
        body = []
        while self.current_token and self.current_token.type != 'RBRACE':
            body.append(self.statement())
        self.eat('RBRACE')
        
        # Se retorna la estructura del for
        return {
            'type': 'for',
            'init': init,
            'condition': condition,
            'step': step,
            'body': body
        }

    def return_statement(self):
        
        #Se analizan las declaraciones de retorno
        
        self.eat('RETURN')
        expr = self.expression()
        self.eat('SEMICOLON')
        
        # Se retorna la estructura del return
        return {'type': 'return', 'expr': expr}

    def expression(self):
        #Se analiza una expresión completa
        #Punto de entrada para la expresión
        return self.logical_or()

    def logical_or(self):
        
        # Se analizan operadores lógicos OR (||)
        
        #Se pasa a la función lógica AND si no se encuentra un operador lógico OR
        node = self.logical_and()
        
        # Se analiza si hay operadores lógicos OR
        while self.current_token and self.current_token.type == 'OR':
            self.eat('OR')
            node = {
                'type': 'binary',
                'op': '||',
                'left': node,
                'right': self.logical_and()
            }

        # Se retorna el nodo de la expresión
        return node

    def logical_and(self):

        # Se analizan operadores lógicos AND (&&)
        
        # Se pasa a la función de igualdad si no se encuentra un operador lógico AND
        node = self.equality()

        # Se analiza si hay operadores lógicos AND
        while self.current_token and self.current_token.type == 'AND':
            self.eat('AND')
            node = {
                'type': 'binary',
                'op': '&&',
                'left': node,
                'right': self.equality()
            }
        
        # Se retorna el nodo de la expresión
        return node

    def equality(self):
        
        # Se analizan operadores de igualdad (==, !=)
        
        # Se pasa a la función relacional si no se encuentra un operador de igualdad
        node = self.relational()
        
        # Se analiza si hay operadores de igualdad
        while self.current_token and self.current_token.type in ['EQ', 'NE']:
            op = self.current_token.type
            self.eat(op)
            node = {
                'type': 'binary',
                'op': '==' if op == 'EQ' else '!=',
                'left': node,
                'right': self.relational()
            }

        # Se retorna el nodo de la expresión
        return node

    def relational(self):

        # Se analizan operadores relacionales (<, >, <=, >=)
        
        # Se pasa a la función aditiva si no se encuentra un operador relacional
        node = self.additive()

        # Se analiza si hay operadores relacionales
        while self.current_token and self.current_token.type in ['LT', 'GT', 'LE', 'GE']:
            op = self.current_token.type
            self.eat(op)
            node = {
                'type': 'binary',
                'op': op.lower(),
                'left': node,
                'right': self.additive()
            }
        
        # Se retorna el nodo de la expresión
        return node

    def additive(self):
        
        # Se analizan operadores aditivos (+, -)
        
        # Se pasa a la función multiplicativa si no se encuentra un operador aditivo
        node = self.multiplicative()
        
        # Se analiza si hay operadores aditivos
        while self.current_token and self.current_token.type in ['PLUS', 'MINUS']:
            op = self.current_token.type
            self.eat(op)
            node = {
                'type': 'binary',
                'op': op,
                'left': node,
                'right': self.multiplicative()
            }
        
        # Se retorna el nodo de la expresión
        return node

    def multiplicative(self):
        
        # Se analizan operadores multiplicativos (*, /)
        
        # Se pasa a la función unaria si no se encuentra un operador multiplicativo
        node = self.unary()

        # Se analiza si hay operadores multiplicativos
        while self.current_token and self.current_token.type in ['MUL', 'DIV']:
            op = self.current_token.type
            self.eat(op)
            node = {
                'type': 'binary',
                'op': op,
                'left': node,
                'right': self.unary()
            }

        # Se retorna el nodo de la expresión
        return node

    def unary(self):

        # Se analizan operadores unarios (NOT, -)
        
        #Se analiza si hay un operador unario
        if self.current_token and self.current_token.type in ['NOT', 'MINUS']:
            op = self.current_token.type
            self.eat(op)
            return {
                'type': 'unary',
                'op': '!' if op == 'NOT' else '-',
                'operand': self.primary()
            }
        
        # se retorna el nodo de la expresión primaria
        return self.primary()

    def primary(self):

        # Se analizan expresiones primarias (ID, INT, BOOL, LPAREN)
        
        # Se obtiene el token actual
        token = self.current_token
        
        # Se analiza el tipo de token
        if token.type == 'ID':
            self.eat('ID')
            return {'type': 'id', 'name': token.value} # Se retorna el token ID
        
        elif token.type == 'INT':
            self.eat('INT')
            return {'type': 'number', 'value': int(token.value)} # Se retorna el token INT
        
        elif token.type == 'BOOL':
            self.eat('BOOL')
            return {'type': 'boolean', 'value': token.value == 'true'} # Se retorna el token BOOL
        
        elif token.type == 'LPAREN':
            
            # Se analiza una expresión entre paréntesis

            self.eat('LPAREN')
            expr = self.expression()
            self.eat('RPAREN')
            return expr
        
        else:
            raise SyntaxError(f"Token inesperado: {token.type}") # Se lanza un error si el token no es válido