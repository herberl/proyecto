#include <iostream>
#include <vector>
#include <string>

class IntermediateCodeGenerator {
public:
    IntermediateCodeGenerator() : temp_count(0), label_count(0) {}

    std::string new_temp() {
        // Genera un nuevo nombre temporal
        // para almacenar resultados intermedios
        std::string temp = "t" + std::to_string(temp_count++);
        return temp;
    }

    std::string new_label() {
        // Genera una nueva etiqueta para
        // saltos en el código intermedio
        // (por ejemplo, para bucles o condicionales)
        std::string label = "L" + std::to_string(label_count++);
        return label;
    }

    std::vector<std::string> generate(const std::vector<Function>& ast) {
        // Genera el código intermedio a partir del AST
        code.clear();
        for (const auto& function : ast) {
            generate_function(function);
        }
        return code;
    }

private:
    int temp_count;
    int label_count;
    std::vector<std::string> code;

    void generate_function(const Function& function) {
        // Genera el código intermedio para una función
        // (incluyendo su nombre y parámetros)
        code.push_back("PROC " + function.name + ":");
        for (const auto& statement : function.body) {
            generate_statement(statement);
        }
        code.push_back("ENDP");
    }

    void generate_statement(const Statement& statement) {
        // Genera el código intermedio para una declaración
        // (incluyendo asignaciones, condicionales, bucles, etc.)
        std::string stmt_type = statement.type;

        // Generar código según el tipo de declaración
        if (stmt_type == "assignment") {
            // Generar código para una asignación
            std::string target = statement.target;
            auto [expr_code, temp] = generate_expression(statement.expr);
            code.insert(code.end(), expr_code.begin(), expr_code.end());
            code.push_back(target + " = " + temp);
        } else if (stmt_type == "if") { // Se genera código para una declaración if
            //Se crean las etiquetas necesarias para el if
            auto [condition_code, condition_temp] = generate_expression(statement.condition);
            std::string label_else = new_label();
            std::string label_end = new_label();

            //Se genera el código para la condición
            code.insert(code.end(), condition_code.begin(), condition_code.end());
            code.push_back("IF_FALSE " + condition_temp + " GOTO " + label_else);

            //Se genera el código para el cuerpo del if
            for (const auto& stmt : statement.if_body) {
                generate_statement(stmt);
            }
            code.push_back("GOTO " + label_end);

            //Se genera el código para la parte else si es que existe
            code.push_back(label_else + ":");
            for (const auto& stmt : statement.else_body) {
                generate_statement(stmt);
            }

            //Se agrega la etiqueta de fin del if
            code.push_back(label_end + ":");
        } else if (stmt_type == "while") {  // Se genera código para un bucle while
            // Se crean las etiquetas necesarias para el bucle
            std::string label_start = new_label();
            std::string label_end = new_label();

            // Se genera la etiqueta de inicio del bucle
            code.push_back(label_start + ":");

            // Se genera el código para la condición
            auto [condition_code, condition_temp] = generate_expression(statement.condition);
            code.insert(code.end(), condition_code.begin(), condition_code.end());
            code.push_back("IF_FALSE " + condition_temp + " GOTO " + label_end);

            //Se genera el código para el cuerpo del bucle
            for (const auto& stmt : statement.body) {
                generate_statement(stmt);
            }

            // Se vuelve al inicio del bucle
            code.push_back("GOTO " + label_start);
            code.push_back(label_end + ":");
        } else if (stmt_type == "do_while") { // Se genera código para un bucle do-while
            // Se crean las etiquetas necesarias para el bucle
            std::string label_start = new_label();
            code.push_back(label_start + ":");

            //Se genera el código para el cuerpo del bucle
            for (const auto& stmt : statement.body) {
                generate_statement(stmt);
            }

            //Se genera el código para la condición
            auto [condition_code, condition_temp] = generate_expression(statement.condition);
            code.insert(code.end(), condition_code.begin(), condition_code.end());
            code.push_back("IF " + condition_temp + " GOTO " + label_start);
        } else if (stmt_type == "for") { // Se genera código para un bucle for
            // Se crean las etiquetas necesarias para el bucle
            std::string label_start = new_label();
            std::string label_end = new_label();

            //Se genera el código para la inicialización
            generate_statement(statement.init);

            //Se genera la etiqueta de inicio del bucle
            code.push_back(label_start + ":");

            //Se genera el código para la condición
            auto [condition_code, condition_temp] = generate_expression(statement.condition);
            code.insert(code.end(), condition_code.begin(), condition_code.end());
            code.push_back("IF_FALSE " + condition_temp + " GOTO " + label_end);

            //Se genera el código para el cuerpo del bucle
            for (const auto& stmt : statement.body) {
                generate_statement(stmt);
            }

            //Se genera el código para la actualización del bucle o incremento del contador
            generate_statement(statement.increment);

            //Se vuelve al inicio del bucle
            code.push_back("GOTO " + label_start);
            code.push_back(label_end + ":");
        } else if (stmt_type == "return") { // Se genera código para una declaración return
            //Se genera el código para la expresión de retorno
            auto [expr_code, temp] = generate_expression(statement.expr);
            code.insert(code.end(), expr_code.begin(), expr_code.end());
            code.push_back("RETURN " + temp);
        }
    }

    std::pair<std::vector<std::string>, std::string> generate_expression(const Expression& expr) {
        // Genera el código intermedio para una expresión
        std::vector<std::string> code;
        std::string temp;

        if (expr.type == "binary") { // Se genera código para una expresión binaria
            // Se generan los códigos para las expresiones izquierda y derecha
            auto [left_code, left_temp] = generate_expression(expr.left);
            auto [right_code, right_temp] = generate_expression(expr.right);
            temp = new_temp();

            // Se generan las instrucciones para la operación binaria
            code.insert(code.end(), left_code.begin(), left_code.end());
            code.insert(code.end(), right_code.begin(), right_code.end());
            code.push_back(temp + " = " + left_temp + " " + expr.op + " " + right_temp);
        } else if (expr.type == "unary") { // Se genera código para una expresión unaria
            // Se genera el código para la expresión unaria
            auto [operand_code, operand_temp] = generate_expression(expr.operand);
            temp = new_temp();

            // Se genera la instrucción para la operación unaria
            code.insert(code.end(), operand_code.begin(), operand_code.end());
            code.push_back(temp + " = " + expr.op + operand_temp);
        } else if (expr.type == "id") { // Se genera código para una variable identificador
            // Se obtiene el nombre de la variable
            // y se asigna a la variable temporal
            temp = expr.name;
        } else if (expr.type == "number") { // Se genera código para un número literal
            // Se asigna el valor del número literal
            // a la variable temporal
            temp = std::to_string(expr.value);
        }

        return {code, temp}; // Devuelve el código generado y la variable temporal
    }
};

// Define the structures to match the Python AST
struct Expression {
    std::string type;
    std::string op;
    std::string name;
    int value;
    Expression left;
    Expression right;
    Expression operand;
};

struct Statement {
    std::string type;
    std::string target;
    Expression expr;
    Expression condition;
    std::vector<Statement> if_body;
    std::vector<Statement> else_body;
    std::vector<Statement> body;
    Statement init;
    Statement increment;
};

struct Function {
    std::string name;
    std::vector<Statement> body;
};