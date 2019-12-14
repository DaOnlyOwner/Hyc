%o header-file="../include/Lexer.h" fast outfile="../src/Lexer.cpp" noline debug

%top{
#include "Token.h"
#include <vector>
#include <algorithm>
%} 

%class{
   std::vector<Token> m_tokens;
   size_t m_current_token = -1; // Start before the actual token
   void push(Token::Type ttype)
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

   void match_token(Token::Type type)
   {    
      const Token& token = eat();
      if(token.type != type) 
      {
          printf("not possible to match: %s", token.text);
	  abort();
      }
    }


%}

ubin	0b[0-1]+
uoct	0[0-7]+
uhex	0x[:xdigit:]+	
udec	[1-9]\d*
ident	[a-zA-Z_'`´][a-zA-Z_'`´0-9]* 
integer ({uhex}|{uoct}|{ubin}|{udec})(u?(c|i|l))? 
float	\d*\.\d+([Ee][-+]\d+)?
plus    "+"
minus   "-"
asterix "*"
slash   "/"
decl	":="
equal	"="
semicln ";"

%%
\s+
{integer} { push(Token::Integer); }
{float}   { push(Token::Float); }
{plus} 	  { push(Token::Plus); }
{minus}   { push(Token::Minus); }
{asterix} { push(Token::Asterix); }
{slash}   { push(Token::Slash); }
{decl}	  { push(Token::Decl); }
{equal}   { push(Token::Equal); }
{ident}	  { push(Token::Ident); }
{semicln} { push(Token::Semicolon); }
<<EOF>>   { push(Token::Eof); return 0;}
%%