#include <iostream>
#include <string>
#include <vector>
#include <regex>

class Token {
public:
    std::string type;
    std::string value;
    int line;
    int column;

    Token(std::string type, std::string value, int line, int column) :
        type(type), value(value), line(line), column(column) {}

    // For debugging purposes
    friend std::ostream& operator<<(std::ostream& os, const Token& token) {
        os << "Token(" << token.type << ", " << token.value << ", line=" << token.line << ", column=" << token.column << ")";
        return os;
    }
};

class Lexer {
public:
    Lexer(std::string code) : code(code), current_position(0), current_line(1), current_column(1) {
        // Definir los tipos de tokens
        keywords = {
            {"function", "FUNCTION"},
            {"if", "IF"},
            {"else", "ELSE"},
            {"while", "WHILE"},
            {"do", "DO"},
            {"for", "FOR"},
            {"int", "INT_TYPE"},
            {"bool", "BOOL_TYPE"},
            {"return", "RETURN"},
            {"true", "BOOL"},
            {"false", "BOOL"}
        };

        // Definir las expresiones regulares para los tokens
        token_specs = {
            {"ID", "[a-zA-Z_][a-zA-Z0-9_]*"},
            {"INT", "\\d+"},
            {"ASSIGN", "="},
            {"EQ", "=="},
            {"NE", "!="},
            {"LE", "<="},
            {"GE", ">="},
            {"LT", "<"},
            {"GT", ">"},
            {"AND", "&&"},
            {"OR", "\\|\\|"},
            {"NOT", "!"},
            {"PLUS", "\\+"},
            {"MINUS", "-"},
            {"MUL", "\\*"},
            {"DIV", "/"},
            {"LPAREN", "\\("},
            {"RPAREN", "\\)"},
            {"LBRACE", "\\{"},
            {"RBRACE", "\\}"},
            {"SEMICOLON", ";"},
            {"COMMENT", "//.*"},
            {"WHITESPACE", "\\s+"},
            {"UNKNOWN", "."}
        };
    }

    std::vector<Token> tokenizer() {
        // Bucle principal: mientras no se haya llegado al final del código
        // se itera sobre el código dado como entrada
        while (current_position < code.length()) {
            bool matched = false;

            /*
                Se busca coincidencia con cada expresión regular
                en el orden definido en token_specs.
                Se utiliza la posición actual para buscar coincidencias
                y se actualiza la posición, línea y columna
                según el tipo de token encontrado.
            */
            for (const auto& spec : token_specs) {
                std::regex regex(spec.second);
                std::string s = code.substr(current_position);
                std::smatch match;

                if (std::regex_search(s, match, regex) && match.position() == 0) {
                    std::string token_type = spec.first;
                    std::string value = match.str(0);

                    if (token_type == "ID" && keywords.count(value)) {
                        token_type = keywords[value];
                        Token token(token_type, value, current_line, current_column);
                        tokens.push_back(token);
                    }
                    else if (token_type == "COMMENT" || token_type == "WHITESPACE") {
                        // Se ignoran los comentarios y espacios en blanco
                    }
                    else {
                        Token token(token_type, value, current_line, current_column);
                        tokens.push_back(token);
                    }

                    // Actualizar posición, línea y columna
                    size_t newline_count = std::count(value.begin(), value.end(), '\n');
                    if (newline_count > 0) {
                        for (char c : value) {
                            if (c == '\n') {
                                current_line++;
                                current_column = 1;
                            }
                            else {
                                current_column++;
                            }
                        }
                        size_t last_newline_pos = value.rfind('\n');
                        if (last_newline_pos != std::string::npos) {
                            current_column = value.length() - last_newline_pos;
                        }
                    }
                    else {
                        current_column += value.length();
                    }

                    current_position += value.length();
                    matched = true;
                    break;
                }
            }

            if (!matched) {
                throw std::runtime_error("Carácter inesperado '" + std::string(1, code[current_position]) + "' en línea " + std::to_string(current_line) + ", columna " + std::to_string(current_column));
            }
        }

        return tokens;
    }

private:
    std::string code;
    int current_position;
    int current_line;
    int current_column;
    std::vector<Token> tokens;
    std::map<std::string, std::string> keywords;
    std::vector<std::pair<std::string, std::string>> token_specs;
};