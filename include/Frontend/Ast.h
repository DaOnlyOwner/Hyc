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
#include <cassert>
#include <variant>
#include "llvm/IR/DerivedTypes.h"
#include "UptrCast.h"

#define IMPL_VISITOR virtual void accept(IAstVisitor& visitor) override {visitor.visit(*this);}
#define IMPL_CLONE(u) uptr<u> clone() const override 
#define IMPL_ASSTR virtual std::string as_str() const override
#define CPY_VEC(name_from,name_to, type) std::vector<type> name_to; for(auto& p : name_from) name_to.push_back(p->clone());

template <typename TRet, typename TArg>
inline std::unique_ptr<TRet> ast_as(std::unique_ptr<TArg> arg)
{
	return static_cast<std::unique_ptr<TRet>>(std::move(arg));
}

template<typename T>
using uptr = std::unique_ptr<T>;

// Couldn't get std::move alias to work, so now here is a macro
#define mv(arg) std::move(arg)



struct IAstVisitor
{
	virtual void visit(struct DecimalLiteralExpr& lit) {};
	virtual void visit(struct IntegerLiteralExpr& lit) {};
	virtual void visit(struct BinOpExpr& bin_op) {};
	virtual void visit(struct PrefixOpExpr& pre_op) {};
	virtual void visit(struct PostfixOpExpr& post_op) {};
	virtual void visit(struct DeclCpyStmt& decl_cpy) {};
	virtual void visit(struct DeclMvStmt& decl_mv) { };
	virtual void visit(struct DeclInitStmt& decl_init) {};
	virtual void visit(struct IdentExpr& name) {};
	virtual void visit(struct NamespaceStmt& namespace_stmt); 
	virtual void visit(struct FuncCallExpr& func_call_expr) {};
	virtual void visit(struct FuncDeclStmt& func_decl) {};
	virtual void visit(struct FuncDefStmt& func_def_stmt);
	virtual void visit(struct TypeDefStmt& struct_def) {};
	virtual void visit(struct UnionDefStmt& union_def) {};
	virtual void visit(struct ReturnStmt& ret_stmt) {};
	virtual void visit(struct ExprStmt& expr_stmt);
	virtual void visit(struct DeclStmt& decl_stmt) {};
	virtual void visit(struct UnionDeclStmt& udecl_stmt) {};
	virtual void visit(struct PointerTypeSpec& pt_spec) {};
	virtual void visit(struct BaseTypeSpec& bt_spec) {};
	virtual void visit(struct ArrayTypeSpec& at_spec) {};
	virtual void visit(struct FptrTypeSpec& fptr) {};
	virtual void visit(struct ScopeTypeSpec& scope_spec) {};
	virtual void visit(struct ImplicitCastExpr& ice) {};
	virtual void visit(struct IfStmt& if_stmt);
	virtual void visit(struct WhileStmt& while_stmt);
	virtual void visit(struct ForStmt& for_stmt);
	virtual void visit(struct ContinueStmt& cont_stmt) {};
	virtual void visit(struct ArraySubscriptExpr& subs) {};
	virtual void visit(struct TernaryExpr& tern) {};
	virtual void visit(struct MatchStmt& match);
	virtual void visit(struct ScopeStmt& sc);
	virtual void visit(struct FptrIdentExpr& fptr){};
	virtual void visit(struct DelOpExpr& del) {};
	virtual void visit(struct MemOpExpr& mem) {}
	virtual void visit(struct InitListExpr& i) {}
	virtual void visit(struct OffsetofExpr& ooe){}
	virtual void visit(struct SizeOrAlignmentInfoExpr& soaie){};
	virtual void visit(struct SizeBetweenMemberInfoExpr& e){}
	virtual void visit(struct InitListArrayExpr& a) {}
	virtual void visit(struct InitListStructExpr& s) {}
};

struct Node
{
	virtual void accept(IAstVisitor& visitor) = 0;
	virtual ~Node() = default;

};

// Expressions

struct EvalIntegerResult
{
	int amount_bits;
	uint64_t val;
	bool is_signed;
};

// TODO: Also copy the semantic types and all the other stop in the clone method.
struct Expr : Node
{
	// Semantic annotations:
	Type sem_type;
	virtual	uptr<Expr> clone() const = 0;
	virtual std::string as_str() const = 0;
	virtual const Token& first_token() const = 0;
	virtual bool is_sret_func_call() const { return false; }
	virtual bool is_func_call() const { return false; }
};

#define IMPL_FT virtual const Token& first_token() const override

enum class IntegerLiteralType
{
	UInt=0,UHalf,UShort,UChar,Int,Half,Short,Char,Count
};

