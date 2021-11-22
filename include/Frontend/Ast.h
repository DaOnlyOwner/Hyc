#pragma once
#include <memory>
#include "Token.h"
#include <vector>
#include <algorithm>
#include <utility>
#include "Primitive.h"
#include <cerrno>
#include "DebugPrint.h"
#include "fmt/core.h"
#include <stdexcept>
#include "AtomicType.h"

#define IMPL_VISITOR virtual void accept(IAstVisitor& visitor) override {visitor.visit(*this);}

template <typename TRet, typename TArg>
inline std::unique_ptr<TRet> ast_as(std::unique_ptr<TArg> arg)
{
	return static_cast<std::unique_ptr<TRet>>(std::move(arg));
}

template<typename T>
using uptr = std::unique_ptr<T>;

// Couldn't get std::move alias to work, so now here is a macro
#define mv(arg) std::move(arg)

struct EvalIntegerResult
{
	Primitive::Specifier Spec;
	uint64_t amount_uint;
	int64_t amount_int;
};


struct IAstVisitor
{
	virtual void visit(struct FloatLiteralExpr& lit) = 0;
	virtual void visit(struct IntegerLiteralExpr& lit) = 0;
	virtual void visit(struct BinOpExpr& bin_op) = 0;
	virtual void visit(struct PrefixOpExpr& pre_op) = 0;
	virtual void visit(struct PostfixOpExpr& post_op) = 0;
	virtual void visit(struct DeclCpyStmt& decl_cpy) = 0;
	virtual void visit(struct DeclMvStmt& decl_mv) = 0;
	virtual void visit(struct DeclInitStmt& decl_init) = 0;
	virtual void visit(struct IdentExpr& ident) = 0;
	virtual void visit(struct NamespaceStmt& namespace_stmt) = 0;
	virtual void visit(struct FuncCallExpr& func_call_expr) = 0;
	virtual void visit(struct FuncDeclStmt& func_decl) = 0;
	virtual void visit(struct FuncDefStmt& func_def_stmt) = 0;
	virtual void visit(struct StructDefStmt& struct_def_stmt) = 0;
	virtual void visit(struct ReturnStmt& ret_stmt) = 0;
	virtual void visit(struct ExprStmt& expr_stmt) = 0;
	virtual void visit(struct DeclStmt& decl_stmt) = 0;
	virtual void visit(struct PointerTypeSpec& pt_spec) = 0;
	virtual void visit(struct BaseTypeSpec& bt_spec) = 0;
	virtual void visit(struct ArrayTypeSpec& at_spec) = 0;
	virtual void visit(struct ImplicitCastExpr& ice) = 0;
	virtual void visit(struct IfStmt& if_stmt) = 0;
	virtual void visit(struct WhileStmt& while_stmt) = 0;
	virtual void visit(struct ForStmt& for_stmt) = 0;
	virtual void visit(struct ContinueStmt& cont_stmt) = 0;
	virtual void visit(struct FptrTypeSpec& fptr) = 0;
	virtual void visit(struct ArraySubscriptExpr& subs) = 0;
	virtual void visit(struct TernaryExpr& tern) = 0;
	virtual void visit(struct UnionDefStmt& union_def) = 0;
	virtual void visit(struct MatchStmt& match) = 0;
};

struct Node
{
	virtual void accept(IAstVisitor& visitor) = 0;
	virtual ~Node() = default;
};



// Expressions

struct Expr : Node
{
	// Semantic annotations:
	Type sem_type;
};

struct BinOpExpr : Expr
{
	BinOpExpr(const Token& op, uptr<Expr> lh, uptr<Expr> rh)
		:op(op),lh(mv(lh)),rh(mv(rh)){}
	Token op;
	uptr<Expr> lh, rh;

	// Semantic annotations
	struct Function* sem_bin_op = nullptr;

	IMPL_VISITOR
};

struct PrefixOpExpr : Expr
{
	PrefixOpExpr(const Token& op, uptr<Expr> lh)
		:op(op),lh(mv(lh)){}
	Token op;
	uptr<Expr> lh;

	// Semantic annotations
	struct Function* sem_unary_op = nullptr;

