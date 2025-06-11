grammar Rexion;

// Program Root
program
    : (importStmt | statement)* EOF
    ;

// Top-level Statements
statement
    : funcDecl
    | classDecl
    | moduleDecl
    | varDecl ';'
    | assignStmt ';'
    | controlStmt
    | loopStmt
    | breakStmt ';'
    | continueStmt ';'
    | returnStmt ';'
    | throwStmt ';'
    | tryCatchBlock
    | printStmt ';'
    | expressionStmt ';'
    ;

// Modules
moduleDecl
    : 'module' IDENTIFIER block
    ;

// Classes and Objects
classDecl
    : 'class' IDENTIFIER (':' IDENTIFIER)? block
    ;

// Variable Declarations
varDecl
    : 'let' IDENTIFIER (':' type)? ('=' expression)?
    ;

// Function Declaration
funcDecl
    : 'func' IDENTIFIER '(' paramList? ')' (':' type)? block
    ;

// Parameters
paramList
    : param (',' param)*
    ;
param
    : IDENTIFIER (':' type)?
    ;

// Blocks
block
    : '{' statement* '}'
    ;

// Function Calls
funcCall
    : IDENTIFIER '(' argList? ')'
    ;

// Arguments
argList
    : expression (',' expression)*
    ;

// Expressions
expression
    : assignmentExpr
    ;

assignmentExpr
    : conditionalExpr
    | IDENTIFIER '=' assignmentExpr
    ;

conditionalExpr
    : logicalOrExpr ('?' expression ':' expression)?
    ;

logicalOrExpr
    : logicalAndExpr ('||' logicalAndExpr)*
    ;

logicalAndExpr
    : equalityExpr ('&&' equalityExpr)*
    ;

equalityExpr
    : relationalExpr (('==' | '!=') relationalExpr)*
    ;

relationalExpr
    : additiveExpr (('<' | '<=' | '>' | '>=') additiveExpr)*
    ;

additiveExpr
    : multiplicativeExpr (('+' | '-') multiplicativeExpr)*
    ;

multiplicativeExpr
    : unaryExpr (('*' | '/' | '%') unaryExpr)*
    ;

unaryExpr
    : ('+' | '-' | '!') unaryExpr
    | primaryExpr
    ;

primaryExpr
    : literal
    | IDENTIFIER
    | funcCall
    | '(' expression ')'
    ;

// Expression Statement
expressionStmt
    : expression
    ;

// Print Statement
printStmt
    : 'print' expression
    ;

// Assignment
assignStmt
    : IDENTIFIER '=' expression
    ;

// Loops
loopStmt
    : whileLoop
    | forLoop
    ;

whileLoop
    : 'while' '(' expression ')' block
    ;

forLoop
    : 'for' '(' (varDecl | assignStmt)? ';' expression? ';' expression? ')' block
    ;

// Control
controlStmt
    : ifStmt
    ;

ifStmt
    : 'if' '(' expression ')' block ('else' block)?
    ;

// Return
returnStmt
    : 'return' expression?
    ;

// Break and Continue
breakStmt
    : 'break'
    ;

continueStmt
    : 'continue'
    ;

// Throw / Try / Catch
throwStmt
    : 'throw' expression
    ;

tryCatchBlock
    : 'try' block 'catch' '(' IDENTIFIER ')' block
    ;

// Import Statements
importStmt
    : 'import' STRING ';'
    ;

// Types
type
    : 'int'
    | 'float'
    | 'string'
    | 'bool'
    | 'void'
    | IDENTIFIER
    ;

// Literals
literal
    : NUMBER
    | STRING
    | BOOLEAN
    | 'null'
    ;

// Tokens
BOOLEAN     : 'true' | 'false' ;
IDENTIFIER  : [a-zA-Z_][a-zA-Z_0-9]* ;
NUMBER      : [0-9]+ ('.' [0-9]+)? ;
STRING      : '"' ( ~["\\] | '\\' . )* '"' ;

// Whitespace and Comments
WS          : [ \t\r\n]+ -> skip ;
LINE_COMMENT: '//' ~[\r\n]* -> skip ;
BLOCK_COMMENT: '/*' .*? '*/' -> skip ;