struct IntegerLiteralExpr : Expr
{
	IntegerLiteralExpr(const Token& token, const EvalIntegerResult& res, IntegerLiteralType ilt)
		: integer_literal(token), eval_res(res),type(ilt)
	{

	}

	IntegerLiteralExpr()
		:integer_literal(Token::Specifier::Int,"0"), eval_res{64,0,true}, type(IntegerLiteralType::Int){}

	Token integer_literal;
	EvalIntegerResult eval_res;
	IntegerLiteralType type;
	IMPL_VISITOR;
	IMPL_CLONE(Expr) { return uptr<Expr>(new IntegerLiteralExpr(*this)); }
	IMPL_ASSTR
	{
		return integer_literal.text;
	}
		IMPL_FT
	{
		return integer_literal;
	}
};


// Types

struct GenericInfo
{
	Token name;
	uptr<struct TypeSpec> default_type;
};

struct TypeSpec : Node
{
	TypeSpec(uptr<TypeSpec> inner)
		:inner(mv(inner)){}
	virtual const Token& get_ident_token() const { throw std::runtime_error("Compiler bug, cannot get token"); };
	virtual std::string as_str() const = 0;
	void push_inner(uptr<TypeSpec>&& elems) { if (inner) inner->push_inner(mv(elems)); else inner = mv(elems); }
	uptr<TypeSpec> inner = nullptr;
	Type semantic_type;
	virtual uptr<TypeSpec> clone() const = 0;
};

struct PointerTypeSpec : TypeSpec
{
	PointerTypeSpec(uptr<TypeSpec> inner)
		:TypeSpec(mv(inner)) {}
	virtual std::string as_str() const override { return "*" + (inner ? inner->as_str() : std::string()); }
	IMPL_VISITOR;
	IMPL_CLONE(TypeSpec) { return uptr<TypeSpec>(new PointerTypeSpec(inner?inner->clone():nullptr)); }

};

struct BaseTypeSpec : TypeSpec
{
	BaseTypeSpec(const Token& name, uptr<TypeSpec> inner, std::vector<uptr<TypeSpec>>&& generic_list)
		:name(name), TypeSpec(mv(inner)), generic_list(mv(generic_list)) {}
	BaseTypeSpec(const Token& name)
		:name(name), generic_list{},TypeSpec(nullptr){}
	Token name;
	std::vector<uptr<TypeSpec>> generic_list;
	virtual const Token& get_ident_token() const override { return name; }
	virtual std::string as_str() const override {

		std::string gl = "<";
		if (!generic_list.empty())
		{
			gl += generic_list[0]->as_str();
		}

		for (int i = 1; i < generic_list.size(); i++)
		{
			auto& ts = generic_list[i];
			gl += "," + ts->as_str();
		}
		gl += ">";
		return fmt::format("{}{}{}", name.text, generic_list.empty() ? "" : gl, inner ? inner->as_str() : std::string());
	}
	IMPL_VISITOR;
	IMPL_CLONE(TypeSpec) {
		CPY_VEC(generic_list,generic_list_cpy,uptr<TypeSpec>);
		auto ts = uptr<TypeSpec>(new BaseTypeSpec(Token(name),inner ? inner->clone() : nullptr,mv(generic_list_cpy))); 
		ts->semantic_type = semantic_type;
		return mv(ts);
	}
};

struct ScopeTypeSpec : TypeSpec
{
	ScopeTypeSpec(uptr<TypeSpec>&& inner, uptr<BaseTypeSpec>&& base)
		:TypeSpec(mv(inner)), base(mv(base)) {}
	uptr<BaseTypeSpec> base;

	virtual std::string as_str() const override {

		return fmt::format("{}::{}", base->as_str(), inner ? inner->as_str() : "");
	}

	virtual const Token& get_ident_token() const override
	{
		return inner->get_ident_token();
	}

	IMPL_VISITOR;
	IMPL_CLONE(TypeSpec) { assert(false); return nullptr; }
};

struct FptrTypeSpec : TypeSpec
{
	FptrTypeSpec(std::vector<uptr<TypeSpec>>&& args, uptr<TypeSpec>&& ret_type, uptr<TypeSpec>&& inner)
		:args(mv(args)), ret_type(mv(ret_type)),TypeSpec(mv(inner)){}
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

	IMPL_VISITOR;
	IMPL_CLONE(TypeSpec) {
		std::vector<uptr<TypeSpec>> args_cpy;
	for (auto& p : args) args_cpy.push_back(p->clone());
		return uptr<TypeSpec>(new FptrTypeSpec(mv(args_cpy),ret_type->clone(),inner->clone()));
	}

};

