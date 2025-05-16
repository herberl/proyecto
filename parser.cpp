#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

#include "lexer.cpp" // Assuming you have a lexer.h with the Token definition

// Forward declarations of AST node structures
struct ProgramNode;
struct FunctionNode;
struct StatementNode;
struct DeclarationNode;
struct AssignmentNode;
struct IfNode;
struct WhileNode;
struct DoWhileNode;
struct ForNode;
struct ReturnNode;
struct ExpressionNode;

// Union to hold different expression values
struct Value {
    int int_val;
    bool bool_val;
    std::string id_name;
    // Add more types as needed
};

// Expression Node
struct ExpressionNode {
    std::string type; // "binary", "unary", "id", "number", "boolean"
    std::string op;    // "+", "-", "*", "/", "==", "!=", "<", ">", "<=", ">=", "&&", "||", "!", "-"
    Value value;
    ExpressionNode* left;   // For binary expressions
    ExpressionNode* right;  // For binary expressions
    ExpressionNode* operand; // For unary expressions

    ExpressionNode() : left(nullptr), right(nullptr), operand(nullptr) {}
};

// Statement Node (Base class for all statements)
struct StatementNode {
    std::string type; // "declaration", "assignment", "if", "while", "do_while", "for", "return"
    virtual ~StatementNode() {}
};

// Declaration Node
struct DeclarationNode : public StatementNode {
    std::string var_type; // "INT_TYPE", "BOOL_TYPE"
    std::string var_name;
    ExpressionNode* init; // Optional initialization expression

    DeclarationNode() : init(nullptr) { type = "declaration"; }
    ~DeclarationNode() { delete init; }
};

// Assignment Node
struct AssignmentNode : public StatementNode {
    std::string target; // Variable name to assign to
    ExpressionNode* expr;

    AssignmentNode() : expr(nullptr) { type = "assignment"; }
    ~AssignmentNode() { delete expr; }
};

// If Node
struct IfNode : public StatementNode {
    ExpressionNode* condition;
    std::vector<StatementNode*> if_body;
    std::vector<StatementNode*> else_body;

    IfNode() : condition(nullptr) { type = "if"; }
    ~IfNode() {
        delete condition;
        for (auto stmt : if_body) delete stmt;
        for (auto stmt : else_body) delete stmt;
    }
};

// While Node
struct WhileNode : public StatementNode {
    ExpressionNode* condition;
    std::vector<StatementNode*> body;

    WhileNode() : condition(nullptr) { type = "while"; }
    ~WhileNode() {
        delete condition;
        for (auto stmt : body) delete stmt;
    }
};

// Do-While Node
struct DoWhileNode : public StatementNode {
    ExpressionNode* condition;
    std::vector<StatementNode*> body;

    DoWhileNode() : condition(nullptr) { type = "do_while"; }
    ~DoWhileNode() {
        delete condition;
        for (auto stmt : body) delete stmt;
    }
};

// For Node
struct ForNode : public StatementNode {
    StatementNode* init;      // Initialization statement (optional)
    ExpressionNode* condition; // Condition expression (optional)
    StatementNode* step;      // Increment/decrement statement (optional)
    std::vector<StatementNode*> body;

    ForNode() : init(nullptr), condition(nullptr), step(nullptr) { type = "for"; }
    ~ForNode() {
        delete init;
        delete condition;
        delete step;
        for (auto stmt : body) delete stmt;
    }
};

// Return Node
struct ReturnNode : public StatementNode {
    ExpressionNode* expr; // Expression to return

    ReturnNode() : expr(nullptr) { type = "return"; }
    ~ReturnNode() { delete expr; }
};

// Function Node
struct FunctionNode {
    std::string type; // "function"
    std::string name;
    std::vector<StatementNode*> body;

    FunctionNode() { type = "function"; }
    ~FunctionNode() {
        for (auto stmt : body) delete stmt;
    }
};

// Program Node (Root node of the AST)
struct ProgramNode {
    std::string type; // "program"
    std::vector<FunctionNode*> functions;

    ProgramNode() { type = "program"; }
    ~ProgramNode() {
        for (auto func : functions) delete func;
    }
};

class Parser {
public:
    Parser(std::vector<Token> tokens) : tokens(tokens), token_index(-1), current_token(nullptr) {
        advance();
    }

    ~Parser() {}