	IMPL_VISITOR
};

struct PostfixOpExpr : Expr
{
	PostfixOpExpr(const Token& op, uptr<Expr> rh)
		:op(op), rh(mv(rh)) {}
	Token op;
	uptr<Expr> rh;
	IMPL_VISITOR
};

struct IntegerLiteralExpr : Expr
{
	IntegerLiteralExpr(const Token& token, const EvalIntegerResult& res) 
		: integer_literal(token),eval_res(res)
	{
	
	}
	Token integer_literal;
	EvalIntegerResult eval_res;
	IMPL_VISITOR
};

struct FloatLiteralExpr : Expr
{
	FloatLiteralExpr(const Token& token)
		: float_literal(token){}
	Token float_literal;
	IMPL_VISITOR
};

struct TypeSpec;
struct IdentExpr : Expr
{
	IdentExpr(const Token& token)
		: ident(token) {}
	Token ident;
	IMPL_VISITOR
};

struct FuncCallExpr : Expr
{
	FuncCallExpr(uptr<Expr>& from, std::vector<uptr<Expr>>&& arg_list, std::vector<uptr<TypeSpec>>&& generic_params)
		: from(mv(from)), arg_list(mv(arg_list)),generic_params(mv(generic_params)) {}
	uptr<Expr> from;
	std::vector<uptr<Expr>> arg_list;
	std::vector<uptr<TypeSpec>> generic_params;
	// Semantic annotations:
	struct Function* sem_function = nullptr;


	IMPL_VISITOR
};

struct ArraySubscriptExpr : Expr
{
	ArraySubscriptExpr(uptr<Expr>&& from, uptr<Expr>&& inner)
		:from(mv(from)), inner(mv(inner)){}
		uptr<Expr> from, inner;
	IMPL_VISITOR
};

struct TernaryExpr : Expr
{
	TernaryExpr(uptr<Expr>&& fst, uptr<Expr>&& snd, uptr<Expr>&& trd)
		:fst(mv(fst)), snd(mv(snd)), trd(mv(trd)) {}
	uptr<Expr> fst, snd, trd;
	IMPL_VISITOR
};


struct ImplicitCastExpr : Expr
{
	ImplicitCastExpr(uptr<Expr> expr, Type&& t)
		:expr(mv(expr))
	{
		sem_type = mv(t);
	}
	uptr<Expr> expr;

	IMPL_VISITOR
};


// Types

struct GenericInfo
{
	Token name;
	uptr<struct TypeSpec> default_type;
};

struct TypeSpec : Node
{
	virtual const Token& get_ident_token() const { throw std::runtime_error("Compiler bug, cannot get token"); };
	virtual std::string as_str() const = 0;
};

struct PointerTypeSpec : TypeSpec
{
	PointerTypeSpec(uptr<TypeSpec> inner)
		:inner(mv(inner)) {}

	uptr<TypeSpec> inner = nullptr;
	virtual std::string as_str() const override { return "*" + (inner ? inner->as_str() : std::string()); }
	IMPL_VISITOR
};

struct BaseTypeSpec : TypeSpec
{
	BaseTypeSpec(const Token& name, uptr<TypeSpec> inner, std::vector<uptr<TypeSpec>>&& generic_list)
		:name(name), inner(mv(inner)),generic_list(mv(generic_list)) {}
	Token name;
	uptr<TypeSpec> inner;
	std::vector<uptr<TypeSpec>> generic_list;
	virtual const Token& get_ident_token() const override { return name; }
	virtual std::string as_str() const override { 
		
		std::string gl = "<";
		if (!generic_list.empty())
		{
			gl += generic_list[0]->as_str();
		}

		for (int i = 1; i<generic_list.size(); i++)
		{
			auto& ts = generic_list[i];
			gl += "," + ts->as_str();
		}
		gl += ">";
		return fmt::format("{}{}{}",name.text,generic_list.empty() ? "" : gl, inner ? inner->as_str() : std::string()); 
	}
	IMPL_VISITOR
};

