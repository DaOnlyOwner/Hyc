#pragma once
#include <Token.h>
#include <memory>
#include <unordered_map>
#include <functional>
#include "Lexer.h"


/*
This file provides and implementation of a (probably mutated) "pratt" parser that can be extended at runtime
*/

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
	GenericPrattParser(Lexer& token_source)
		: m_token_source(token_source){}
	bool add_operation(Token::Type ttype, const InfixOperation<TReturn>& left)
	{
		return m_infix_operation.emplace(ttype,left).second;
	}

	bool add_operation(Token::Type ttype, const PrefixOperation<TReturn>& prefix)
	{
		return m_prefix_operation.emplace(ttype, prefix).second;
	}

	std::unique_ptr<TReturn> parse_internal(int precedence)
	{
		const Token& prefix_token = m_token_source.eat();
		// Let's parse a prefix
		auto prefix_it = m_prefix_operation.find(prefix_token.type);
		if (prefix_it == m_prefix_operation.end()) { printf("Cannot parse this"); assert(false); }
		PrefixOperation<TReturn>& prefix = prefix_it->second;
		auto lh = prefix.operation(*this, prefix_token);
		const Token* infix_token = &m_token_source.lookahead(1); // To reassign infix_token, use ptr.
		auto infix_it = m_infix_operation.find(infix_token->type);
		int infix_precedence = infix_it == m_infix_operation.end() 
			? -1 
			// Trick so that precedence < infix_precedence when operator is right assoc to stop while loop (right_assoc = 1 or = 0)
			: infix_it->second.precedence + infix_it->second.right_assoc; 
		while (precedence <= infix_precedence)
		{
			m_token_source.eat();
			lh = infix_it->second.operation(*this, *infix_token, std::move(lh));
			infix_token = &m_token_source.lookahead(1);
			infix_it = m_infix_operation.find(infix_token->type);
			infix_precedence = infix_it == m_infix_operation.end() 
				? -1 
				: infix_it->second.precedence + infix_it->second.right_assoc;
		}

		// TODO: Completness: Parse postfix, ternary operators


		return std::move(lh);
	}

	std::unique_ptr<TReturn> parse()
	{
		return parse_internal(0);
	}


	   
private:
	Lexer& m_token_source;
	std::unordered_map<Token::Type, InfixOperation<TReturn>> m_infix_operation;
	std::unordered_map<Token::Type, PrefixOperation<TReturn>> m_prefix_operation;
};