struct ArrayTypeSpec : TypeSpec
{
	ArrayTypeSpec(uptr<struct IntegerLiteralExpr> amount, uptr<TypeSpec> inner)
		:amount(mv(amount)), TypeSpec(mv(inner)) {}
	uptr<IntegerLiteralExpr> amount;
	virtual std::string as_str() const override { return fmt::format("[{}]", amount->integer_literal.text) + (inner ? inner->as_str() : std::string()); }
	IMPL_VISITOR;
	IMPL_CLONE(TypeSpec) { return uptr<TypeSpec>(new ArrayTypeSpec(uptr<IntegerLiteralExpr>(dynamic_cast<IntegerLiteralExpr*>(amount->clone().release())), inner?inner->clone():nullptr)); }
};


struct BinOpExpr : Expr
{
	BinOpExpr(const Token& op, uptr<Expr> lh, uptr<Expr> rh)
		:op(op),lh(mv(lh)),rh(mv(rh)){}
	Token op;
	uptr<Expr> lh, rh;

	// Semantic annotations
	FuncDefStmt* sem_bin_op = nullptr;

	IMPL_VISITOR;

	IMPL_CLONE(Expr)
	{
		return uptr<Expr>(new BinOpExpr(op,lh->clone(),rh->clone()));
	}

	IMPL_ASSTR
	{
		return fmt::format("{}{}{}",lh->as_str(),op.text,rh->as_str());
	}
		IMPL_FT
	{
		return op;
	}

};

struct PrefixOpExpr : Expr
{
	PrefixOpExpr(const Token& op, uptr<Expr> lh)
		:op(op),lh(mv(lh)){}
	Token op;
	uptr<Expr> lh;

	// Semantic annotations
	FuncDeclStmt* sem_unary_op = nullptr;

	IMPL_VISITOR;
	IMPL_CLONE(Expr)
	{
		auto a = new PrefixOpExpr(op, lh->clone()); 
		a->lh->sem_type = lh->sem_type;
		a->sem_type = sem_type;
		return uptr<Expr>(a);
	}
	IMPL_ASSTR
	{
		return fmt::format("{}{}",op.text,lh->as_str());
	}
		IMPL_FT
	{
		return op;
	}
};

struct PostfixOpExpr : Expr
{
	PostfixOpExpr(const Token& op, uptr<Expr> rh)
		:op(op), rh(mv(rh)) {}
	Token op;
	uptr<Expr> rh;
	IMPL_VISITOR;
	IMPL_CLONE(Expr) { return uptr<Expr>(new PostfixOpExpr(op, rh->clone())); }
	IMPL_ASSTR
	{
		return fmt::format("{}{}",rh->as_str(),op.text);
	}
		IMPL_FT
	{
		return op;
	}
};

struct DelOpExpr : Expr
{
	DelOpExpr(uptr<Expr>&& expr)
		:expr(mv(expr)) {}
	uptr<Expr> expr;
	FuncDefStmt* operation = nullptr;
	IMPL_VISITOR;
	IMPL_CLONE(Expr) { return uptr<Expr>(new DelOpExpr(expr->clone())); }
	IMPL_ASSTR
	{
		return fmt::format("del {}",expr->as_str());
	}
		IMPL_FT
	{
		return expr->first_token();
	}

};

struct OffsetofExpr : Expr
{
	OffsetofExpr(const Token& offsetof_kw, uptr<TypeSpec>&& of, const Token& member)
		:offsetof_kw(offsetof_kw),of(mv(of)),member(member){}
	uptr<TypeSpec> of;
	Type of_type;
	Token offsetof_kw;
	Token member;
	IMPL_VISITOR;
	IMPL_CLONE(Expr){auto expr = (new OffsetofExpr(offsetof_kw,of->clone(),member));
			expr->of_type = of_type;
			return uptr<Expr>(expr);
		}
	IMPL_ASSTR
	{
		return fmt::format("offsetof({},{})",of->as_str(),member.text);
	}
	IMPL_FT
	{
		return offsetof_kw;
	}
};


struct SizeOrAlignmentInfoExpr : Expr
{
	SizeOrAlignmentInfoExpr(const Token& op, uptr<Expr>&& expr)
		:op(op),expr(mv(expr)){}
	IMPL_VISITOR;
	IMPL_CLONE(Expr){
		return uptr<Expr>(new SizeOrAlignmentInfoExpr(op,expr->clone()));
	}
	IMPL_ASSTR
	{
		return fmt::format("{}({})",op.type == Token::Specifier::KwAlignof ? "alignof" : "sizeof",expr->as_str());
	}
	IMPL_FT
	{
		return op;
	}
	Token op;
	uptr<Expr> expr;
};

enum class DecimalLiteralType
{
	Float=0,Double,Quad,Count
};