struct FptrTypeSpec : TypeSpec
{
	FptrTypeSpec(std::vector<uptr<TypeSpec>>&& args, uptr<TypeSpec>&& ret_type)
		:args(mv(args)), ret_type(mv(ret_type)) {}
	std::vector<uptr<TypeSpec>> args;
	uptr<TypeSpec> ret_type;
	virtual std::string as_str() const override {
		std::string args_str = "";
		if (!args.empty())
		{
			args_str += args[0]->as_str();
		}
		for (int i = 1; i < args.size(); i++)
		{
			auto& arg = args[i];
			args_str += "," + arg->as_str();
		}
		return fmt::format("fptr({};{})", args_str, ret_type->as_str());
	}

	IMPL_VISITOR

};

struct ArrayTypeSpec : TypeSpec
{
	ArrayTypeSpec(uptr<struct IntegerLiteralExpr> amount, uptr<TypeSpec> inner)
		:amount(mv(amount)), inner(mv(inner)) {}
	uptr<IntegerLiteralExpr> amount;
	uptr<TypeSpec> inner;
	virtual std::string as_str() const override { return fmt::format("[{}]", amount->integer_literal.text) + (inner ? inner->as_str() : std::string()); }
	IMPL_VISITOR
};


// Statements

struct Stmt : Node {};

struct TypedStmt : Stmt
{
	Type type;
};

struct DeclOpStmt : TypedStmt
{
	DeclOpStmt(uptr<TypeSpec> type, Token&& name, uptr<Expr> expr)
		: type(mv(type)), name(mv(name)), expr(mv(expr)) {}
	Token name;
	uptr<TypeSpec> type;
	uptr<Expr> expr;
};

struct DeclCpyStmt : DeclOpStmt
{
	DeclCpyStmt(uptr<TypeSpec> type, Token&& name, uptr<Expr> expr)
		: DeclOpStmt(mv(type),mv(name),mv(expr)) {}
	IMPL_VISITOR
};

struct DeclMvStmt : DeclOpStmt
{
	DeclMvStmt(uptr<TypeSpec> type, Token&& name, uptr<Expr> expr)
		: DeclOpStmt(mv(type), mv(name), mv(expr)) {}
	IMPL_VISITOR
};

struct DeclInitStmt : DeclOpStmt
{
	DeclInitStmt(uptr<TypeSpec> type, Token&& name, uptr<Expr> expr)
		: DeclOpStmt(mv(type), mv(name), mv(expr)) {}
	IMPL_VISITOR
};

struct ContinueStmt : Stmt
{
	IMPL_VISITOR
};

struct DeclStmt : TypedStmt
{
	DeclStmt(uptr<TypeSpec> type, Token&& name)
		:type_spec(mv(type)),name(mv(name)){}

	Token name;
	uptr<TypeSpec> type_spec;
	IMPL_VISITOR
};

struct StructDefStmt : Stmt
{
	StructDefStmt(Token&& name, std::vector<GenericInfo>&& generic_params, std::vector<uptr<Stmt>>&& stmts)
		:name(mv(name)),generic_params(mv(generic_params)), stmts(mv(stmts)){}
	Token name;
	std::vector<uptr<Stmt>> stmts;
	std::vector<GenericInfo> generic_params;
	IMPL_VISITOR
};

struct UnionDefStmt : Stmt
{
	UnionDefStmt(Token&& name, std::vector<GenericInfo>&& generic_params, std::vector<uptr<Stmt>>&& stmts)
		:name(mv(name)), generic_params(mv(generic_params)), stmts(mv(stmts)) {}
	Token name;
	std::vector<uptr<Stmt>> stmts;
	std::vector<GenericInfo> generic_params;
	IMPL_VISITOR
};

struct NamespaceStmt : Stmt
{
	NamespaceStmt(std::vector<uptr<Stmt>>&& stmts, Token&& name)
		:stmts(mv(stmts)),name(mv(name)) {}
	NamespaceStmt(const Token& name)
		:name(name){}
	std::vector<uptr<Stmt>> stmts;
	Token name;
	IMPL_VISITOR
};

