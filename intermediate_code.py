
class IntermediateCodeGenerator:
    
    def __init__(self):
        self.temp_count = 0
        self.label_count = 0
        self.code = []
    
    def new_temp(self):

        # Genera un nuevo nombre temporal
        # para almacenar resultados intermedios

        temp = f"t{self.temp_count}"
        self.temp_count += 1
        return temp
    
    def new_label(self):
        
        # Genera una nueva etiqueta para
        # saltos en el código intermedio
        # (por ejemplo, para bucles o condicionales)

        label = f"L{self.label_count}"
        self.label_count += 1
        return label
    
    def generate(self, ast):
        
        # Genera el código intermedio a partir del AST
        
        for function in ast['functions']:
            self.generate_function(function)
        return self.code
    
    def generate_function(self, function):

        # Genera el código intermedio para una función
        # (incluyendo su nombre y parámetros)

        self.code.append(f"PROC {function['name']}:")
        for statement in function['body']:
            self.generate_statement(statement)
        self.code.append("ENDP")
    
    def generate_statement(self, statement):
        
        # Genera el código intermedio para una declaración
        # (incluyendo asignaciones, condicionales, bucles, etc.)
        
        stmt_type = statement['type']
        
        # Generar código según el tipo de declaración
        if stmt_type == 'assignment':

            # Generar código para una asignación

            target = statement['target']
            expr_code, temp = self.generate_expression(statement['expr'])
            self.code.extend(expr_code)
            self.code.append(f"{target} = {temp}")
        
        elif stmt_type == 'if': # Se genera código para una declaración if

            #Se crean las etiquetas necesarias para el if
            condition_code, condition_temp = self.generate_expression(statement['condition'])
            label_else = self.new_label()
            label_end = self.new_label()
            
            #Se genera el código para la condición
            self.code.extend(condition_code)
            self.code.append(f"IF_FALSE {condition_temp} GOTO {label_else}")
            
            #Se genera el código para el cuerpo del if
            for stmt in statement['if_body']:
                self.generate_statement(stmt)
            self.code.append(f"GOTO {label_end}")
            
            #Se genera el código para la parte else si es que existe
            self.code.append(f"{label_else}:")
            for stmt in statement.get('else_body', []):
                self.generate_statement(stmt)
            
            #Se agrega la etiqueta de fin del if
            self.code.append(f"{label_end}:")
        
        elif stmt_type == 'while':  # Se genera código para un bucle while
            
            # Se crean las etiquetas necesarias para el bucle
            label_start = self.new_label()
            label_end = self.new_label()
            
            # Se genera la etiqueta de inicio del bucle
            self.code.append(f"{label_start}:")
            
            # Se genera el código para la condición
            condition_code, condition_temp = self.generate_expression(statement['condition'])
            self.code.extend(condition_code)
            self.code.append(f"IF_FALSE {condition_temp} GOTO {label_end}")
            
            #Se genera el código para el cuerpo del bucle
            for stmt in statement['body']:
                self.generate_statement(stmt)
            
            # Se vuelve al inicio del bucle
            self.code.append(f"GOTO {label_start}")
            self.code.append(f"{label_end}:")
        
        elif stmt_type == 'do_while': # Se genera código para un bucle do-while

            # Se crean las etiquetas necesarias para el bucle
            label_start = self.new_label()
            self.code.append(f"{label_start}:")
            
            #Se genera el código para el cuerpo del bucle
            for stmt in statement['body']:
                self.generate_statement(stmt)
            
            #Se genera el código para la condición
            condition_code, condition_temp = self.generate_expression(statement['condition'])
            self.code.extend(condition_code)
            self.code.append(f"IF {condition_temp} GOTO {label_start}")
        
        elif stmt_type == 'for': # Se genera código para un bucle for

            # Se crean las etiquetas necesarias para el bucle
            label_start = self.new_label()
            label_end = self.new_label()
            
            #Se genera el código para la inicialización
            self.generate_statement(statement['init'])
            
            #Se genera la etiqueta de inicio del bucle
            self.code.append(f"{label_start}:")
            
            #Se genera el código para la condición
            condition_code, condition_temp = self.generate_expression(statement['condition'])
            self.code.extend(condition_code)
            self.code.append(f"IF_FALSE {condition_temp} GOTO {label_end}")
            
            #Se genera el código para el cuerpo del bucle
            for stmt in statement['body']:
                self.generate_statement(stmt)
            
            #Se genera el código para la actualización del bucle o incremento del contador
            self.generate_statement(statement['increment'])
            
            #Se vuelve al inicio del bucle
            self.code.append(f"GOTO {label_start}")
            self.code.append(f"{label_end}:")
        
        elif stmt_type == 'return': # Se genera código para una declaración return
            
            #Se genera el código para la expresión de retorno
            expr_code, temp = self.generate_expression(statement['expr'])
            self.code.extend(expr_code)
            self.code.append(f"RETURN {temp}")
    
    def generate_expression(self, expr):

        # Genera el código intermedio para una expresión

        # Se inializan las variables necesarias
        code = []
        temp = None
        
        if expr['type'] == 'binary': # Se genera código para una expresión binaria
            
            # Se generan los códigos para las expresiones izquierda y derecha
            left_code, left_temp = self.generate_expression(expr['left'])
            right_code, right_temp = self.generate_expression(expr['right'])
            temp = self.new_temp()
            
            # Se generan las instrucciones para la operación binaria
            code.extend(left_code)
            code.extend(right_code)
            code.append(f"{temp} = {left_temp} {expr['op']} {right_temp}")
        
        elif expr['type'] == 'unary': # Se genera código para una expresión unaria

            # Se genera el código para la expresión unaria
            operand_code, operand_temp = self.generate_expression(expr['operand'])
            temp = self.new_temp()
            
            # Se genera la instrucción para la operación unaria
            code.extend(operand_code)
            code.append(f"{temp} = {expr['op']}{operand_temp}")
        
        elif expr['type'] == 'id': # Se genera código para una variable identificador
            
            # Se obtiene el nombre de la variable
            # y se asigna a la variable temporal
            temp = expr['name']
        
        elif expr['type'] == 'number': # Se genera código para un número literal
                
            # Se asigna el valor del número literal
            # a la variable temporal
            temp = expr['value']
        
        return code, temp # Devuelve el código generado y la variable temporal