struct DecimalLiteralExpr : Expr
{
	DecimalLiteralExpr(const Token& token, DecimalLiteralType t, const std::variant<float,double>& decimal_holder)
		: lit(token),type(t),decimal_holder(decimal_holder) {}
	Token lit;
	DecimalLiteralType type;
	std::variant<float, double> decimal_holder;
	IMPL_VISITOR;
	IMPL_CLONE(Expr) { return uptr<Expr>(new DecimalLiteralExpr(*this)); }
	IMPL_ASSTR
	{
		return lit.text;
	}
		IMPL_FT
	{
		return lit;
	}
};


struct TypeSpec;


struct FptrIdentExpr : Expr
{
	FptrIdentExpr(const Token& token, std::vector<uptr<TypeSpec>>&& generic_params, std::vector<uptr<TypeSpec>>&& params)
		: name(token), generic_params(mv(generic_params)),params(mv(params)) {}
	FptrIdentExpr(const Token& token)
		:name(token), generic_params{}, params{} {}
	Token name;
	std::vector<uptr<TypeSpec>> generic_params;
	std::vector<uptr<TypeSpec>> params;

	FuncDefStmt* def = nullptr;
	
	IMPL_FT
	{
		return name;
	}
	IMPL_VISITOR;
	IMPL_CLONE(Expr)
	{
		CPY_VEC(generic_params, gp_cpy, uptr<TypeSpec>);
		CPY_VEC(params, p_cpy, uptr<TypeSpec>);
		return uptr<Expr>(new FptrIdentExpr(name, mv(gp_cpy), mv(p_cpy)));
	}
	IMPL_ASSTR
	{
		std::string gl = "<";
		if (!generic_params.empty())
		{
			gl += generic_params[0]->as_str();
		}

		for (int i = 1; i < generic_params.size(); i++)
		{
			auto& ts = generic_params[i];
			gl += "," + ts->as_str();
		}
		gl += ">";

		std::string p = "(;";
		if (!params.empty())
		{
			p += params[0]->as_str();
		}

		for (int i = 1; i < params.size(); i++)
		{
			auto& ts = params[i];
			p += "," + ts->as_str();
		}
		p += ")";

		return name.text + (generic_params.empty() ? "" : gl) + p;
	}
};

struct IdentExpr : Expr
{
	IdentExpr(const Token& token, std::vector<uptr<TypeSpec>>&& generic_params)
		: name(token), generic_params(mv(generic_params)) {}
	IdentExpr(const Token& token)
		:name(token), generic_params{}{}
	Token name;
	std::vector<uptr<TypeSpec>> generic_params;

	// Annotation
	DeclStmt* decl=nullptr;
	IMPL_FT
	{
		return name;
	}
	IMPL_VISITOR;
	IMPL_CLONE(Expr) {
		CPY_VEC(generic_params, gp_copy, uptr<TypeSpec>);
		auto ie = (new IdentExpr(name, mv(gp_copy)));
		ie->decl = decl;
		return uptr<Expr>(ie);
	}
	IMPL_ASSTR
	{
		std::string gl = "<";
		if (!generic_params.empty())
		{
			gl += generic_params[0]->as_str();
		}

		for (int i = 1; i < generic_params.size(); i++)
		{
			auto& ts = generic_params[i];
			gl += "," + ts->as_str();
		}
		gl += ">";
		return name.text + (generic_params.empty() ? "" : gl);
	}
};

struct FuncCallArg
{
	uptr<Expr> expr;
};

struct FuncCallExpr : Expr
{
	FuncCallExpr(uptr<Expr>&& from, std::vector<FuncCallArg>&& arg_list)
		: from(mv(from)), arg_list(mv(arg_list)) {}
	uptr<Expr> from;
	std::vector<FuncCallArg> arg_list;

	// Semantic annotations
	FuncDefStmt* def=nullptr;

	virtual bool is_sret_func_call() const override;
	virtual bool is_func_call() const override { return true; }

	IMPL_FT
	{
		return from->first_token();
	}
	IMPL_VISITOR;
	IMPL_CLONE(Expr)
	{
	std::vector<FuncCallArg> arg_list_cpy;
	for (int i = 0; i < arg_list.size(); i++)
	{
		arg_list_cpy.push_back({ arg_list[i].expr->clone() });
	}

	return uptr<Expr>(new FuncCallExpr(from->clone(), mv(arg_list_cpy)));
	}
	IMPL_ASSTR
	{
		std::string args = "";
		if (!arg_list.empty())
		{
			args += arg_list[0].expr->as_str();
		}

		for (int i = 1; i < arg_list.size(); i++)
		{
			auto& arg = arg_list[i];
			args += fmt::format(", {}", arg.expr->as_str());
		}

		return fmt::format("{}({})", from->as_str(), args);
	}
};
// TODO: Klammerung
struct MemOpExpr : Expr
{
	MemOpExpr(uptr<Expr>&& from, uptr<Expr>&& to, uptr<Expr>&& size, const Token& mt)
		:from(mv(from)),to(mv(to)),size(mv(size)),mem_type(mt) {}
	uptr<Expr> from, to;
	uptr<Expr> size;
	Token mem_type;