    ProgramNode* parse() {
        return program();
    }

private:
    std::vector<Token> tokens;
    int token_index;
    Token* current_token;

    void advance() {
        token_index++;
        if (token_index < tokens.size()) {
            current_token = &tokens[token_index];
        }
        else {
            current_token = nullptr;
        }
    }

    void eat(std::string token_type) {
        if (current_token && current_token->type == token_type) {
            advance();
        }
        else {
            throw std::runtime_error("Se esperaba " + token_type + ", se encontró " + (current_token ? current_token->type : "EOF"));
        }
    }

    ProgramNode* program() {
        ProgramNode* node = new ProgramNode();
        while (current_token && current_token->type == "FUNCTION") {
            node->functions.push_back(function());
        }
        return node;
    }

    FunctionNode* function() {
        FunctionNode* node = new FunctionNode();
        eat("FUNCTION");
        node->name = current_token->value;
        eat("ID");
        eat("LPAREN");
        eat("RPAREN");
        eat("LBRACE");

        while (current_token && current_token->type != "RBRACE") {
            node->body.push_back(statement());
        }

        eat("RBRACE");
        return node;
    }

    StatementNode* statement() {
        if (current_token->type == "INT_TYPE" || current_token->type == "BOOL_TYPE") {
            return declaration();
        }
        else if (current_token->type == "ID") {
            return assignment();
        }
        else if (current_token->type == "IF") {
            return if_statement();
        }
        else if (current_token->type == "WHILE") {
            return while_statement();
        }
        else if (current_token->type == "DO") {
            return do_while_statement();
        }
        else if (current_token->type == "FOR") {
            return for_statement();
        }
        else if (current_token->type == "RETURN") {
            return return_statement();
        }
        else {
            throw std::runtime_error("Declaración no válida: " + current_token->type);
        }
    }

    DeclarationNode* declaration() {
        DeclarationNode* node = new DeclarationNode();
        node->var_type = current_token->type;
        eat(node->var_type); // "INT_TYPE" or "BOOL_TYPE"

        node->var_name = current_token->value;
        eat("ID");

        if (current_token->type == "ASSIGN") {
            eat("ASSIGN");
            node->init = expression();
            eat("SEMICOLON");
        }
        else {
            eat("SEMICOLON");
        }
        return node;
    }

    AssignmentNode* assignment() {
        AssignmentNode* node = new AssignmentNode();
        node->target = current_token->value;
        eat("ID");
        eat("ASSIGN");

        node->expr = expression();
        eat("SEMICOLON");

        return node;
    }

    IfNode* if_statement() {
        IfNode* node = new IfNode();
        eat("IF");
        eat("LPAREN");
        node->condition = expression();
        eat("RPAREN");
        eat("LBRACE");

        while (current_token && current_token->type != "RBRACE") {
            node->if_body.push_back(statement());
        }
        eat("RBRACE");

        if (current_token && current_token->type == "ELSE") {
            eat("ELSE");
            eat("LBRACE");
            while (current_token && current_token->type != "RBRACE") {
                node->else_body.push_back(statement());
            }
            eat("RBRACE");
        }

        return node;
    }

    WhileNode* while_statement() {
        WhileNode* node = new WhileNode();
        eat("WHILE");
        eat("LPAREN");
        node->condition = expression();
        eat("RPAREN");
        eat("LBRACE");

        while (current_token && current_token->type != "RBRACE") {
            node->body.push_back(statement());
        }
        eat("RBRACE");

        return node;
    }

    DoWhileNode* do_while_statement() {
        DoWhileNode* node = new DoWhileNode();
        eat("DO");
        eat("LBRACE");

        while (current_token && current_token->type != "RBRACE") {
            node->body.push_back(statement());
        }
        eat("RBRACE");

        eat("WHILE");
        eat("LPAREN");
        node->condition = expression();
        eat("RPAREN");
        eat("SEMICOLON");

        return node;
    }

    ForNode* for_statement() {
        ForNode* node = new ForNode();
        eat("FOR");
        eat("LPAREN");

        // Initialization (optional)
        if (current_token->type == "INT_TYPE" || current_token->type == "BOOL_TYPE") {
            node->init = declaration();
        }
        else if (current_token->type == "ID") {
            node->init = assignment();
        }
        else {
            eat("SEMICOLON");
        }

        // Condition (optional)
        if (current_token->type != "SEMICOLON") {
            node->condition = expression();
        }
        eat("SEMICOLON");

        // Step (optional)
        if (current_token->type != "RPAREN") {
            node->step = assignment();
        }
        eat("RPAREN");

        eat("LBRACE");
        while (current_token && current_token->type != "RBRACE") {
            node->body.push_back(statement());
        }
        eat("RBRACE");

        return node;
    }

