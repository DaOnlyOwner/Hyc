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
   const Token& eat()
   {
      const Token& out = lookahead(1);
      m_current_token++;
      return out;
   }    

   const Token& lookahead(size_t amount) const
   {
      auto minIndex = std::min(m_current_token+amount, m_tokens.size()-1);
      return m_tokens[minIndex];
   }

   const Token& match_token(Token::Specifier type)
   {    
      const Token& token = eat();
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
kw_u8		"u8"
kw_u16		"u16"
kw_u32		"u32"
kw_uint		"uint"
kw_s8		"i8"
kw_s16		"i16"
kw_s32		"i32"
kw_int		"int"
kw_float	"float"
kw_double	"double"
ubin		0b[0-1]+
uoct		0[0-7]+
uhex		0x[:xdigit:]+	
udec		[1-9]\d*
ident		[a-zA-Z_'`´][a-zA-Z_'`´0-9]* 
integer 	-?({uhex}|{uoct}|{ubin}|{udec})
float		-?\d*\.\d+([Ee][-+]\d+)?f
double		-?\d*\.\d+([Ee][-+]\d+)?
plus    	"+"
minus   	"-"
asterix 	"*"
slash   	"/"
decl		":="
equal		"="
semicln 	";"
rparen_l	"("
rparen_r	")"
brace_l		"{"
brace_r		"}"
comma		","


%%
\s+
/*{kw_u8}		{ push(Token::Specifier::kw_u8);}
{kw_u16}	{ push(Token::Specifier::kw_s16);}
{kw_u32}	{ push(Token::Specifier::kw_s32);}
{kw_uint}	{ push(Token::Specifier::kw_uint);}
{kw_s8}		{ push(Token::Specifier::kw_s8);}
{kw_s16}	{ push(Token::Specifier::kw_s16);}
{kw_s32}	{ push(Token::Specifier::kw_s32);}
{kw_int}	{ push(Token::Specifier::kw_int);}
{kw_float}	{ push(Token::Specifier::kw_float);}
{kw_double}	{ push(Token::Specifier::kw_double);}*/
{kw_return}	{ push(Token::Specifier::kw_return);}
{comma}		{ push(Token::Specifier::comma);}
{rparen_l}	{ push(Token::Specifier::rparen_l);}
{rparen_r}	{ push(Token::Specifier::rparen_r);}
{integer}	{ push(Token::Specifier::Integer); }
{float}   	{ push(Token::Specifier::Float); }
{double}	{ push(Token::Specifier::Double); }
{plus} 	  	{ push(Token::Specifier::Plus); }
{minus}   	{ push(Token::Specifier::Minus); }
{asterix} 	{ push(Token::Specifier::Asterix); }
{slash}   	{ push(Token::Specifier::Slash); }
{decl}	  	{ push(Token::Specifier::Decl); }
{equal}   	{ push(Token::Specifier::Equal); }
{ident}	  	{ push(Token::Specifier::Ident); }
{semicln} 	{ push(Token::Specifier::Semicolon); }
{brace_l}	{ push(Token::Specifier::brace_l); }
{brace_r}	{ push(Token::Specifier::brace_r); }
<<EOF>>   	{ push(Token::Specifier::Eof); return 0;}
%%