	IMPL_FT
	{
		return from->first_token();
	}
	IMPL_VISITOR;
	IMPL_CLONE(Expr)
	{
		return uptr<Expr>(new MemOpExpr(from->clone(), to->clone(), size->clone(), mem_type));
	}
	IMPL_ASSTR
	{
		return fmt::format("{} {} {} : {}", from->as_str(), mem_type.text, to->as_str(), size->as_str());
	}

};
 
struct SizeBetweenMemberInfoExpr : Expr
{
	SizeBetweenMemberInfoExpr(TypeDefStmt* of, const Token& mem1, const Token& mem2)
		:of(of),mem1(mem1),mem2(mem2){}
	TypeDefStmt* of;
	Token mem1, mem2;
	IMPL_FT
	{
		return mem1;
	}
	IMPL_VISITOR;
	IMPL_CLONE(Expr)
	{
		return uptr<Expr>(new SizeBetweenMemberInfoExpr(of,mem1,mem2));
	}
	IMPL_ASSTR
	{
		return "If you see this, then you hit a compilerbug. Printed SizeBetweenMemberInfoExpr";
	}
};


struct ArraySubscriptExpr : Expr
{
	ArraySubscriptExpr(uptr<Expr>&& from, uptr<Expr>&& inner)
		:from(mv(from)), inner(mv(inner)){}
	uptr<Expr> from, inner;
	IMPL_FT
	{
		return from->first_token();
	}
	IMPL_VISITOR;
	IMPL_CLONE(Expr) { return uptr<Expr>(new ArraySubscriptExpr(from->clone(), inner->clone())); }
	IMPL_ASSTR
	{
		return fmt::format("{}[{}]",from->as_str(),inner->as_str());
	}
};

struct TernaryExpr : Expr
{
	TernaryExpr(uptr<Expr>&& fst, uptr<Expr>&& snd, uptr<Expr>&& trd)
		:fst(mv(fst)), snd(mv(snd)), trd(mv(trd)) {}
	uptr<Expr> fst, snd, trd;
	IMPL_FT
	{
		return fst->first_token();
	}
	IMPL_VISITOR;
	IMPL_CLONE(Expr) { return uptr<Expr>(new TernaryExpr(fst->clone(), snd->clone(), trd->clone())); }
	IMPL_ASSTR
	{
		return fmt::format("{} ? {} : {}",fst->as_str(),snd->as_str(),trd->as_str());
	}
};


struct ImplicitCastExpr : Expr
{
	ImplicitCastExpr(uptr<Expr> expr, const Type& t)
		:expr(mv(expr))
	{
		sem_type = t;
	}
	uptr<Expr> expr;

	IMPL_FT
	{
		return expr->first_token();
	}
	IMPL_VISITOR;
	IMPL_CLONE(Expr) { return uptr<Expr>(new ImplicitCastExpr(expr->clone(), Type(sem_type))); }
	IMPL_ASSTR
	{
		return expr->as_str();
	}
};

// Statements

struct Stmt : Node {
	virtual uptr<Stmt> clone() const = 0;
	virtual bool is_return_stmt() const { return false; }
	virtual bool is_terminator() const { return false; }
};

struct TypedStmt : Stmt
{
	Type type;
};

struct ScopeStmt : Stmt
{
	ScopeStmt(std::vector<uptr<Stmt>>&& stmts)
		:stmts(mv(stmts)) {}
	std::vector<uptr<Stmt>> stmts;
	IMPL_VISITOR;
	IMPL_CLONE(Stmt)
	{
		CPY_VEC(stmts, stmts_cpy, uptr<Stmt>);
		return uptr<Stmt>(new ScopeStmt(mv(stmts_cpy)));
	}
};

struct ArrayInitInfo
{
	ArrayInitInfo(uptr<Expr> values, bool moved)
		:values(mv(values)), moved(moved) {}
	uptr<Expr> values;
	bool moved;
};

struct InitListArrayExpr : Expr
{
	InitListArrayExpr(uptr<TypeSpec> type_to_init,std::vector<ArrayInitInfo>&& values, const Token& brace)
		:type_to_init(mv(type_to_init)),values(mv(values)),brace(brace) {}
	std::vector<ArrayInitInfo> values;
	uptr<TypeSpec> type_to_init;
	Token brace;
	IMPL_VISITOR;
	IMPL_FT
	{
		return brace;
	}
	IMPL_CLONE(Expr)
	{
		CPY_VEC(values, values_cpy, uptr<Expr>);
		return uptr<Expr>(new InitListArrayExpr(mv(values_cpy),brace));
	}
	IMPL_ASSTR
	{
	std::string out = "";
	if (!values.empty())
	{
		out += fmt::format("{}", values[0]->as_str());
	}
	for (int i = 1; i < values.size(); i++)
	{
		auto& val = values[i];
		out += fmt::format(",{}", val->as_str());
	}
	return fmt::format("{{{}}}", out);
	}
};

