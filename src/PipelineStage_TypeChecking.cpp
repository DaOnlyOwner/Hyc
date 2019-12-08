#include "PipelineStage_TypeChecking.h"
#include <set>
#include <algorithm>
#include "DebugPrint.h"
#include <Symbol.h>
#include "TypeChecker.h"

antlrcpp::Any PipelineStage_TypeChecking::visitCompilationUnit(HyraParser::CompilationUnitContext* context)
{
	
	context->stmtList.accept(this);
}

antlrcpp::Any PipelineStage_TypeChecking::visitAssignmentExpr(HyraParser::AssignmentExprContext* context)
{
	Variable var;
	var.Name = context->lh->accept(this).as<std::string>();
	var.Uid = UID::Zero;
	m_stack.Add(std::move(var));
}

UID getReturnTypeOperator(const StackCollection& stack, const UID& typeLh, const UID& typeRh, antlr4::Token* op, const std::string& name, const std::string& operatorName)
{
	auto* functionsPtr = stack.GetFunctions(name);
	assert(functionsPtr != nullptr);
	auto& functions = *functionsPtr;
	auto type = TypeChecker::InferType_FuncCall2Ary(stack, functions, typeLh, typeRh);
	if (type.has_value()) return type.value;
	else
	{
		std::string typeName1 = stack.GetByUID(typeLh).Name;
		std::string typeName2 = stack.GetByUID(typeRh).Name;
		SemanticErrorDescription descr;
		descr.Message = "No overload of " + operatorName + "("+name+") for the argument types " + typeName1 + " and " + typeName2 + " defined";
		descr.From = op->getStartIndex();
		descr.To = op->getStopIndex();
		descr.File = op->getInputStream()->getSourceName();
		descr.Line = op->getLine();
		descr.Hint = "Define an overload of _mul_ with the two types as the arguments: {return_type} " + name + "(" + typeName1 + "{var_name1}, " + typeName2 + "{var_name2} )";
		SemanticError(descr);
		return UID::Zero;
	}
}

antlrcpp::Any PipelineStage_TypeChecking::visitMulDivModExpr(HyraParser::MulDivModExprContext* context)
{
	UID typeLh = context->lh->accept(this).as<UID>();
	UID typeRh = context->rh->accept(this).as<UID>();
	if (context->op->getText().c_str() == "*") return getReturnTypeOperator(m_stack, typeLh, typeRh, context->op, "_mul_", "+");
	else if (context->op->getText().c_str() == "/") return getReturnTypeOperator(m_stack, typeLh, typeRh, context->op, "_div_", "/");
	else if (context->op->getText().c_str() == "%") return getReturnTypeOperator(m_stack, typeLh, typeRh, context->op, "_mod_", "%");
	else
	{
		assert(false);
	}	
}

antlrcpp::Any PipelineStage_TypeChecking::visitBitwiseNegateExpr(HyraParser::BitwiseNegateExprContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any PipelineStage_TypeChecking::visitBitwiseOrExpr(HyraParser::BitwiseOrExprContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any PipelineStage_TypeChecking::visitBitwiseAndExpr(HyraParser::BitwiseAndExprContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any PipelineStage_TypeChecking::visitAddSubExpr(HyraParser::AddSubExprContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any PipelineStage_TypeChecking::visitParenExpr(HyraParser::ParenExprContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any PipelineStage_TypeChecking::visitConstExpr(HyraParser::ConstExprContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any PipelineStage_TypeChecking::visitConstantExpr(HyraParser::ConstantExprContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any PipelineStage_TypeChecking::visitStmt(HyraParser::StmtContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any PipelineStage_TypeChecking::visitStmtList(HyraParser::StmtListContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any PipelineStage_TypeChecking::visitAtomicStmt(HyraParser::AtomicStmtContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any PipelineStage_TypeChecking::visitDecStmt(HyraParser::DecStmtContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any PipelineStage_TypeChecking::visitBlockStmt(HyraParser::BlockStmtContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any PipelineStage_TypeChecking::visitIfStmt(HyraParser::IfStmtContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any PipelineStage_TypeChecking::visitSimpleBlockStmt(HyraParser::SimpleBlockStmtContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any PipelineStage_TypeChecking::visitPattern(HyraParser::PatternContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any PipelineStage_TypeChecking::visitLiteral(HyraParser::LiteralContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any PipelineStage_TypeChecking::visitIdentifier(HyraParser::IdentifierContext* context)
{
	return antlrcpp::Any();
}
