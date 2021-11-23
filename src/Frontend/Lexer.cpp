// ../src/Frontend/Lexer.cpp generated by reflex 3.0.12 from Hyc.l

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
               exp += fmt::format("{}, ", Token::Translate(spec.type));
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

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  SECTION 2: rules                                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

extern void reflex_code_INITIAL(reflex::Matcher&);

int Lexer::lex(void)
{
  static const reflex::Pattern PATTERN_INITIAL(reflex_code_INITIAL);
  if (!has_matcher())
  {
    matcher(new Matcher(PATTERN_INITIAL, stdinit(), this));
  }
  while (true)
  {
        switch (matcher().scan())
        {
          case 0:
            if (matcher().at_end())
            {
{ push(Token::Specifier::Eof); return 0;}
            }
            else
            {
              out().put(matcher().input());
            }
            break;
          case 1: // rule Hyc.l:165: \s+ :
            break;
          case 2: // rule Hyc.l:166: {kw_return} :
{ push(Token::Specifier::KwReturn);}
            break;
          case 3: // rule Hyc.l:167: {kw_for} :
{ push(Token::Specifier::KwFor);}
            break;
          case 4: // rule Hyc.l:168: {kw_while} :
{ push(Token::Specifier::KwWhile);}
            break;
          case 5: // rule Hyc.l:169: {kw_if} :
{ push(Token::Specifier::KwIf);}
            break;
          case 6: // rule Hyc.l:170: {kw_elif} :
{ push(Token::Specifier::KwElif);}
            break;
          case 7: // rule Hyc.l:171: {kw_else} :
{ push(Token::Specifier::KwElse);}
            break;
          case 8: // rule Hyc.l:172: {kw_struct} :
{ push(Token::Specifier::KwStruct);}
            break;
          case 9: // rule Hyc.l:173: {kw_continue} :
{ push(Token::Specifier::KwContinue);}
            break;
          case 10: // rule Hyc.l:174: {kw_fptr} :
{ push(Token::Specifier::KwFptr);}
            break;
          case 11: // rule Hyc.l:175: {kw_as} :
{ push(Token::Specifier::KwAs);}
            break;
          case 12: // rule Hyc.l:176: {kw_throw} :
{ PUSH(KwThrow);}
            break;
          case 13: // rule Hyc.l:177: {kw_union} :
{ PUSH(KwUnion);}
            break;
          case 14: // rule Hyc.l:178: {kw_namespace} :
{PUSH(KwNamespace);}
            break;
          case 15: // rule Hyc.l:179: {kw_match} :
{PUSH(KwMatch);}
            break;
          case 16: // rule Hyc.l:180: {kw_case} :
{PUSH(KwCase);}
            break;
          case 17: // rule Hyc.l:181: {kw_operator} :
{PUSH(KwOperator);}
            break;
          case 18: // rule Hyc.l:182: {double_colon} :
{ PUSH(DoubleColon);}
            break;
          case 19: // rule Hyc.l:183: {double_plus} :
{ PUSH(DoublePlus);}
            break;
          case 20: // rule Hyc.l:184: {double_minus} :
{ PUSH(DoubleMinus);}
            break;
          case 21: // rule Hyc.l:185: {ex_mark} :
{ PUSH(ExclMark);}
            break;
          case 22: // rule Hyc.l:186: {tilde} :
{ PUSH(Tilde);}
            break;
          case 23: // rule Hyc.l:187: {amp} :
{ PUSH(Ampersand);}
            break;
          case 24: // rule Hyc.l:188: {perc} :
{ PUSH(Percent);}
            break;
          case 25: // rule Hyc.l:189: {sr} :
{ PUSH(ShiftRight);}
            break;
          case 26: // rule Hyc.l:190: {sl} :
{ PUSH(ShiftLeft);}
            break;
          case 27: // rule Hyc.l:191: {thrway} :
{ PUSH(ThreeWay);}
            break;
          case 28: // rule Hyc.l:192: {lessEql} :
{ PUSH(LessEql);}
            break;
          case 29: // rule Hyc.l:193: {grEql} :
{ PUSH(GreaterEql);}
            break;
          case 30: // rule Hyc.l:194: {double_eql} :
{ PUSH(DoubleEqual);}
            break;
          case 31: // rule Hyc.l:195: {not_equal} :
{ PUSH(NotEqual);}
            break;
          case 32: // rule Hyc.l:196: {caret} :
{ PUSH(Caret);}
            break;
          case 33: // rule Hyc.l:197: {or} :
{ PUSH(Or);}
            break;
          case 34: // rule Hyc.l:198: {double_amp} :
{ PUSH(DoubleAmpersand);}
            break;
          case 35: // rule Hyc.l:199: {double_or} :
{ PUSH(DoubleOr);}
            break;
          case 36: // rule Hyc.l:200: {questionmark} :
{ PUSH(QuestionMark);}
            break;
          case 37: // rule Hyc.l:201: {plus_eql} :
{ PUSH(PlusEqual);}
            break;
          case 38: // rule Hyc.l:202: {minus_eql} :
{ PUSH(MinusEqual);}
            break;
          case 39: // rule Hyc.l:203: {asterix_eql} :
{ PUSH(AsterixEqual);}
            break;
          case 40: // rule Hyc.l:204: {slash_eql} :
{ PUSH(SlashEqual);}
            break;
          case 41: // rule Hyc.l:205: {percent_equal} :
{ PUSH(PercentEqual);}
            break;
          case 42: // rule Hyc.l:206: {sl_eql} :
{ PUSH(SlEqual);}
            break;
          case 43: // rule Hyc.l:207: {sr_eql} :
{ PUSH(SrEqual);}
            break;
          case 44: // rule Hyc.l:208: {amp_eql} :
{ PUSH(AmpersandEqual);}
            break;
          case 45: // rule Hyc.l:209: {caret_eql} :
{ PUSH(CaretEqual);}
            break;
          case 46: // rule Hyc.l:210: {or_eql} :
{ PUSH(OrEqual);}
            break;
          case 47: // rule Hyc.l:211: {dot} :
{ PUSH(Dot);}
            break;
          case 48: // rule Hyc.l:212: {mem_acc} :
{ PUSH(MemAccess);}
            break;
          case 49: // rule Hyc.l:213: {comma} :
{ push(Token::Specifier::Comma);}
            break;
          case 50: // rule Hyc.l:214: {rparen_l} :
{ push(Token::Specifier::RParenL);}
            break;
          case 51: // rule Hyc.l:215: {rparen_r} :
{ push(Token::Specifier::RParenR);}
            break;
          case 52: // rule Hyc.l:216: {integer} :
{ push(Token::Specifier::Integer); }
            break;
          case 53: // rule Hyc.l:217: {float} :
{ push(Token::Specifier::Float); }
            break;
          case 54: // rule Hyc.l:218: {double} :
{ push(Token::Specifier::Double); }
            break;
          case 55: // rule Hyc.l:219: {plus} :
{ push(Token::Specifier::Plus); }
            break;
          case 56: // rule Hyc.l:220: {minus} :
{ push(Token::Specifier::Minus); }
            break;
          case 57: // rule Hyc.l:221: {asterix} :
{ push(Token::Specifier::Asterix); }
            break;
          case 58: // rule Hyc.l:222: {slash} :
{ push(Token::Specifier::Slash); }
            break;
          case 59: // rule Hyc.l:223: {decl_cpy} :
{ push(Token::Specifier::DeclCpy); }
            break;
          case 60: // rule Hyc.l:224: {decl_mv} :
{ push(Token::Specifier::DeclMv);  }
            break;
          case 61: // rule Hyc.l:225: {colon} :
{ push(Token::Specifier::Colon); }
            break;
          case 62: // rule Hyc.l:226: {equal} :
{ push(Token::Specifier::Equal); }
            break;
          case 63: // rule Hyc.l:227: {ident} :
{ push(Token::Specifier::Ident); }
            break;
          case 64: // rule Hyc.l:228: {semicln} :
{ push(Token::Specifier::Semicolon); }
            break;
          case 65: // rule Hyc.l:229: {brace_l} :
{ push(Token::Specifier::BraceL); }
            break;
          case 66: // rule Hyc.l:230: {brace_r} :
{ push(Token::Specifier::BraceR); }
            break;
          case 67: // rule Hyc.l:231: {bracket_r} :
{ push(Token::Specifier::BracketR); }
            break;
          case 68: // rule Hyc.l:232: {bracket_l} :
{ push(Token::Specifier::BracketL);}
            break;
          case 69: // rule Hyc.l:233: {less} :
{ push(Token::Specifier::Less); }
            break;
          case 70: // rule Hyc.l:234: {greater} :
{ push(Token::Specifier::Greater); }
            break;
        }
  }
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  TABLES                                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <reflex/matcher.h>

#if defined(OS_WIN)
#pragma warning(disable:4101 4102)
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-label"
#elif defined(__clang__)
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-label"
#endif

void reflex_code_INITIAL(reflex::Matcher& m)
{
  int c0 = 0, c1 = 0;
  m.FSM_INIT(c1);

S0:
  m.FSM_FIND();
  c1 = m.FSM_CHAR();
  if (c1 == '~') goto S155;
  if (c1 == '}') goto S220;
  if (c1 == '|') goto S164;
  if (c1 == '{') goto S218;
  if (c1 == 'w') goto S60;
  if (c1 == 'u') goto S110;
  if (c1 == 't') goto S103;
  if (c1 == 's') goto S81;
  if (c1 == 'r') goto S45;
  if (c1 == 'o') goto S131;
  if (c1 == 'n') goto S117;
  if (c1 == 'm') goto S124;
  if (c1 == 'i') goto S67;
  if (c1 == 'f') goto S52;
  if (c1 == 'e') goto S74;
  if (c1 == 'c') goto S88;
  if (c1 == 'a') goto S96;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if (c1 == '^') goto S179;
  if (c1 == ']') goto S222;
  if (c1 == '[') goto S224;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if (c1 == '?') goto S182;
  if (c1 == '>') goto S173;
  if (c1 == '=') goto S176;
  if (c1 == '<') goto S169;
  if (c1 == ';') goto S216;
  if (c1 == ':') goto S138;
  if ('1' <= c1 && c1 <= '9') goto S206;
  if (c1 == '0') goto S199;
  if (c1 == '/') goto S187;
  if (c1 == '.') goto S190;
  if (c1 == '-') goto S147;
  if (c1 == ',') goto S193;
  if (c1 == '+') goto S143;
  if (c1 == '*') goto S184;
  if (c1 == ')') goto S197;
  if (c1 == '(') goto S195;
  if (c1 == '\'') goto S210;
  if (c1 == '&') goto S157;
  if (c1 == '%') goto S161;
  if (c1 == '!') goto S152;
  if (c1 == ' ') goto S226;
  if ('\t' <= c1 && c1 <= '\r') goto S226;
  return m.FSM_HALT(c1);

S45:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'e') goto S230;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S52:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'p') goto S244;
  if (c1 == 'o') goto S237;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S60:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'h') goto S251;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S67:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'f') goto S258;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S74:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'l') goto S264;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S81:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 't') goto S272;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S88:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'o') goto S279;
  if (c1 == 'a') goto S286;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S96:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 's') goto S293;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S103:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'h') goto S299;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S110:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'n') goto S306;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S117:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'a') goto S313;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S124:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'a') goto S320;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S131:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'p') goto S327;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S138:
  m.FSM_TAKE(61);
  c1 = m.FSM_CHAR();
  if (c1 == '=') goto S336;
  if (c1 == ':') goto S334;
  if (c1 == '#') goto S338;
  return m.FSM_HALT(c1);

