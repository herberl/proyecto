import re

class Token:
    def __init__(self, type, value, line, column):
        self.type = type
        self.value = value
        self.line = line
        self.column = column
    
    def __repr__(self):
        return f"Token({self.type}, {self.value}, line={self.line}, column={self.column})"

class Lexer:
    def __init__(self, code):
        
        # Inicializar el lexer con el código fuente
        self.code = code
        self.current_position = 0
        self.current_line = 1
        self.current_column = 1
        self.tokens = []

        # Definir los tipos de tokens
        self.keywords = {
            'function': 'FUNCTION',
            'if': 'IF',
            'else': 'ELSE',
            'while': 'WHILE',
            'do': 'DO',
            'for': 'FOR',
            'int': 'INT_TYPE',
            'bool': 'BOOL_TYPE',
            'return': 'RETURN',
            'true': 'BOOL',
            'false': 'BOOL'
        }

        # Definir las expresiones regulares para los tokens
        self.token_specs = [
            ('ID', r'[a-zA-Z_][a-zA-Z0-9_]*'),
            ('INT', r'\d+'),
            ('ASSIGN', r'='),
            ('EQ', r'=='),
            ('NE', r'!='),
            ('LE', r'<='),
            ('GE', r'>='),
            ('LT', r'<'),
            ('GT', r'>'),
            ('AND', r'&&'),
            ('OR', r'\|\|'),
            ('NOT', r'!'),
            ('PLUS', r'\+'),
            ('MINUS', r'-'),
            ('MUL', r'\*'),
            ('DIV', r'/'),
            ('LPAREN', r'\('),
            ('RPAREN', r'\)'),
            ('LBRACE', r'\{'),
            ('RBRACE', r'\}'),
            ('SEMICOLON', r';'),
            ('COMMENT', r'//.*'),
            ('WHITESPACE', r'\s+'),
            ('UNKNOWN', r'.')
        ]
    
    def tokenizer(self):

        # Bucle principal: mientras no se haya llegado al final del código
        # se itera sobre el código dado como entrada
        while self.current_position < len(self.code):
            
            matched = False
            
            """
                Se busca coincidencia con cada expresión regular
                en el orden definido en token_specs.
                Se utiliza la posición actual para buscar coincidencias
                y se actualiza la posición, línea y columna
                según el tipo de token encontrado.
            """
            for token_type, pattern in self.token_specs:
                
                regex = re.compile( pattern )
                match = regex.match(self.code, self.current_position )

                if match:

                    #print(f"Matched {token_type}: {match.group(0)} at position {self.current_position}")                    

                    value = match.group(0)
                    
                    if token_type == 'ID' and value in self.keywords:
                        token_type = self.keywords[value]
                        token = Token(token_type, value, self.current_line, self.current_column)
                        self.tokens.append(token)
                    elif token_type == 'COMMENT' or token_type == 'WHITESPACE':
                        pass # Se ignoran los comentarios y espacios en blanco
                    else:
                        token = Token(token_type, value, self.current_line, self.current_column)
                        self.tokens.append(token)
                    
                    # Actualizar posición, línea y columna
                    lines = value.split('\n')
                    if len(lines) > 1:
                        self.current_line += len(lines) - 1
                        self.current_column = len(lines[-1]) + 1
                    else:
                        self.current_column += len(value)
                    
                    self.current_position = match.end()
                    matched = True
                    break
            
            if not matched:
                raise SyntaxError(f"Carácter inesperado '{self.code[self.position]}' en línea {self.line}, columna {self.column}")
        
        return self.tokens