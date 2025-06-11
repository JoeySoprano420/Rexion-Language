
grammar Rexion;

program         : statement* EOF;

statement
    : import_stmt
    | define_stmt
    | assign_stmt
    | func_decl
    | func_call
    | control_stmt
    | print_stmt
    | memory_stmt
    | logic_stmt
    | io_stmt
    | flow_stmt
    ;

import_stmt     : 'import' STRING_LITERAL ';';

define_stmt     : 'define' IDENTIFIER ':' type_spec ';';

type_spec
    : 'integer'
    | 'decimal'
    | 'string'
    | 'boolean'
    | 'list' '<' type_spec '>'
    | 'array' '<' type_spec '>'
    | 'module'
    | 'class'
    | 'object'
    ;

assign_stmt     : IDENTIFIER '=' expression ';';

expression      : term (('+' | '-') term)*;
term            : factor (('*' | '/') factor)*;
factor          : NUMBER
                | STRING_LITERAL
                | IDENTIFIER
                | func_call
                | '(' expression ')';

func_decl       : 'func' IDENTIFIER '(' param_list? ')' block;
param_list      : IDENTIFIER (',' IDENTIFIER)*;

func_call       : IDENTIFIER '(' arg_list? ')' ';';
arg_list        : expression (',' expression)*;

block           : '{' statement* '}';

control_stmt
    : 'if' condition block ('else' block)?
    | 'while' condition block
    | 'for' IDENTIFIER 'in' expression block
    | 'return' expression? ';';

condition       : expression comparator expression;
comparator      : '==' | '!=' | '<' | '<=' | '>' | '>=';

print_stmt      : 'print' expression ';';

io_stmt
    : 'input' IDENTIFIER ':' type_spec ';'
    | 'output' expression ';';

memory_stmt
    : 'allocate' IDENTIFIER 'in' ('stack' | 'heap') ';'
    | 'deallocate' IDENTIFIER ';'
    | 'mutex' IDENTIFIER block
    | 'wipe' IDENTIFIER ';';

flow_stmt
    : 'pop' IDENTIFIER ';'
    | 'jump' IDENTIFIER ';'
    | 'mov' REGISTER ',' value ';'
    | 'bump' IDENTIFIER ';';

logic_stmt
    : 'assert' condition ';'
    | 'assume' condition ';'
    | 'diagnose' expression ';'
    | 'raise' error_level STRING_LITERAL ';'
    | 'error' STRING_LITERAL ';'
    | 'flag' IDENTIFIER ';'
    | 'throw' STRING_LITERAL ';'
    | 'ignore' error_level ';'
    | 'bypass' IDENTIFIER ';'
    | 'pass' IDENTIFIER ';';

error_level     : 'minor' | 'major' | 'fatal';

value           : NUMBER | STRING_LITERAL | IDENTIFIER;

class_decl      : 'class' IDENTIFIER block;
object_decl     : 'object' IDENTIFIER block;
module_decl     : 'module' IDENTIFIER block;

enum_decl       : 'enum' IDENTIFIER '{' IDENTIFIER (',' IDENTIFIER)* '}';

special_cmd     : 'compile' | 'interpret' | 'execute' | 'run' | 'build';

STRING_LITERAL  : '"' .*? '"';
NUMBER          : DIGIT+ ('.' DIGIT+)?;
IDENTIFIER      : LETTER (LETTER | DIGIT | '_')*;
REGISTER        : 'eax' | 'ebx' | 'ecx' | 'edx' | 'rsi' | 'rdi' | 'rsp' | 'rbp';

fragment DIGIT  : [0-9];
fragment LETTER : [a-zA-Z];

WS              : [ \t\r\n]+ -> skip;
COMMENT         : '//' ~[\n]* -> skip;
