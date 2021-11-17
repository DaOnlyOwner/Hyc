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
	virtual void visit(struct FuncDefStmt& func_call_def_stmt) = 0;
	virtual void visit(struct ReturnStmt& ret_stmt) = 0;
	virtual void visit(struct ExprStmt& expr_stmt) = 0;
	virtual void visit(struct DeclStmt& def_stmt) = 0;
	virtual void visit(struct PointerTypeSpec& pt_spec) = 0;
	virtual void visit(struct BaseTypeSpec& bt_spec) = 0;
	virtual void visit(struct ArrayTypeSpec& at_spec) = 0;
	virtual void visit(struct ImplicitCastExpr& ice) = 0;
	virtual void visit(struct IfStmt& if_stmt) = 0;
	virtual void visit(struct WhileStmt& while_stmt) = 0;
	virtual void visit(struct ContractDefStmt& contract_def_stmt) = 0;
	virtual void visit(struct ContractImplStmt& contract_impl_stmt) = 0;
	virtual void visit(struct ForStmt& for_stmt) = 0;
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

struct IdentExpr : Expr
{
	IdentExpr(const Token& token)
		: ident(token){}
	Token ident;
	IMPL_VISITOR
};

struct FuncCallExpr : Expr
{
	FuncCallExpr(const Token& name, std::vector<uptr<Expr>> arg_list)
		: name(name), arg_list(std::move(arg_list)){}
	Token name;
	std::vector<uptr<Expr>> arg_list;

	// Semantic annotations:
	struct Function* sem_function = nullptr;


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
	std::vector<Token> needed_contracts;
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
	virtual std::string as_str() const override { return name.text + (inner ? inner->as_str() : std::string()); }
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
	StructDefStmt(Token&& name, std::vector<uptr<Stmt>>&& stmts)
		:name(mv(name)),stmts(mv(stmts)){}
	Token name;
	std::vector<uptr<Stmt>> stmts;
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

struct ContractDefStmt : Stmt
{
	ContractDefStmt(Token&& name, std::vector<Token>&& inherited_contracts, std::vector<uptr<Stmt>>&& stmts)
		:name(mv(name)),inherited_contracts(mv(inherited_contracts)),stmts(mv(stmts)){}
	Token name;
	std::vector<Token> inherited_contracts;
	std::vector<uptr<Stmt>> stmts;
	IMPL_VISITOR
};

struct ContractImplStmt : Stmt
{
	ContractImplStmt(Token&& contr_name, Token&& for_name, std::vector<uptr<Stmt>>&& func_defs)
		:contr_name(mv(contr_name)), for_name(mv(for_name)), func_defs(mv(func_defs)) {}
	Token contr_name;
	Token for_name;
	std::vector<uptr<Stmt>> func_defs;
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
	ForStmt(uptr<Stmt> decl_stmt, uptr<Expr> fst_expr, uptr<Expr> snd_expr)
		:decl_stmt(mv(decl_stmt)),fst_expr(mv(fst_expr)),snd_expr(mv(snd_expr)){}
	uptr<Stmt> decl_stmt;
	uptr<Expr> fst_expr;
	uptr<Expr> snd_expr;
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

struct FuncDefStmt : Stmt
{
	FuncDefStmt(uptr<TypeSpec> ret_type, Token&& name, std::vector<GenericInfo>&& generic_list, std::vector<std::pair<uptr<TypeSpec>, Token>>&& arg_list_type_ident, std::vector<uptr<Stmt>>&& body)
		: ret_type(mv(ret_type)), name(mv(name)), generic_list(mv(generic_list)), arg_list_type_ident(mv(arg_list_type_ident)), body(mv(body)){}
	uptr<TypeSpec> ret_type;
	Token name;
	std::vector<std::pair<uptr<TypeSpec>, Token>> arg_list_type_ident;
	std::vector<GenericInfo> generic_list;
	std::vector<uptr<Stmt>> body;

	// Semantic annotations:
	struct Function* sem_function = nullptr;

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











