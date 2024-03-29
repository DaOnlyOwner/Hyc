// ../include/Frontend/Lexer.h generated by reflex 3.0.12 from Hyc.l

#ifndef REFLEX____INCLUDE_FRONTEND_LEXER_H
#define REFLEX____INCLUDE_FRONTEND_LEXER_H
#define IN_HEADER 1
#define REFLEX_VERSION "3.0.12"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  OPTIONS USED                                                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#define REFLEX_OPTION_fast                true
#define REFLEX_OPTION_header_file         "../include/Frontend/Lexer.h"
#define REFLEX_OPTION_lex                 lex
#define REFLEX_OPTION_lexer               Lexer
#define REFLEX_OPTION_noline              true
#define REFLEX_OPTION_outfile             "../src/Frontend/Lexer.cpp"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  SECTION 1: %top user code                                                 //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#include "Token.h"
#include "DebugPrint.h"
#include "fmt/core.h"
#include <vector>
#include <algorithm>
#include <array>
#define PUSH(name) push(Token::Specifier::name)


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  REGEX MATCHER                                                             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <reflex/matcher.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  ABSTRACT LEXER CLASS                                                      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <reflex/abslexer.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  LEXER CLASS                                                               //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class Lexer : public reflex::AbstractLexer<reflex::Matcher> {

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



 public:
  typedef reflex::AbstractLexer<reflex::Matcher> AbstractBaseLexer;
  Lexer(
      const reflex::Input& input = reflex::Input(),
      std::ostream&        os    = std::cout)
    :
      AbstractBaseLexer(input, os)
  {
  }
  static const int INITIAL = 0;
  virtual int lex(void);
  int lex(const reflex::Input& input)
  {
    in(input);
    return lex();
  }
  int lex(const reflex::Input& input, std::ostream *os)
  {
    in(input);
    if (os)
      out(*os);
    return lex();
  }
};

#endif