S143:
  m.FSM_TAKE(55);
  c1 = m.FSM_CHAR();
  if (c1 == '=') goto S342;
  if (c1 == '+') goto S340;
  return m.FSM_HALT(c1);

S147:
  m.FSM_TAKE(56);
  c1 = m.FSM_CHAR();
  if (c1 == '>') goto S348;
  if (c1 == '=') goto S346;
  if (c1 == '-') goto S344;
  return m.FSM_HALT(c1);

S152:
  m.FSM_TAKE(21);
  c1 = m.FSM_CHAR();
  if (c1 == '=') goto S350;
  return m.FSM_HALT(c1);

S155:
  m.FSM_TAKE(22);
  return m.FSM_HALT();

S157:
  m.FSM_TAKE(23);
  c1 = m.FSM_CHAR();
  if (c1 == '=') goto S354;
  if (c1 == '&') goto S352;
  return m.FSM_HALT(c1);

S161:
  m.FSM_TAKE(24);
  c1 = m.FSM_CHAR();
  if (c1 == '=') goto S356;
  return m.FSM_HALT(c1);

S164:
  m.FSM_TAKE(33);
  c1 = m.FSM_CHAR();
  if (c1 == '|') goto S361;
  if (c1 == '>') goto S358;
  if (c1 == '=') goto S363;
  return m.FSM_HALT(c1);

