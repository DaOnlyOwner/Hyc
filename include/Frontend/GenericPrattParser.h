#pragma once
#include <Token.h>
#include <memory>
#include <unordered_map>
#include <functional>
#include "Lexer.h"
#include "DebugPrint.h"

/*
This file provides and implementation of a (probably mutated) "pratt" parser that can be extended at runtime
*/

#define InfixExprFnArgs ExprParser& parser, const Token& token, std::unique_ptr<Expr> lh
#define PrefixExprFnArgs ExprParser& parser, const Token& token

class Parser;

template<typename TReturn>
class GenericPrattParser;

template<typename TReturn>
using OperationFnPrefix = std::function< std::unique_ptr<TReturn>(GenericPrattParser<TReturn>&, const Token&) >;

template<typename TReturn>
using OperationFnInfix = std::function< std::unique_ptr<TReturn>(GenericPrattParser<TReturn>&, const Token&, std::unique_ptr<TReturn>) >;

template<typename TReturn>
struct InfixOperation
{
	int precedence;
	bool right_assoc;
	OperationFnInfix<TReturn> operation;
};

template<typename TReturn>
struct PrefixOperation
{
	int precedence;
	OperationFnPrefix<TReturn> operation;
};

template<typename TReturn>
class GenericPrattParser
{
public:
	GenericPrattParser(Parser* overall_parser, Lexer& tkns)
		: overall_parser(overall_parser),tkns(tkns){}
	bool add_operation(Token::Specifier ttype, const InfixOperation<TReturn>& left)
	{
		return m_infix_operation.emplace(ttype,left).second;
	}

	bool add_operation(Token::Specifier ttype, const PrefixOperation<TReturn>& prefix)
	{
		return m_prefix_operation.emplace(ttype, prefix).second;
	}

	std::unique_ptr<TReturn> parse_internal(int precedence)
	{
		const Token& prefix_token = tkns.eat();
		// Let's parse a prefix
		auto prefix_it = m_prefix_operation.find(prefix_token.type);
		if (prefix_it == m_prefix_operation.end()) 
		{
			auto descr = Error::FromToken(prefix_token);
			descr.Message = fmt::format("Expected an expression, but got '{}'", Token::Translate(prefix_token.type));
			descr.Hint = fmt::format("The offending token is '{}'", prefix_token.text);
			Error::SyntacticalError(descr);
		}
		PrefixOperation<TReturn>& prefix = prefix_it->second;
		auto lh = prefix.operation(*this, prefix_token);
		const Token* infix_token = &tkns.lookahead(1); // To reassign infix_token, use ptr.
		auto infix_it = m_infix_operation.find(infix_token->type);
		int infix_precedence = infix_it == m_infix_operation.end() 
			? 0 
			// Trick so that precedence < infix_precedence when operator is right assoc to stop while loop (right_assoc = 1 or = 0)
			: infix_it->second.precedence + infix_it->second.right_assoc; 
		while (precedence < infix_precedence)
		{
			tkns.eat();
			lh = infix_it->second.operation(*this, *infix_token, std::move(lh));
			infix_token = &tkns.lookahead(1);
			infix_it = m_infix_operation.find(infix_token->type);
			infix_precedence = infix_it == m_infix_operation.end() 
				? 0 
				: infix_it->second.precedence + infix_it->second.right_assoc;
		}

		return std::move(lh);
	}

	std::unique_ptr<TReturn> parse()
	{
		return parse_internal(0);
	}

	Parser* overall_parser;
private:
	Lexer& tkns;
	std::unordered_map<Token::Specifier, InfixOperation<TReturn>> m_infix_operation;
	std::unordered_map<Token::Specifier, PrefixOperation<TReturn>> m_prefix_operation;
};