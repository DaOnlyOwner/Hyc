grammar Hyra;

s : stmt_list EOF;
stmt : 
atom_stmt 
| if_stmt        
| while_stmt               
| var_decdef_stmt           
| def_stmt               
| impl_stmt          
| return_stmt   
;

def_stmt : 
var_def_stmt 
| func_def_stmt
| struct_def_stmt 
| union_def_stmt 
| trait_def_stmt 
//| plugin_def_stmt 
| enum_def_stmt
;

stmt_list: stmt*;

// expressions
expr : 
'(' expr ')'
| constant_expr 
| '(' (expr ',')+ expr ')'
| expr?// '..' expr? ('->' expr)?
| expr ('.' | '->') expr
| expr ( '++' | '--' | 'as' generic_type_specifier | '(' arg_call_list ')' | '[' expr ']' | '[' expr? ':' expr? ']')
| <assoc=right> ('sizeof' | '++' | '--' | '~' | '!' | '-' | '+' | 'addr' | 'val') expr  
| var_def_lhs ( '=' | '*=' | '/=' | '%=' | '+=' | '-=' | 'ls=' | 'rs=' | '&=' | '|=' | '^=') expr
| <assoc=right> expr '?' expr ':' expr
| expr ('or' | '||') expr
| expr ('and' | '&&') expr
| expr '|' expr
| expr '^' expr
| expr '&' expr
| expr ('!=' | '==') expr
| expr ('>=' | '<=' | '>' | '<') expr
| expr ('rs'|'ls') expr
| expr ('+' | '-') expr
| expr ('%' | '*' | '/') expr
;

// patterns
pattern : 
| tuple_destr_pattern
| union_destr_pattern
| expr
;
tuple_destr_pattern: '(' ((pattern ',')* pattern)? ')';
union_destr_pattern: Identifier pattern;

// statements
atom_stmt : expr ';';
if_stmt : 'if' expr  block_or_single;
while_stmt : 'while' expr block_or_single;
func_def_stmt : func_proto ( block | '=' stmt );
var_decdef_stmt : var_decdef ';';
var_def_stmt : ('var' | generic_type_specifier) var_def_lhs '=' expr ';';
struct_def_stmt:AccessModifier? 'struct' Identifier generic_def_helper? '{' struct_body '}';
union_def_stmt: AccessModifier? 'union' Identifier generic_def_helper? '{' union_body '}';
trait_def_stmt : AccessModifier? 'contract' Identifier generic_def_helper? '{' contract_body '}';
enum_def_stmt : AccessModifier? 'enum' Identifier '{' enum_body '}';
impl_stmt : 'impl' (normal_impl_helper | generic_impl_helper) '{' func_def_stmt* '}';
match_stmt : 'match' expr '{' match_body '}';
return_stmt : 'return' expr ';';

// Helper
var_decdef : 'noinit'? (tuple_var_destr | normal_var) ('(' arg_call_list ')')?;
tuple_var_destr : tuple_type tuple_destr; // TODO: Naming of rule
normal_var : generic_type_specifier ( Identifier | Wildcard );	  // TODO: Naming of rule

block: '{' stmt_list '}';
block_or_single : block | ':' stmt;
arg_def_list : '(' ((arg ',')* (arg))? ')';
arg: ('in' | 'out')? generic_type_specifier Identifier;
arg_call_list : ((expr ',')+ expr)?;
func_proto : ('var' | generic_type_specifier) Identifier (arg_def_list | generic_arg_def_list arg_def_list generic_constraints?);
generic_arg_def_list : '<' (Identifier constraint? ',')* Identifier constraint? '>';
generic_arg_call_list : '<' (generic_type_specifier ',')* generic_type_specifier '>';

generic_constraints : 'where' (generic_type_specifier constraint ',')* generic_type_specifier constraint;
generic_type_specifier : types generic_arg_call_list? 'ptr'*;
generic_def_helper : generic_arg_def_list generic_constraints?;
generic_impl_helper : generic_arg_def_list generic_type_specifier 'for' generic_type_specifier generic_constraints;
constraint : ':' ((generic_type_specifier '+')* generic_type_specifier | ('spec' generic_type_specifier));
normal_impl_helper : Identifier 'for' Identifier;
types : tuple_type | simple_types;
tuple_type : '(' (generic_type_specifier ',')+ generic_type_specifier ')';
simple_types : Float | Double | SByte | Short | Int | Long | Byte | UShort | UInt | ULong | Identifier;
struct_body : (SectionAccessModifier? (var_decdef_stmt | def_stmt) )*;
union_body : (var_decdef_stmt | var_def_stmt)*;
contract_body : (func_def_stmt | (func_proto ';'))+;
match_body : pattern '=>' (atom_stmt | return_stmt);
enum_body : (Identifier ',')* Identifier;
var_def_lhs : Wildcard | Identifier | tuple_destr;
tuple_destr : '(' Identifier ',' Identifier ')' | '(' (tuple_destr ',')+ tuple_destr ')';


constant_expr : 
numeric_constant_expr                 
| boolean_constant_expr               
| variable_expr                       
;
variable_expr : Identifier;
numeric_constant_expr : (Integer | Floating);
boolean_constant_expr : ('true' | 'false');
// TODO:
//new_expr : 'new' type_specifier '(' arg_call_list ')';
//del_expr : 'del' type_specifier;
//brace_init : '{' expr* '}'; 
//brace_field_init_expr : '{' (Identifier '=' expr)+ '}';
// shared, owned etc. pointer annotations when core lib is defined

// "complex" Tokens
SectionAccessModifier : AccessModifier ':';
AccessModifier : ('public' | 'private');
Float : 'f32' | 'float';
Double : 'f64' | 'double';
SByte : 'sbyte' | 's8';
Short : 'short' | 's16'; 
Int : 'int' | 's32';
Long : 'long' | 's64';
Byte : 'byte' | 'u8';
UShort : 'ushort' | 'u16';
UInt : 'uint' | 'u32';
ULong : 'ulong' | 'u64';
Integer : [0-9]+;
Floating : [0-9]* '.' [0-9]+ ;
Wildcard : '_';
Identifier : [_a-zA-Z][_´`a-zA-Z0-9]*;

fragment NEWLINE : '\r'? '\n' | '\r' | '\f' SPACES?;
fragment SPACES : [ \t]+;
fragment COMMENT : ('//' ~[\r\n\f]*) | ('/*' .*? '*/');



fragment LINE_JOINING
 : '\\' SPACES? ( '\r'? '\n' | '\r' | '\f')
 ;

 SKIP_
 : ( NEWLINE | SPACES | COMMENT | LINE_JOINING ) -> channel(HIDDEN);