S169:
  m.FSM_TAKE(69);
  c1 = m.FSM_CHAR();
  if (c1 == '|') goto S365;
  if (c1 == '=') goto S368;
  return m.FSM_HALT(c1);

S173:
  m.FSM_TAKE(70);
  c1 = m.FSM_CHAR();
  if (c1 == '=') goto S371;
  return m.FSM_HALT(c1);

S176:
  m.FSM_TAKE(62);
  c1 = m.FSM_CHAR();
  if (c1 == '=') goto S373;
  return m.FSM_HALT(c1);

S179:
  m.FSM_TAKE(32);
  c1 = m.FSM_CHAR();
  if (c1 == '=') goto S375;
  return m.FSM_HALT(c1);

S182:
  m.FSM_TAKE(36);
  return m.FSM_HALT();

S184:
  m.FSM_TAKE(57);
  c1 = m.FSM_CHAR();
  if (c1 == '=') goto S377;
  return m.FSM_HALT(c1);

S187:
  m.FSM_TAKE(58);
  c1 = m.FSM_CHAR();
  if (c1 == '=') goto S379;
  return m.FSM_HALT(c1);

S190:
  m.FSM_TAKE(47);
  c1 = m.FSM_CHAR();
  if ('0' <= c1 && c1 <= '9') goto S381;
  return m.FSM_HALT(c1);

