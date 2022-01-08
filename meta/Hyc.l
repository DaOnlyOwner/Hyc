%o header-file="../include/Frontend/Lexer.h" fast outfile="../src/Frontend/Lexer.cpp" noline

%top{
#include "Token.h"
#include "DebugPrint.h"
#include "fmt/core.h"
#include <vector>
#include <algorithm>
#include <array>
#define PUSH(name) push(Token::Specifier::name)
%} 

%class{
   std::vector<Token> m_tokens;
   size_t m_current_token = -1; // Start before the actual token
   std::string file;
   void push(Token::Specifier ttype)
   {
	m_tokens.emplace_back(ttype, str(), file, matcher().line(), lineno(), columno(), lineno_end(), columno_end());
   }

public:
   typedef size_t TokenPos;
   TokenPos get_tk_pos(){return m_current_token;}
   void goto_tk_pos(TokenPos tp) {m_current_token = tp;}
   void set_filename(const std::string& filename)
   {
        file=filename;
   }
   Token& eat()
   {
      Token& out = lookahead(1);
      m_current_token++;
      return out;
   }

   const Token& lookahead(size_t amount) const
   {
      auto minIndex = std::min(m_current_token+amount, m_tokens.size()-1);
      return m_tokens[minIndex];
   }

   bool is_at(Token::Specifier spec, size_t amount) const
   {
	return spec == lookahead(1).type;
   }

   Token& lookahead(size_t amount)
   {
       auto minIndex = std::min(m_current_token + amount, m_tokens.size() - 1);
       return m_tokens[minIndex];
   }

   Token& match_token(Token::Specifier type)
   {
      Token& token = eat();
      if(token.type != type)
      {
          auto descr = Error::FromToken(token);
          descr.Message = fmt::format("Expected {}, but got {}",Token::Translate(type),Token::Translate(token.type));
	  descr.Hint = fmt::format("The offending token is '{}'",token.text);
          Error::SyntacticalError(descr);
      }
      return token;
    }

template<Token::Specifier... specs>
   Token& match_one_of()
   {
       Token& token = eat();
       // One of the arguments is the token type
       if (((token.type == specs) || ... ))
       {
           return token;
       }
       else
       {
           std::array<Token::Specifier, sizeof... (specs)> specs_ary = { specs... };
           std::string exp = "";
           for (auto& spec : specs_ary)
           {
               exp += fmt::format("{}, ", Token::Translate(spec));
           }
           auto descr = Error::FromToken(token);
           descr.Message = fmt::format("Expected one of '{}' but got '{}'", exp, Token::Translate(token.type));
           descr.Hint = fmt::format("The offending token is '{}'", token.text);
           Error::SyntacticalError(descr);
       }
   }


%}
kw_return 	"return"
kw_for		"for"
kw_union    "union"
kw_if		"if"
kw_while	"while"
kw_elif		"elif"
kw_else		"else"
kw_struct	"struct"
kw_namespace	"namespace"
kw_continue	"continue"
kw_fptr		"fptr"
kw_as		"as"
kw_throw	"throw"
kw_match	"match"
kw_case		"case"
kw_operator	"operator"
kw_auto		"auto"
kw_del		"del"
kw_moved	"moved"
kw_sizeof	"sizeof"
kw_offsetof	"offsetof"
kw_alignof	"alignof"
ubin		0b[0-1]+
uoct		0[0-7]+
uhex		0x[:xdigit:]+	
udec		0|([1-9]\d*)
ident		[a-zA-Z_][a-zA-Z_0-9]* 
int_	 	({uhex}|{uoct}|{ubin}|{udec})i?
uint		{int_}ui
half		{int_}h
uhalf		{int_}uh
short		{int_}s
ushort		{int_}us
char		{int_}c
uchar		{int_}uc
float		\d*\.\d+([Ee][-+]\d+)?f
double		\d*\.\d+([Ee][-+]\d+)?
quad		\d*\.\d+([Ee][-+]\d+)?q
plus    	"+"
hashtag		"#"
minus   	"-"
asterix 	"*"
slash   	"/"
gen_fcall_open  "<."
gen_fcall_close ".>"
decl_cpy	":="
colon		":"
double_colon	"::"
less		"<"
greater		">"
equal		"="
semicln 	";"
rparen_l	"("
rparen_r	")"
brace_l		"{"
brace_r		"}"
bracket_r	"]"
bracket_l	"["
comma		","
double_plus	"++"
double_minus	"--"
ex_mark		"!"
tilde		"~"
amp		"&"
perc		"%"
sr		"|>"
sl		"<|"
array_start	"[."
array_end	".]"
thrway		"<=>"
lessEql		"<="
grEql		">="
double_eql	"=="
not_equal	"!="
caret		"^"
or		"|"
mem_cpy		"-->"
mem_set		"--*"
mem_move	"--+"
dot   "."
mem_acc "->"
dot_comma ".,"
mem_acc_comma "->,"
double_qm	"??"
double_em	"!!"
double_amp	"&&"
double_or	"||"
questionmark	"?"
plus_eql	"+="
minus_eql	"-="
asterix_eql	"*="
slash_eql	"/="
percent_equal	"%="
sl_eql		"<|="
sr_eql		"|>="
amp_eql		"&="
caret_eql	"^="
or_eql		"|="

