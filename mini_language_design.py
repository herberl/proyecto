
# Diseño fundamental del lenguaje de programación 

"""
<program> ::= <function>

<function> ::= "function" ID "()" "{" <statement> "}"

<statement> ::= <declaration> | <assignment> | <if> | <while> | <do_while> | <for> | <return>

<declaration> ::= "int" ID ";" | "bool" ID ";"

<assignment> ::= ID "=" <expression> ";"

<if> ::= "if" "(" <expression> ")" "{" <statement>* "}" ["else" "{" <statement> "}"]

<while> ::= "while" "(" <expression> ")" "{" <statement> "}"

<do_while> ::= "do" "{" <statement>* "}" "while" "(" <expression> ")" ";"

<for> ::= "for" "(" <assignment>? ";" <expression>? ";" <assignment>? ")" "{" <statement> "}"

<return> ::= "return" <expression> ";"

<expression> ::= <logical_or>

<logical_or> ::= <logical_and> ("||" <logical_and>)

<logical_and> ::= <equality> ("&&" <equality>)

<equality> ::= <relational> (("==" | "!=") <relational>)

<relational> ::= <additive> (("<" | ">" | "<=" | ">=") <additive>)

<additive> ::= <multiplicative> (("+" | "-") <multiplicative>)

<multiplicative> ::= <unary> (("*" | "/") <unary>)

<unary> ::= ("!" | "-") <primary> | <primary>

<primary> ::= ID | INT | BOOL | <expression> | "(" <expression> ")"
"""