struct WhileStmt : Stmt
{
	WhileStmt(uptr<Expr>&& expr, std::vector<uptr<Stmt>>&& stmts)
		:expr(mv(expr)),stmts(mv(stmts)){}
	uptr<Expr> expr;
	std::vector<uptr<Stmt>> stmts;
	IMPL_VISITOR
};

struct ForStmt : Stmt
{
	ForStmt(uptr<Stmt> decl_stmt, uptr<Expr> fst_expr, uptr<Expr> snd_expr, std::vector<uptr<Stmt>>&& stmts)
		:decl_stmt(mv(decl_stmt)),fst_expr(mv(fst_expr)),snd_expr(mv(snd_expr)),stmts(mv(stmts)){}
	uptr<Stmt> decl_stmt;
	uptr<Expr> fst_expr;
	uptr<Expr> snd_expr;
	std::vector<uptr<Stmt>> stmts;
	IMPL_VISITOR
};

struct IfStmt : Stmt
{
	IfStmt(uptr<Expr>&& if_expr,
	std::vector<uptr<Stmt>>&& if_stmts,
	std::vector<uptr<Expr>>&& elif_exprs,
	std::vector<std::vector<uptr<Stmt>>>&& all_elif_stmts,
	std::vector<uptr<Stmt>>&& else_stmts) 
		:if_expr(mv(if_expr)),if_stmts(mv(if_stmts)),elif_exprs(mv(elif_exprs)),all_elif_stmts(mv(all_elif_stmts)),else_stmts(mv(else_stmts))
	{}
	uptr<Expr> if_expr;
	std::vector<uptr<Stmt>> if_stmts;
	std::vector<uptr<Expr>> elif_exprs;
	std::vector<std::vector<uptr<Stmt>>> all_elif_stmts;
	std::vector<uptr<Stmt>> else_stmts;
	IMPL_VISITOR
};

struct FuncDeclStmt : Stmt
{
	FuncDeclStmt(uptr<TypeSpec> ret_type, Token&& name, std::vector<GenericInfo>&& generic_list, std::vector<std::pair<uptr<TypeSpec>, Token>>&& arg_list_type_ident)
		: ret_type(mv(ret_type)), name(mv(name)), arg_list_type_ident(mv(arg_list_type_ident)), generic_list(mv(generic_list)) {}
	uptr<TypeSpec> ret_type;
	Token name;
	std::vector<std::pair<uptr<TypeSpec>, Token>> arg_list_type_ident;
	std::vector<GenericInfo> generic_list;
	std::vector<uptr<FuncDeclStmt>> needed_funcs;
	IMPL_VISITOR
};

struct FuncDefStmt : Stmt
{
	FuncDefStmt(uptr<FuncDeclStmt>&& func_decl, std::vector<uptr<Stmt>>&& body)
		: decl(mv(func_decl)), body(mv(body)) {}
	uptr<FuncDeclStmt> decl;
	std::vector<uptr<Stmt>> body;

	// Semantic annotations:
	struct Function* sem_function = nullptr;

	IMPL_VISITOR
};

struct MatchCase 
{
	MatchCase(uptr<DeclStmt>&& decl_stmt, std::vector<uptr<Stmt>>&& body)
		:decl_stmt(mv(decl_stmt)),body(mv(body)){}
	uptr<DeclStmt> decl_stmt;
	std::vector<uptr<Stmt>> body;
};

struct MatchStmt : Stmt
{
	MatchStmt(std::vector<MatchCase>&& match_cases, uptr<Expr>&& match_on)
		:match_cases(mv(match_cases)),match_on(mv(match_on)){}
	std::vector<MatchCase> match_cases;
	uptr<Expr> match_on;
	IMPL_VISITOR
};


struct ExprStmt : Stmt
{
	ExprStmt(uptr<Expr>&& expr)
		: expr(mv(expr)){}
	uptr<Expr> expr;

	IMPL_VISITOR
};

struct ReturnStmt : Stmt
{
	ReturnStmt(uptr<Expr>&& returned_expr, const Token& return_kw)
		: returned_expr(mv(returned_expr)),return_kw(return_kw){}
	uptr<Expr> returned_expr;
	Token return_kw;
	IMPL_VISITOR
};











