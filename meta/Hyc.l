%o header-file="../include/Lexer.h" fast outfile="../src/Lexer.cpp" noline debug

%top{
#include "Token.h"
#include <vector>
#include <algorithm>
%} 

%class{
   std::vector<Token> m_tokens;
   size_t m_current_token = -1; // Start before the actual token
   void push(Token::Specifier ttype)
   {
	m_tokens.emplace_back(ttype, str(), matcher().line(), lineno(), columno(), lineno_end(), columno_end());
   }
public:
   // this only has elements when Lexer.lex() is called.
   std::vector<Token> get_tokens()
   {
	return std::move(m_tokens);
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

   void match_token(Token::Specifier type)
   {    
      const Token& token = eat();
      if(token.type != type) 
      {
          printf("not possible to match: %s", token.text.c_str());
	  abort();
      }
    }


%}

ubin		0b[0-1]+
uoct		0[0-7]+
uhex		0x[:xdigit:]+	
udec		[1-9]\d*
ident		[a-zA-Z_'`´][a-zA-Z_'`´0-9]* 
kw_return 	"return"
kw_u8		"u8"
kw_u16		"u16"
kw_u32		"u32"
kw_u64		"u64"
kw_s8		"s8"
kw_s16		"s16"
kw_s32		"s32"
kw_s64		"s64"
kw_float	"float"
kw_double	"double"
integerS64	({uhex}|{uoct}|{ubin}|{udec})
integerU8 	({uhex}|{uoct}|{ubin}|{udec})u8
integerU16 	({uhex}|{uoct}|{ubin}|{udec})u16
integerU32 	({uhex}|{uoct}|{ubin}|{udec})u32
integerU64 	({uhex}|{uoct}|{ubin}|{udec})u64
integerS8 	({uhex}|{uoct}|{ubin}|{udec})s8
integerS16 	({uhex}|{uoct}|{ubin}|{udec})s16
integerS32 	({uhex}|{uoct}|{ubin}|{udec})s32
float		\d*\.\d+([Ee][-+]\d+)?f
double		\d*\.\d+([Ee][-+]\d+)?
plus    	"+"
minus   	"-"
asterix 	"*"
slash   	"/"
decl		":="
equal		"="
semicln 	";"
rparen_l	"("
rparen_r	")"
comma		","

%%
\s+
{kw_u8}		{ push(Token::u8);}
{kw_u16}	{ push(Token::s16);}
{kw_u32}	{ push(Token::s32);}
{kw_u64}	{ push(Token::s64);}
{kw_s8}		{ push(Token::s8);}
{kw_s16}	{ push(Token::s16);}
{kw_s32}	{ push(Token::s32);}
{kw_s64}	{ push(Token::s64);}
{kw_float}	{ push(Token::float);}
{kw_double}	{ push(Token::double);}
{comma}		{ push(Token::comma);}
{rparen_l}	{ push(Token::rparen_l);}
{rparen_r}	{ push(Token::rparen_r);}
{integerS64}	{ push(Token::Specifier::IntegerS64); }
{integerU8} 	{ push(Token::Specifier::IntegerU8); }
{integerU16} 	{ push(Token::Specifier::IntegerU16); }
{integerU32}	{ push(Token::Specifier::IntegerU32); }
{integerU64} 	{ push(Token::Specifier::IntegerU64); }
{integerS8} 	{ push(Token::Specifier::IntegerS8); }
{integerS16} 	{ push(Token::Specifier::IntegerS16); }
{integerS32} 	{ push(Token::Specifier::IntegerS32); }
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
<<EOF>>   	{ push(Token::Specifier::Eof); return 0;}
%%