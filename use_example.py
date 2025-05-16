
from lexer import Lexer
from parser import Parser
from intermediate_code import IntermediateCodeGenerator

import json

"""
    Código básico de ejemplo para el uso de un analizador léxico, sintáctico y generador de código intermedio.
    Este código no es funcional y solo sirve como ejemplo de cómo se pueden usar las clases definidas en los módulos.
    Se espera que el código fuente sea un fragmento de un lenguaje de programación ficticio.
"""

sourc_code_00 = """
    function main() {
        int x;
        int y;
        x = 10;
        y = 20;

        if (x < y) {
            return x + y;
        } else {
            return x - y;
        }
    }
"""

sourc_code_01 = """
    function suma() {
    
        int i;
        int sum;
        sum = 0;
        i = 1;

        while (i <= 5) {
            sum = sum + i;
            i = i + 1;
        }

        return sum;
    }
"""

source_code_02 = """
    function main() {
        int a;
        a = 5;
        int b = 10;
        if (a < b) {
            return a + b;
        } else {
            return a - b;
        }
    }
"""

source_code_03 = """
    function calculo_salario_neto() {
        int salario = 1000;
        int descuento = 25;
        return salario - (salario * descuento / 100);
    }
"""

programs = [
    sourc_code_00,
    sourc_code_01,
    source_code_02,
    source_code_03
]

for program in programs:
    
    print("\n<----- Código Fuente ----->\n")
    print(program)

    # Análisis léxico
    lexer = Lexer(program)
    tokens = lexer.tokenizer()

    print("\n<----- Tokens Generados ----->\n")
    for token in tokens:
        print(token)

    # Análisis sintáctico
    parser = Parser(tokens)
    ast = parser.parse()

    print("\n<----- Árbol de Sintaxis Abstracta ----->\n")
    print( json.dumps(ast, indent=4) )

    # Generación de código intermedio
    generator = IntermediateCodeGenerator()
    intermediate_code = generator.generate(ast)

    print("\n<----- Código Intermedio Generado ----->\n")
    # Imprimir código intermedio
    for instruction in intermediate_code:
        print(instruction)