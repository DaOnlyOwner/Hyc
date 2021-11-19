%o header-file="../include/Lexer.h" fast outfile="../src/Lexer.cpp" noline

%top{
#include "Token.h"
#include "DebugPrint.h"
#include "fmt/core.h"
#include <vector>
#include <algorithm>
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


%}
kw_return 	"return"
kw_for		"for"
kw_fulfill	"fulfill"
kw_if		"if"
kw_while	"while"
kw_elif		"elif"
kw_else		"else"
kw_struct	"struct"
kw_contract	"contract"
kw_namespace	"namespace"
kw_continue	"continue"
kw_fptr		"fptr"
ubin		0b[0-1]+
uoct		0[0-7]+
uhex		0x[:xdigit:]+	
udec		0|([1-9]\d*)
ident		[a-zA-Z_'`´][a-zA-Z_'`´0-9]* 
integer 	-?({uhex}|{uoct}|{ubin}|{udec})
float		-?\d*\.\d+([Ee][-+]\d+)?f
double		-?\d*\.\d+([Ee][-+]\d+)?
plus    	"+"
minus   	"-"
asterix 	"*"
slash   	"/"
decl_cpy	":="
decl_mv		":#"
colon		":"
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


%%
\s+
{kw_return}	{ push(Token::Specifier::KwReturn);}
{kw_for}	{ push(Token::Specifier::KwFor);}
{kw_while}	{ push(Token::Specifier::KwWhile);}
{kw_contract}	{ push(Token::Specifier::KwContract);}
{kw_fulfill}	{ push(Token::Specifier::KwFulfill);}
{kw_if}		{ push(Token::Specifier::KwIf);}
{kw_elif}	{ push(Token::Specifier::KwElif);}
{kw_else}	{ push(Token::Specifier::KwElse);}
{kw_struct}	{ push(Token::Specifier::KwStruct);}
{kw_continue}	{ push(Token::Specifier::KwContinue);}
{kw_fptr}	{ push(Token::Specifier::KwFptr);}
{comma}		{ push(Token::Specifier::Comma);}
{rparen_l}	{ push(Token::Specifier::RParenL);}
{rparen_r}	{ push(Token::Specifier::RParenR);}
{integer}	{ push(Token::Specifier::Integer); }
{float}   	{ push(Token::Specifier::Float); }
{double}	{ push(Token::Specifier::Double); }
{plus} 	  	{ push(Token::Specifier::Plus); }
{minus}   	{ push(Token::Specifier::Minus); }
{asterix} 	{ push(Token::Specifier::Asterix); }
{slash}   	{ push(Token::Specifier::Slash); }
{decl_cpy}  	{ push(Token::Specifier::DeclCpy); }
{decl_mv}	{ push(Token::Specifier::DeclMv);  }
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