#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "lexer.cpp"
#include "parser.cpp"
#include "intermediate_code.cpp"

/*
    Código básico de ejemplo para el uso de un analizador léxico, sintáctico y generador de código intermedio.
    Este código no es funcional y solo sirve como ejemplo de cómo se pueden usar las clases definidas en los módulos.
    Se espera que el código fuente sea un fragmento de un lenguaje de programación ficticio.
*/

int main() {
    std::vector<std::string> programs = {
        R"(
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
        )",
        R"(
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
        )",
        R"(
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
        )",
        R"(
            function calculo_salario_neto() {
                int salario = 1000;
                int descuento = 25;
                return salario - (salario * descuento / 100);
            }
        )"
    };

    for (const auto& program : programs) {
        std::cout << "\n<----- Código Fuente ----->\n" << program << std::endl;

        // Análisis léxico
        Lexer lexer(program);
        std::vector<Token> tokens = lexer.tokenizer();

        std::cout << "\n<----- Tokens Generados ----->\n";
        for (const auto& token : tokens) {
            std::cout << token << std::endl;
        }

        // Análisis sintáctico
        Parser parser(tokens);
        ProgramNode* ast = parser.parse();

        std::cout << "\n<----- Árbol de Sintaxis Abstracta ----->\n";
        // Imprimir el AST (necesitarías una función para imprimir el AST en C++)
        // printAST(ast); // Implementa esta función para imprimir el AST

        // Generación de código intermedio
        IntermediateCodeGenerator generator;

        // Convert ProgramNode* to vector<Function>
        std::vector<Function> functions;
        for (FunctionNode* funcNode : ast->functions) {
            Function func;
            func.name = funcNode->name;
            for (StatementNode* stmtNode : funcNode->body) {
                // Convert StatementNode* to Statement
                Statement stmt;
                stmt.type = stmtNode->type;
                func.body.push_back(stmt);
            }
            functions.push_back(func);
        }

        std::vector<std::string> intermediate_code = generator.generate(functions);

        std::cout << "\n<----- Código Intermedio Generado ----->\n";
        // Imprimir código intermedio
        for (const auto& instruction : intermediate_code) {
            std::cout << instruction << std::endl;
        }
    }

    return 0;
}