struct StructInitInfo
{
	StructInitInfo(Token& member, uptr<Expr> init, bool moved)
		:member(member),init(mv(init)),moved(moved) {}
	Token member;
	uptr<Expr> init;
	bool moved;
};

struct InitListStructExpr : Expr
{
	InitListStructExpr(uptr<TypeSpec> type_to_init,	std::vector<StructInitInfo>&& sii,const Token& brace)
		:type_to_init(mv(type_to_init)),struct_init_info(mv(sii)),brace(brace) {}
	Token brace;
	std::vector<StructInitInfo> struct_init_info;
	uptr<TypeSpec> type_to_init;

	IMPL_FT
	{
		return type_to_init->get_ident_token();
	}
	IMPL_VISITOR;
	IMPL_CLONE(Expr)
	{
		std::vector<StructInitInfo> sii_cpy;
		for (auto& p : struct_init_info)
		{
			sii_cpy.emplace_back(p.member, p.init->clone(),p.moved);
		}
		return uptr<Expr>(new InitListStructExpr(type_to_init->clone(),mv(sii_cpy)));
	}
	IMPL_ASSTR
	{
		std::string out = "";
	if (!struct_init_info.empty())
	{
		auto& info = struct_init_info[0];
		out += fmt::format(".{}{}{}", info.member.text,info.moved ? "#" : "=",info.init->as_str());
	}
	for (int i = 1; i < struct_init_info.size(); i++)
	{
		auto& info = struct_init_info[i];
		out += fmt::format(",.{}{}{}", info.member.text, info.moved ? "#" : "=", info.init->as_str());
	}
	return fmt::format("{{{}}}", out);
	}
};

struct DeclOpStmt : TypedStmt
{
	DeclOpStmt(uptr<TypeSpec> type, const Token& name, uptr<Expr> expr)
		: type(mv(type)), name(name), expr(mv(expr)) {}
	Token name;
	uptr<TypeSpec> type;
	uptr<Expr> expr;
};

struct DeclCpyStmt : DeclOpStmt
{
	DeclCpyStmt(uptr<TypeSpec> type, const Token& name, uptr<Expr> expr)
		: DeclOpStmt(mv(type),name,mv(expr)) {}
	IMPL_VISITOR;
	IMPL_CLONE(Stmt) { return uptr<Stmt>(new DeclCpyStmt(type->clone(), name, expr->clone())); }
};

struct DeclMvStmt : DeclOpStmt
{
	DeclMvStmt(uptr<TypeSpec> type, const Token& name, uptr<Expr> expr)
		: DeclOpStmt(mv(type), name, mv(expr)) {}
	IMPL_VISITOR;
	IMPL_CLONE(Stmt) { return uptr<Stmt>(new DeclMvStmt(type->clone(), name, expr->clone())); }
};

struct DeclInitStmt : DeclOpStmt
{
	DeclInitStmt(uptr<TypeSpec> type, const Token& name, uptr<Expr> expr)
		: DeclOpStmt(mv(type), name, mv(expr)) {}
	IMPL_VISITOR;
	IMPL_CLONE(Stmt) { return uptr<Stmt>(new DeclInitStmt(type->clone(), name, expr->clone())); }
};

struct ContinueStmt : Stmt
{
	IMPL_VISITOR;
	IMPL_CLONE(Stmt) { return uptr<Stmt>(new ContinueStmt()); }
};

struct DeclStmt : TypedStmt
{
	DeclStmt(uptr<TypeSpec> type, const Token& name, uptr<InitList> init)
		:type_spec(mv(type)),name(name),init(mv(init)) {}

	Token name;
	uptr<TypeSpec> type_spec;
	uptr<InitList> init;
	bool is_sret = false;
	bool is_passed_as_ptr = false;
	IMPL_VISITOR;
	IMPL_CLONE(Stmt) {
		auto a = (new DeclStmt(type_spec ? type_spec->clone() : nullptr, name,init ? init->clone() : nullptr));
		a->type = type;
		a->is_sret = is_sret;
		a->is_passed_as_ptr = is_passed_as_ptr;
		return uptr<Stmt>(a);
	}
};

enum class CollectionType
{
	Union,Struct
};

