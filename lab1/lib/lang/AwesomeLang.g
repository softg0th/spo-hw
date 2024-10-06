grammar AwesomeLang;
 
options {
    language = C;
    backtrack = true;
}
 
BOOL
    : 'true' | 'false'
    ;
 
IDENTIFIER
    : ('a'..'z' | 'A'..'Z' | '_') ('a'..'z' | 'A'..'Z' | '0'..'9' | '_')*
    ;
 
STRING
    : '"' ('\\' . | ~('\\' | '"'))* '"'
    ;
 
CHAR
    : '\'' ('\\' . | ~('\\' | '\'')) '\''
    ;
 
HEX
    : '0' ('x' | 'X') ('0'..'9' | 'a'..'f' | 'A'..'F')+
    ;
 
BITS
    : '0' ('b' | 'B') ('0' | '1')+
    ;
 
DEC
    : '0'..'9'+
    ;
 
 
list
    : (item (',' item)*)?
    ;
 
item
    : IDENTIFIER ('of' typeRef)?
    ;
 
binOp
    : '+' | '-' | '*' | '/' | '^' | '%' 
    | '<' | '<=' | '>' | '>=' | '==' | '!=' 
    | 'and' | 'or'
    ;
 
unOp
    : '-' | '!'
    ;
 
typeRef
    : builtin
    | IDENTIFIER
    | arrayType
    ;
 
builtin
    : 'bool' | 'byte' | 'int' | 'uint' | 'long' | 'ulong' | 'char' | 'string'
    ;
 
arrayType
    : (builtin | IDENTIFIER) 'array' '[' DEC ']'
    ;
 
funcSignature
    : IDENTIFIER '(' list ')' ('of' typeRef)?
    ;
 
sourceItem
    : funcDef
    ;
 
funcDef
    : 'def' funcSignature statement* 'end'
    ;
 
expr
    : primaryExpr (binOp primaryExpr)*
    ;
 
primaryExpr
    : unOp primaryExpr
    | '(' expr ')'
    | IDENTIFIER '(' list ')'
    | IDENTIFIER
    | literal
 
    ;
 
sliceExpr
    : primaryExpr '[' rangeList ']'
    ;

callExpr
    : IDENTIFIER '(' list ')'
    ;
 
literal
    : BOOL
    | STRING
    | CHAR
    | HEX
    | BITS
    | DEC
    ;
 
range
    : expr ('..' expr)?
    ;
 
rangeList
    : range (',' range)*
    ;
 
statement
    : ifStatement
    | loopStatement
    | repeatStatement
    | breakStatement
    | expressionStatement
    | blockStatement
    ;
 
ifStatement
    : 'if' expr 'then' statement (elseStatement)?
    ;
 
elseStatement
    : 'else' statement
    ;
 
loopStatement
    : ('while' | 'until') expr statement* 'end'
    ;
 
repeatStatement
    : 'repeat' (statement+)? ('while' | 'until') expr ';'
    ;
 
breakStatement
    : 'break' ';'
    ;
 
expressionStatement
    : expr ';'
    ;
 
blockStatement
    : ('begin' | '{') (statement | sourceItem)* ('end' | '}')
    ;