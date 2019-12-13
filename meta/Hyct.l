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

slash   "/"

%%
\s+
{slash}   { push(Token::Slash); }
.	  { push(Token::Error); }
%%