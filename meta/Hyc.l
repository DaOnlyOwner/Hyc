%o header-file="../include/Lexer.h" fast outfile="../src/Lexer.cpp" noline debug

%top{
#include "Token.h"
#include <vector>
%} 

%class{
   std::vector<Token> m_tokens;
   void push(Token::TokenType ttype)
   {
	m_tokens.emplace_back(ttype, str(), matcher().line(), lineno(), columno(), lineno_end(), columno_end());
   }
public:
   // this only has elements when Lexer.lex() is called.
   std::vector<Token> get_tokens()
   {
	return std::move(m_tokens);
   }
   
   Token next()
   {
	
   }    


%}

ubin	0b[0-1]+
uoct	0[0-7]+
uhex	0x[:xdigit:]+	
udec	[1-9][:digit:]*
integer ({uhex}|{uoct}|{ubin}|{udec})(u?(c|i|l))? 
float	\d*\.\d+([Ee][-+]\d+)?
plus    "+"
minus   "-"
asterix "*"
slash   "/"

%%
\s+
{integer} { push(Token::Integer); }
{float}   { push(Token::Float); }
{plus} 	  { push(Token::Plus); }
{minus}   { push(Token::Minus); }
{asterix} { push(Token::Asterix); }
{slash}   { push(Token::Slash); }
%%