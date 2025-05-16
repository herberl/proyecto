
class SymbolTable:

    #constructor
    def __init__(self):
        self.symbols = {}
        self.scopes = [{}] #lista de diccionarios para manejar los ámbitos
    
    #seccion de ambitos -> begin

    #Funcion para crear un nuevo ámbito vacio y agregarlo a la lista de ámbitos
    def enter_scope(self):
        self.scopes.append({})
    
    #Funcion para salir del ámbito actual, eliminando el último ámbito de la lista
    def exit_scope(self):
        if len(self.scopes) > 1:
            self.scopes.pop()
    
    #seccion de ambitos -> end

    #seccion de simbolos -> begin

    #Funcion para agregar un nuevo símbolo al ámbito actual
    def add_symbol(self, name, symbol_type, value=None, is_function=False, is_constant=False):
        
        # Verifica si el símbolo ya existe en el ámbito actual
        if name in self.scopes[-1]:
            raise ValueError(f"Símbolo '{name}' ya declarado en este ámbito")
        
        # Agrega el símbolo al ámbito actual
        self.scopes[-1][name] = {
            'type': symbol_type,
            'value': value,
            'is_constant': is_constant,
            'is_function': is_function
        }
    
    # Método para registrar una función en la tabla de símbolos
    #en el ambito actual
    def register_function(self, name, return_type, parameters):
        
        # Verifica si la función ya está declarada en el ámbito actual
        if name in self.scopes[-1]:
            raise ValueError(f"Función '{name}' ya declarada en este ámbito")
        
        # Agrega la función al ámbito actual
        self.scopes[-1][name] = {
            'is_function': True,
            'return_type': return_type,
            'parameters': parameters,
            'type': 'function'
        }

    #Funcion para buscar un símbolo en los ámbitos
    #Busca el símbolo en los ámbitos desde el más interno al más externo
    def lookup(self, name):
        
        # Busca el símbolo en los ámbitos desde el más interno al más externo
        for scope in reversed(self.scopes):
            if name in scope:
                return scope[name]
            
        # Si no se encuentra el símbolo, lanza una excepción
        raise ValueError(f"Símbolo '{name}' no declarado")
    
    #Funcion para actualizar el valor de un símbolo existente
    def update_symbol(self, name, value):

        # Busca el símbolo en los ámbitos desde el más interno al más externo
        for scope in reversed(self.scopes):
            if name in scope:
                scope[name]['value'] = value
                return
        
        # Si no se encuentra el símbolo, lanza una excepción
        raise ValueError(f"Símbolo '{name}' no declarado")

    #seccion de simbolos -> end