// Used in union definitions.
struct UnionDeclStmt : Stmt
{
	UnionDeclStmt(uptr<DeclStmt>&& decl_stmt, const std::optional<EvalIntegerResult>& eir)
		:decl_stmt(mv(decl_stmt)), tagged_value(eir) {}
	uptr<DeclStmt> decl_stmt;
	std::optional<EvalIntegerResult> tagged_value;

	IMPL_VISITOR;
	IMPL_CLONE(Stmt)
	{
		auto decl = uptr<DeclStmt>(dynamic_cast<DeclStmt*>(decl_stmt->clone().release()));
		return uptr<Stmt>(new UnionDeclStmt(mv(decl), tagged_value));
	}
};

struct TypeDefStmt : Stmt
{
	TypeDefStmt(const Token& name, std::vector<GenericInfo>&& generic_params, std::vector<uptr<Stmt>>&& stmts, CollectionType ct)
		:ct(ct),name(name), generic_params(mv(generic_params)), stmts(mv(stmts)) {}
	TypeDefStmt(const std::string& name)
		:stmts{}, generic_params{},ct(CollectionType::Struct), name(Token(Token::Specifier::Ident,name)){}
	CollectionType ct;
	Token name;
	std::vector<GenericInfo> generic_params;
	std::vector<uptr<Stmt>> stmts;
	llvm::StructType* llvm_struct_type = nullptr;
	IMPL_VISITOR;
	IMPL_CLONE(Stmt) {
		CPY_VEC(stmts, stmts_cpy, uptr<Stmt>);
		std::vector<GenericInfo> gp_cpy;
		for (auto& p : generic_params) gp_cpy.push_back({ p.name,p.default_type ? p.default_type->clone() : nullptr });
		return uptr<Stmt>(new TypeDefStmt(name, mv(gp_cpy), mv(stmts_cpy),ct));
	}

	CollectionType get_ct() { return ct; }
	std::string get_collection_type()
	{
		return ct == CollectionType::Union ? "union" : "struct";
	}

};

struct NamespaceStmt : Stmt
{
	NamespaceStmt(std::vector<uptr<Stmt>>&& stmts, const Token& name)
		:stmts(mv(stmts)),name(name) {}
	NamespaceStmt(const Token& name)
		:name(name){}
	std::vector<uptr<Stmt>> stmts;
	Token name;
	IMPL_VISITOR;
	IMPL_CLONE(Stmt) { CPY_VEC(stmts, stmts_cpy, uptr<Stmt>) return uptr<Stmt>(new NamespaceStmt(mv(stmts_cpy), name)); }
};

struct WhileStmt : Stmt
{
	WhileStmt(uptr<Expr>&& expr, std::vector<uptr<Stmt>>&& stmts)
		:expr(mv(expr)),stmts(mv(stmts)){}
	uptr<Expr> expr;
	std::vector<uptr<Stmt>> stmts;
	IMPL_VISITOR;
	IMPL_CLONE(Stmt) { CPY_VEC(stmts, stmts_cpy, uptr<Stmt>); return uptr<Stmt>(new WhileStmt(expr->clone(), mv(stmts_cpy))); }
};

struct ForStmt : Stmt
{
	ForStmt(uptr<Stmt> decl_stmt, uptr<Expr> fst_expr, uptr<Expr> snd_expr, std::vector<uptr<Stmt>>&& stmts)
		:decl_stmt(mv(decl_stmt)),fst_expr(mv(fst_expr)),snd_expr(mv(snd_expr)),stmts(mv(stmts)){}
	uptr<Stmt> decl_stmt;
	uptr<Expr> fst_expr;
	uptr<Expr> snd_expr;
	std::vector<uptr<Stmt>> stmts;
	IMPL_VISITOR;
	IMPL_CLONE(Stmt) { CPY_VEC(stmts, stmts_cpy, uptr<Stmt>); return uptr<Stmt>(new ForStmt(decl_stmt->clone(), fst_expr->clone(), snd_expr->clone(), mv(stmts_cpy))); }
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
	IMPL_VISITOR;
	IMPL_CLONE(Stmt) {
		CPY_VEC(if_stmts,if_stmts_cpy,uptr<Stmt>);
	CPY_VEC(elif_exprs, elif_exprs_cpy, uptr<Expr>);
	CPY_VEC(else_stmts, else_stmts_cpy, uptr<Stmt>);
	std::vector<std::vector<uptr<Stmt>>> all_elif_stmts_cpy;
	for (auto& vec : all_elif_stmts)
	{
		CPY_VEC(vec, vec_cpy, uptr<Stmt>);
		all_elif_stmts_cpy.push_back(mv(vec_cpy));
	}
	return uptr<Stmt>(new IfStmt(if_expr->clone(), mv(if_stmts_cpy), mv(elif_exprs_cpy), mv(all_elif_stmts_cpy), mv(else_stmts_cpy)));
	}
};

struct FuncArg
{
	bool moved;
	uptr<DeclStmt> decl;
};