%%
\s+
"//"[^\n]*
[$][^$]*[$]
{kw_return}	{ push(Token::Specifier::KwReturn);}
{kw_for}	{ push(Token::Specifier::KwFor);}
{kw_while}	{ push(Token::Specifier::KwWhile);}
{kw_if}		{ push(Token::Specifier::KwIf);}
{kw_elif}	{ push(Token::Specifier::KwElif);}
{kw_else}	{ push(Token::Specifier::KwElse);}
{kw_struct}	{ push(Token::Specifier::KwStruct);}
{kw_continue}	{ push(Token::Specifier::KwContinue);}
{kw_fptr}	{ push(Token::Specifier::KwFptr);}
{kw_as}		{ push(Token::Specifier::KwAs);}
{kw_sizeof}	{ push(Token::Specifier::KwSizeof);}
{kw_alignof}	{ push(Token::Specifier::KwAlignof);}
{kw_offsetof}	{ push(Token::Specifier::KwOffsetof);}
{kw_throw}	{ PUSH(KwThrow);}
{kw_union}  { PUSH(KwUnion);}
{kw_namespace} {PUSH(KwNamespace);}
{kw_match}	{PUSH(KwMatch);}
{kw_case}	{PUSH(KwCase);}
{kw_operator}	{PUSH(KwOperator);}
{kw_auto}	{PUSH(KwAuto);}
{kw_del}	{PUSH(KwDel);}
{kw_moved}	{PUSH(KwMoved);}
{double_colon}	{ PUSH(DoubleColon);}
{dot_comma}	{ PUSH(DotComma);}
{mem_acc_comma} { PUSH(MemAccComma);}
{double_plus}	{ PUSH(DoublePlus);}
{double_minus}	{ PUSH(DoubleMinus);}
{ex_mark}	{ PUSH(ExclMark);}
{double_qm}	{ PUSH(DoubleQM);}
{double_em}	{ PUSH(DoubleEM);}
{tilde}		{ PUSH(Tilde);}
{gen_fcall_open} { PUSH(GenFCallOpen);}
{gen_fcall_close} {PUSH(GenFCallClose);}
{amp}		{ PUSH(Ampersand);}
{perc}		{ PUSH(Percent);}
{sr}		{ PUSH(ShiftRight);}
{sl}		{ PUSH(ShiftLeft);}
{thrway}		{ PUSH(ThreeWay);}
{lessEql}		{ PUSH(LessEql);}
{grEql}		{ PUSH(GreaterEql);}
{mem_cpy}	{ PUSH(MemCpy);}
{mem_move}	{ PUSH(MemMove);}
{mem_set}	{ PUSH(MemSet);}
{double_eql}	{ PUSH(DoubleEqual);}
{not_equal}	{ PUSH(NotEqual);}
{caret}		{ PUSH(Caret);}
{or}   		{ PUSH(Or);}
{array_start}	{ PUSH(ArrayStart);}
{array_end}	{ PUSH(ArrayEnd);}
{hashtag}	{ PUSH(Hashtag);}
{double_amp}	{ PUSH(DoubleAmpersand);}
{double_or}	{ PUSH(DoubleOr);}
{questionmark}	{ PUSH(QuestionMark);}
{plus_eql}	   { PUSH(PlusEqual);}
{minus_eql}  { PUSH(MinusEqual);}
{asterix_eql}	{ PUSH(AsterixEqual);}
{slash_eql}	{ PUSH(SlashEqual);}
{percent_equal}	{ PUSH(PercentEqual);}
{sl_eql}		{ PUSH(SlEqual);}
{sr_eql}		{ PUSH(SrEqual);}
{amp_eql}		{ PUSH(AmpersandEqual);}
{caret_eql}	{ PUSH(CaretEqual);}
{or_eql}		{ PUSH(OrEqual);}
{dot}       { PUSH(Dot);}
{mem_acc}   { PUSH(MemAccess);}
{comma}		{ push(Token::Specifier::Comma);}
{rparen_l}	{ push(Token::Specifier::RParenL);}
{rparen_r}	{ push(Token::Specifier::RParenR);}
{float}   	{ push(Token::Specifier::Float); }
{double}	{ push(Token::Specifier::Double); }
{quad}		{ push(Token::Specifier::Quad); }
{plus} 	  	{ push(Token::Specifier::Plus); }
{int_}		{ push(Token::Specifier::Int);}
{uint}		{ push(Token::Specifier::UInt);}
{half}		{ push(Token::Specifier::Half);}
{uhalf}		{ push(Token::Specifier::UHalf);}
{char}		{ push(Token::Specifier::Char);}
{uchar}		{ push(Token::Specifier::UChar);}
{short}		{ push(Token::Specifier::Short);}
{ushort}	{ push(Token::Specifier::UShort);}
{minus}   	{ push(Token::Specifier::Minus); }
{asterix} 	{ push(Token::Specifier::Asterix); }
{slash}   	{ push(Token::Specifier::Slash); }
{decl_cpy}  	{ push(Token::Specifier::DeclCpy); }
{colon}		{ push(Token::Specifier::Colon); }
{equal}   	{ push(Token::Specifier::Equal); }
{ident}	  	{ push(Token::Specifier::Ident); }
{semicln} 	{ push(Token::Specifier::Semicolon); }
{brace_l}	{ push(Token::Specifier::BraceL); }
{brace_r}	{ push(Token::Specifier::BraceR); }
{bracket_r}	{ push(Token::Specifier::BracketR); }
{bracket_l}	{ push(Token::Specifier::BracketL);}
{less}		{ push(Token::Specifier::Less); }
{greater}	{ push(Token::Specifier::Greater); }
<<EOF>>   	{ push(Token::Specifier::Eof); return 0;}
%%