    ReturnNode* return_statement() {
        ReturnNode* node = new ReturnNode();
        eat("RETURN");
        node->expr = expression();
        eat("SEMICOLON");

        return node;
    }

    ExpressionNode* expression() {
        return logical_or();
    }

    ExpressionNode* logical_or() {
        ExpressionNode* node = logical_and();

        while (current_token && current_token->type == "OR") {
            ExpressionNode* new_node = new ExpressionNode();
            new_node->type = "binary";
            new_node->op = "||";
            new_node->left = node;
            eat("OR");
            new_node->right = logical_and();
            node = new_node;
        }

        return node;
    }

    ExpressionNode* logical_and() {
        ExpressionNode* node = equality();

        while (current_token && current_token->type == "AND") {
            ExpressionNode* new_node = new ExpressionNode();
            new_node->type = "binary";
            new_node->op = "&&";
            new_node->left = node;
            eat("AND");
            new_node->right = equality();
            node = new_node;
        }

        return node;
    }

    ExpressionNode* equality() {
        ExpressionNode* node = relational();

        while (current_token && (current_token->type == "EQ" || current_token->type == "NE")) {
            ExpressionNode* new_node = new ExpressionNode();
            new_node->type = "binary";
            new_node->op = (current_token->type == "EQ") ? "==" : "!=";
            new_node->left = node;
            eat(current_token->type);
            new_node->right = relational();
            node = new_node;
        }

        return node;
    }

    ExpressionNode* relational() {
        ExpressionNode* node = additive();

        while (current_token && (current_token->type == "LT" || current_token->type == "GT" || current_token->type == "LE" || current_token->type == "GE")) {
            ExpressionNode* new_node = new ExpressionNode();
            new_node->type = "binary";
            new_node->op = current_token->type;
            new_node->left = node;
            eat(current_token->type);
            new_node->right = additive();
            node = new_node;
        }

        return node;
    }

    ExpressionNode* additive() {
        ExpressionNode* node = multiplicative();

        while (current_token && (current_token->type == "PLUS" || current_token->type == "MINUS")) {
            ExpressionNode* new_node = new ExpressionNode();
            new_node->type = "binary";
            new_node->op = current_token->type;
            new_node->left = node;
            eat(current_token->type);
            new_node->right = multiplicative();
            node = new_node;
        }

        return node;
    }

    ExpressionNode* multiplicative() {
        ExpressionNode* node = unary();

        while (current_token && (current_token->type == "MUL" || current_token->type == "DIV")) {
            ExpressionNode* new_node = new ExpressionNode();
            new_node->type = "binary";
            new_node->op = current_token->type;
            new_node->left = node;
            eat(current_token->type);
            new_node->right = unary();
            node = new_node;
        }

        return node;
    }

    ExpressionNode* unary() {
        if (current_token && (current_token->type == "NOT" || current_token->type == "MINUS")) {
            ExpressionNode* node = new ExpressionNode();
            node->type = "unary";
            node->op = (current_token->type == "NOT") ? "!" : "-";
            eat(current_token->type);
            node->operand = primary();
            return node;
        }

        return primary();
    }

    ExpressionNode* primary() {
        Token* token = current_token;

        if (token->type == "ID") {
            eat("ID");
            ExpressionNode* node = new ExpressionNode();
            node->type = "id";
            node->value.id_name = token->value;
            return node;
        }
        else if (token->type == "INT") {
            eat("INT");
            ExpressionNode* node = new ExpressionNode();
            node->type = "number";
            node->value.int_val = std::stoi(token->value);
            return node;
        }
        else if (token->type == "BOOL") {
            eat("BOOL");
            ExpressionNode* node = new ExpressionNode();
            node->type = "boolean";
            node->value.bool_val = (token->value == "true");
            return node;
        }
        else if (token->type == "LPAREN") {
            eat("LPAREN");
            ExpressionNode* node = expression();
            eat("RPAREN");
            return node;
        }
        else {
            throw std::runtime_error("Token inesperado: " + token->type);
        }
    }
};