struct FuncDeclStmt : Stmt
{
	FuncDeclStmt(uptr<TypeSpec> ret_type, const Token& name, uptr<DeclStmt>&& named_return, std::vector<GenericInfo>&& generic_list, std::vector<FuncArg>&& arg_list,
		bool moved_return)
		: ret_type(mv(ret_type)), name(name), named_return(mv(named_return)), arg_list(mv(arg_list)), generic_list(mv(generic_list)),moved_return(moved_return) {}
	FuncDeclStmt(uptr<TypeSpec> ret_type, const Token& name, uptr<DeclStmt>&& named_return, std::vector<FuncArg>&& arg_list)
		:ret_type(mv(ret_type)),name(name),named_return(mv(named_return)), arg_list(mv(arg_list)),generic_list{}, moved_return(false) {}
	uptr<TypeSpec> ret_type;
	Token name;
	uptr<DeclStmt> named_return;
	std::vector<FuncArg> arg_list;
	std::vector<GenericInfo> generic_list;
	bool is_sret = false;
	bool is_gen = false;
	bool moved_return;
	bool is_generated() { return is_gen; }
	IMPL_VISITOR;
	IMPL_CLONE(Stmt) {
	std::vector<GenericInfo> generic_list_cpy;
	for (auto& p : generic_list) generic_list_cpy.push_back({ p.name, p.default_type?p.default_type->clone():nullptr });
	std::vector<FuncArg> arg_list_cpy;
	for (auto& p : arg_list) arg_list_cpy.push_back({ p.moved,uptr<DeclStmt>(new DeclStmt(p.decl->type_spec->clone(), p.decl->name)) });
	return uptr<Stmt>(new FuncDeclStmt(ret_type->clone(), 
		name,
		dynamic_uptr_cast_no_deleter<DeclStmt>(named_return->clone()),
		mv(generic_list_cpy),
		mv(arg_list_cpy),moved_return));
	}
};

struct FuncDefStmt : Stmt
{
	FuncDefStmt(uptr<FuncDeclStmt>&& func_decl, std::vector<uptr<Stmt>>&& body, bool is_operator=false)
		: decl(mv(func_decl)), body(mv(body)),is_operator(is_operator) {}
	uptr<FuncDeclStmt> decl;
	std::vector<uptr<Stmt>> body;
	bool is_operator;

	IMPL_VISITOR;
	IMPL_CLONE(Stmt) {
		CPY_VEC(body,body_cpy,uptr<Stmt>);
		return uptr<Stmt>(new FuncDefStmt(uptr<FuncDeclStmt>(dynamic_cast<FuncDeclStmt*>(decl->clone().release())), mv(body_cpy)));
	}
};

struct MatchCase 
{
	MatchCase(const Token& var, const std::optional<Token>& as, std::vector<uptr<Stmt>>&& body)
		:var(var),as(as), body(mv(body)){}
	Token var;
	std::optional<Token> as;
	uptr<DeclStmt> as_decl = nullptr;
	std::vector<uptr<Stmt>> body;
	MatchCase clone() const
	{
		CPY_VEC(body, body_cpy, uptr<Stmt>);
		return MatchCase(var, as, mv(body_cpy));
	}
};

struct MatchStmt : Stmt
{
	MatchStmt(std::vector<MatchCase>&& match_cases, uptr<Expr>&& match_on)
		:match_cases(mv(match_cases)),match_on(mv(match_on)){}
	std::vector<MatchCase> match_cases;
	uptr<Expr> match_on;
	IMPL_VISITOR;
	IMPL_CLONE(Stmt) {
		std::vector<MatchCase> match_cases_cpy;
	for (auto& p : match_cases) match_cases_cpy.push_back(p.clone());
		return uptr<Stmt>(new MatchStmt(mv(match_cases_cpy),match_on->clone()));
	}
};


struct ExprStmt : Stmt
{
	ExprStmt(uptr<Expr>&& expr)
		: expr(mv(expr)){}
	uptr<Expr> expr;

	IMPL_VISITOR;
	IMPL_CLONE(Stmt) {
		return uptr<Stmt>(new ExprStmt(expr->clone()));
	}
};

struct ReturnStmt : Stmt
{
	ReturnStmt(uptr<Expr>&& returned_expr, const Token& return_kw)
		: returned_expr(mv(returned_expr)),return_kw(return_kw) {}
	uptr<Expr> returned_expr;
	Token return_kw;
	IMPL_VISITOR;
	IMPL_CLONE(Stmt) {
		return uptr<Stmt>(new ReturnStmt(returned_expr->clone(),Token(return_kw)));
	}
	virtual bool is_return_stmt() const override { return true; }
	virtual bool is_terminator() const override { return true; }
};



