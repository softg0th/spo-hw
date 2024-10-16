grammar MyLang;

options {
  language=C;
  output=AST;
  backtrack=true;
}

tokens {
  SourceToken;
  CondToken;
  FuncDefToken;
  FuncSignatureToken;
  ArgListToken;
  TypeRefToken;
  ExpressionToken;
  LoopToken;
  RepeatToken;
  ArgToken;
  Body;
  ReturnToken;
  BreakToken;
  VarDeclToken;
  BlockToken;
  SliceToken;
  RangeToken;
  ArrayToken;
  CallToken;
}

source
  : sourceItem* -> ^(SourceToken sourceItem*)
  ;

sourceItem
  :  'def' funcSignature body 'end'-> ^(FuncDefToken funcSignature body)
  |   varDeclaration -> ^(VarDeclToken varDeclaration)
  ;

funcSignature
  :  identifier '(' argList ')' ('of' typeRef)? -> ^(FuncSignatureToken ^(Identifier identifier) argList? typeRef)
  ;

argList
  :  (arg (',' arg)*)? -> ^(ArgListToken arg*)
  ;

arg
  :  identifier ('of' typeRef)? -> ^(ArgToken ^(Identifier identifier) typeRef?)
  ;

statement
  :  expression ';'!
  |  loopStatement
  |  breakStatement
  |  expressionStatement
  |  condStatement
  |  repeatStatement
  |  returnStatement
  |  varDeclStatement
  |  assignmentStatement
  |  callStatement
  ;

 body
	:	statement* -> ^(Body statement*)
	;

varDeclStatement
    :   'var' identifier ('of' typeRef)? ('=' expression)? -> ^(VarDeclToken identifier typeRef? expression?)
    ;

assignmentStatement
    : identifier '=' expression -> ^(AssignmentOp ^(Identifier identifier) expression)
    ;

callStatement
    : identifier '(' argList ')' -> ^(CallToken ^(Identifier identifier) ^(ArgListToken argList)?)
    ;

condStatement
  :  'if' expression 'then' body ('else' body)?
  -> ^(CondToken ^(ExpressionToken expression) ^(CondToken body) ^(CondToken body)?)
  ;

loopStatement
  :  ('while'|'until') expression body 'end'  -> ^(LoopToken ^(ExpressionToken expression) ^(LoopToken body))
  ;

returnStatement
  : 'return' expression body -> ^(ReturnToken ^(ExpressionToken expression))
  ;

repeatStatement
  :  'repeat' body ('while'|'until') expression -> ^(RepeatToken ^(RepeatToken body) ^(ExpressionToken expression))
  ;

breakStatement
  :  'break' ';'
  ;

expressionStatement
  :  expression ';'
  ;

blockStatement
  : ('begin' | '{') (statement | source | typeRef)* ('end' | '}')
  ;

expression
  :  assignmentExpression
  ;

slice
: expression '[' ranges ']' -> ^(SliceToken expression ranges)
;

ranges
: expression ('..' expression)? -> ^(RangeToken expression (expression)?)
;

assignmentExpression
  :  logicalOrExpression (AssignmentOp^ assignmentExpression)?
  ;

logicalOrExpression
  :  logicalAndExpression ('||'^ logicalAndExpression)*
  ;

logicalAndExpression
  :  bitwiseOrExpression ('&&'^ bitwiseOrExpression)*
  ;

bitwiseOrExpression
  :  bitwiseXorExpression ('|'^ bitwiseXorExpression)*
  ;

bitwiseXorExpression
  :  bitwiseAndExpression ('^'^ bitwiseAndExpression)*
  ;

bitwiseAndExpression
  :  equalityExpression ('&'^ equalityExpression)*
  ;

equalityExpression
  :  relationalExpression (('=='|'!=')^ relationalExpression)*
  ;

relationalExpression
  :  shiftExpression (RelationalOp^ shiftExpression)*
  ;

shiftExpression
  :  additiveExpression (('<<'|'>>')^ additiveExpression)*
  ;

additiveExpression
  :  multiplicativeExpression (('+'|'-')^ multiplicativeExpression)*
  ;

multiplicativeExpression
  :  unaryExpression (('*'|'/'|'%')^ unaryExpression)*
  ;

unaryExpression
  :  ('!'|'~')^ unaryExpression
      |  (UnaryOp^)? primaryExpression
  ;

callExpression
    : identifier '(' argList ')' -> ^(CallToken identifier ArgListToken)
    ;

primaryExpression
  :  '('! assignmentExpression ')'!
  |  Literal
  |  Identifier
  ;

expressionList
  :  (expression (',' expression)*)
  ;

varDeclaration
  : 'var' identifier ('of' typeRef)? ('=' expression)? -> ^(VarDeclToken identifier typeRef? expression?)
  ;

typeRef
    : arrayType
    | identifier
    | builtin
    ;

arrayType
    : 'array' '[' dec ']' typeRef  -> ^(ArrayToken dec)
    ;

dec : Dec;

builtin
  :   BuiltIn
  ;

identifier
  :  Identifier
  ;

Literal
  :  Bool
  |  Bits
  |  Hex
  |  Dec
  |  Char
  |  Str
  ;

fragment
Bool:  ('true'|'false');

fragment
Bits:  '0' ('b'|'B') ('0'..'1')+;

fragment
Hex :  '0' ('x'|'X') ('0'..'9'|'a'..'f'|'A'..'F')+;

fragment
Dec  :  ('0'..'9')+;

fragment
Char:  '\'' ~('\'') '\'';

fragment
Str :  '"' ( ~('"'|'\\') | ('\\'.) )* '"';

BuiltIn
  :  'bool'
  |  'byte'
  |  'int'
  |  'uint'
  |  'long'
  |  'ulong'
  |  'char'
  |  'string'
  ;

Identifier
  :  ('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'A'..'Z'|'_'|'0'..'9')*
  ;

AssignmentOp
  :  ('+'|'-'|'*'|'/'|'%'|'<<'|'>>'|'&'|'^'|'|')? '='
  ;

RelationalOp
  :  ('<'|'<='|'>'|'>=')
  ;

UnaryOp
  :  ('++'|'--'|'+'|'-')
  ;

WS  :  (' '|'\r'|'\t'|'\u000C'|'\n') { $channel=HIDDEN; };