S193:
  m.FSM_TAKE(49);
  return m.FSM_HALT();

S195:
  m.FSM_TAKE(50);
  return m.FSM_HALT();

S197:
  m.FSM_TAKE(51);
  return m.FSM_HALT();

S199:
  m.FSM_TAKE(52);
  c1 = m.FSM_CHAR();
  if (c1 == 'x') goto S387;
  if (c1 == 'b') goto S394;
  if ('8' <= c1 && c1 <= '9') goto S403;
  if ('0' <= c1 && c1 <= '7') goto S398;
  if (c1 == '.') goto S396;
  return m.FSM_HALT(c1);

S206:
  m.FSM_TAKE(52);
  c1 = m.FSM_CHAR();
  if ('0' <= c1 && c1 <= '9') goto S206;
  if (c1 == '.') goto S396;
  return m.FSM_HALT(c1);

S210:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S216:
  m.FSM_TAKE(64);
  return m.FSM_HALT();

S218:
  m.FSM_TAKE(65);
  return m.FSM_HALT();

S220:
  m.FSM_TAKE(66);
  return m.FSM_HALT();

S222:
  m.FSM_TAKE(67);
  return m.FSM_HALT();

S224:
  m.FSM_TAKE(68);
  return m.FSM_HALT();

S226:
  m.FSM_TAKE(1);
  c1 = m.FSM_CHAR();
  if (c1 == ' ') goto S226;
  if ('\t' <= c1 && c1 <= '\r') goto S226;
  return m.FSM_HALT(c1);

