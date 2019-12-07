grammar Hyra;

compilationUnit : stmtList EOF;

expr : '(' expr ')' #parenExpr
| constantExpr 	#constExpr
| lh=expr '|' rh=expr #bitwiseOrExpr
| lh=expr '^'#bitwiseNegateExpr
| lh=expr '&' rh=expr #bitwiseAndExpr
| lh=expr op=('+' | '-') rh=expr #AddSubExpr
| lh=expr op=('%' | '*' | '/') rh=expr #MulDivModExpr
| lh=pattern '=' rh=expr #assignmentExpr
;

constantExpr : literal 
| identifier;


stmt : atomicStmt
| decStmt
| blockStmt;

stmtList : stmt*;

atomicStmt: expr ';';
decStmt : 'var' pattern '=' expr ';';  
blockStmt : ifStmt
| simpleBlockStmt;

ifStmt : 'if' '(' expr ')' ('{' stmtList '}' | 'then' stmt ';');
simpleBlockStmt : '{' stmtList '}';

pattern : literal
| identifier;

literal : DecIntegerLiteral | FloatLiteral;

identifier : Identifier;
Identifier : [a-zA-Z]+;

fragment DIGIT : [0-9]+;
DecIntegerLiteral : DIGIT+;
FloatLiteral : DecIntegerLiteral '.' DecIntegerLiteral;

Whitespace
    :   [ \t]+
        -> skip
    ;

Newline
    :   (   '\r' '\n'?
        |   '\n'
        )
        -> skip
    ;

BlockComment
    :   '/*' .*? '*/'
        -> skip
    ;

LineComment
    :   '//' ~[\r\n]*
        -> skip
    ;