S230:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 't') goto S406;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S237:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'r') goto S413;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S244:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 't') goto S419;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S251:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'i') goto S426;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S258:
  m.FSM_TAKE(5);
  c1 = m.FSM_CHAR();
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S264:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 's') goto S440;
  if (c1 == 'i') goto S433;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S272:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'r') goto S447;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S279:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'n') goto S454;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S286:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 's') goto S461;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S293:
  m.FSM_TAKE(11);
  c1 = m.FSM_CHAR();
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S299:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'r') goto S468;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S306:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'i') goto S475;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S313:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'm') goto S482;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S320:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 't') goto S489;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S327:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'e') goto S496;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S334:
  m.FSM_TAKE(18);
  return m.FSM_HALT();

S336:
  m.FSM_TAKE(59);
  return m.FSM_HALT();

S338:
  m.FSM_TAKE(60);
  return m.FSM_HALT();

S340:
  m.FSM_TAKE(19);
  return m.FSM_HALT();

S342:
  m.FSM_TAKE(37);
  return m.FSM_HALT();

S344:
  m.FSM_TAKE(20);
  return m.FSM_HALT();

S346:
  m.FSM_TAKE(38);
  return m.FSM_HALT();

S348:
  m.FSM_TAKE(48);
  return m.FSM_HALT();

S350:
  m.FSM_TAKE(31);
  return m.FSM_HALT();

S352:
  m.FSM_TAKE(34);
  return m.FSM_HALT();

S354:
  m.FSM_TAKE(44);
  return m.FSM_HALT();

S356:
  m.FSM_TAKE(41);
  return m.FSM_HALT();

S358:
  m.FSM_TAKE(25);
  c1 = m.FSM_CHAR();
  if (c1 == '=') goto S503;
  return m.FSM_HALT(c1);

S361:
  m.FSM_TAKE(35);
  return m.FSM_HALT();

S363:
  m.FSM_TAKE(46);
  return m.FSM_HALT();

S365:
  m.FSM_TAKE(26);
  c1 = m.FSM_CHAR();
  if (c1 == '=') goto S505;
  return m.FSM_HALT(c1);

S368:
  m.FSM_TAKE(28);
  c1 = m.FSM_CHAR();
  if (c1 == '>') goto S507;
  return m.FSM_HALT(c1);

S371:
  m.FSM_TAKE(29);
  return m.FSM_HALT();

S373:
  m.FSM_TAKE(30);
  return m.FSM_HALT();

S375:
  m.FSM_TAKE(45);
  return m.FSM_HALT();

S377:
  m.FSM_TAKE(39);
  return m.FSM_HALT();

S379:
  m.FSM_TAKE(40);
  return m.FSM_HALT();

S381:
  m.FSM_TAKE(54);
  c1 = m.FSM_CHAR();
  if (c1 == 'f') goto S509;
  if (c1 == 'e') goto S511;
  if (c1 == 'E') goto S511;
  if ('0' <= c1 && c1 <= '9') goto S381;
  return m.FSM_HALT(c1);

S387:
  c1 = m.FSM_CHAR();
  if (c1 == 'x') goto S514;
  if (c1 == 't') goto S514;
  if (c1 == 'i') goto S514;
  if (c1 == 'g') goto S514;
  if (c1 == 'd') goto S514;
  if (c1 == ':') goto S514;
  return m.FSM_HALT(c1);

S394:
  c1 = m.FSM_CHAR();
  if ('0' <= c1 && c1 <= '1') goto S522;
  return m.FSM_HALT(c1);

S396:
  c1 = m.FSM_CHAR();
  if ('0' <= c1 && c1 <= '9') goto S381;
  return m.FSM_HALT(c1);

S398:
  m.FSM_TAKE(52);
  c1 = m.FSM_CHAR();
  if ('8' <= c1 && c1 <= '9') goto S403;
  if ('0' <= c1 && c1 <= '7') goto S398;
  if (c1 == '.') goto S396;
  return m.FSM_HALT(c1);

S403:
  c1 = m.FSM_CHAR();
  if ('0' <= c1 && c1 <= '9') goto S403;
  if (c1 == '.') goto S396;
  return m.FSM_HALT(c1);

S406:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'u') goto S525;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S413:
  m.FSM_TAKE(3);
  c1 = m.FSM_CHAR();
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S419:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'r') goto S532;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S426:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'l') goto S538;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S433:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'f') goto S545;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S440:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'e') goto S551;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S447:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'u') goto S557;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S454:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 't') goto S564;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S461:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'e') goto S571;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S468:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'o') goto S577;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S475:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'o') goto S584;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S482:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'e') goto S591;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S489:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'c') goto S598;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S496:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'r') goto S605;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S503:
  m.FSM_TAKE(43);
  return m.FSM_HALT();

S505:
  m.FSM_TAKE(42);
  return m.FSM_HALT();

S507:
  m.FSM_TAKE(27);
  return m.FSM_HALT();

S509:
  m.FSM_TAKE(53);
  return m.FSM_HALT();

S511:
  c1 = m.FSM_CHAR();
  if (c1 == '-') goto S612;
  if (c1 == '+') goto S612;
  return m.FSM_HALT(c1);

S514:
  m.FSM_TAKE(52);
  c1 = m.FSM_CHAR();
  if (c1 == 'x') goto S514;
  if (c1 == 't') goto S514;
  if (c1 == 'i') goto S514;
  if (c1 == 'g') goto S514;
  if (c1 == 'd') goto S514;
  if (c1 == ':') goto S514;
  return m.FSM_HALT(c1);

S522:
  m.FSM_TAKE(52);
  c1 = m.FSM_CHAR();
  if ('0' <= c1 && c1 <= '1') goto S522;
  return m.FSM_HALT(c1);

S525:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'r') goto S614;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S532:
  m.FSM_TAKE(10);
  c1 = m.FSM_CHAR();
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S538:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'e') goto S621;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S545:
  m.FSM_TAKE(6);
  c1 = m.FSM_CHAR();
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S551:
  m.FSM_TAKE(7);
  c1 = m.FSM_CHAR();
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S557:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'c') goto S627;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S564:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'i') goto S634;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S571:
  m.FSM_TAKE(16);
  c1 = m.FSM_CHAR();
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S577:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'w') goto S641;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S584:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'n') goto S647;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S591:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 's') goto S653;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S598:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'h') goto S660;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S605:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'a') goto S666;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S612:
  c1 = m.FSM_CHAR();
  if ('0' <= c1 && c1 <= '9') goto S673;
  return m.FSM_HALT(c1);

S614:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'n') goto S677;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S621:
  m.FSM_TAKE(4);
  c1 = m.FSM_CHAR();
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S627:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 't') goto S683;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S634:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'n') goto S689;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S641:
  m.FSM_TAKE(12);
  c1 = m.FSM_CHAR();
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S647:
  m.FSM_TAKE(13);
  c1 = m.FSM_CHAR();
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S653:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'p') goto S696;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S660:
  m.FSM_TAKE(15);
  c1 = m.FSM_CHAR();
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S666:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 't') goto S703;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S673:
  m.FSM_TAKE(54);
  c1 = m.FSM_CHAR();
  if (c1 == 'f') goto S509;
  if ('0' <= c1 && c1 <= '9') goto S673;
  return m.FSM_HALT(c1);

S677:
  m.FSM_TAKE(2);
  c1 = m.FSM_CHAR();
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S683:
  m.FSM_TAKE(8);
  c1 = m.FSM_CHAR();
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S689:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'u') goto S710;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S696:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'a') goto S717;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S703:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'o') goto S724;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S710:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'e') goto S731;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S717:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'c') goto S737;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S724:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'r') goto S744;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S731:
  m.FSM_TAKE(9);
  c1 = m.FSM_CHAR();
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S737:
  m.FSM_TAKE(63);
  c1 = m.FSM_CHAR();
  if (c1 == 'e') goto S750;
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S744:
  m.FSM_TAKE(17);
  c1 = m.FSM_CHAR();
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);

S750:
  m.FSM_TAKE(14);
  c1 = m.FSM_CHAR();
  if ('_' <= c1 && c1 <= 'z') goto S210;
  if ('A' <= c1 && c1 <= 'Z') goto S210;
  if ('0' <= c1 && c1 <= '9') goto S210;
  if (c1 == '\'') goto S210;
  return m.FSM_HALT(c1);
}

