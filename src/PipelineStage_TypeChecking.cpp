#include "StackCreator.h"
#include <set>
#include <algorithm>
#include "DebugPrint.h"
#include <Symbol.h>
#include "TypeChecker.h"

antlrcpp::Any StackCreatorVisitor::visitCompilationUnit(HyraParser::CompilationUnitContext* context)
{
	
	context->stmtList.accept(this);
}

antlrcpp::Any StackCreatorVisitor::visitAssignmentExpr(HyraParser::AssignmentExprContext* context)
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

antlrcpp::Any StackCreatorVisitor::visitMulDivModExpr(HyraParser::MulDivModExprContext* context)
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

antlrcpp::Any StackCreatorVisitor::visitBitwiseNegateExpr(HyraParser::BitwiseNegateExprContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any StackCreatorVisitor::visitBitwiseOrExpr(HyraParser::BitwiseOrExprContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any StackCreatorVisitor::visitBitwiseAndExpr(HyraParser::BitwiseAndExprContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any StackCreatorVisitor::visitAddSubExpr(HyraParser::AddSubExprContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any StackCreatorVisitor::visitParenExpr(HyraParser::ParenExprContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any StackCreatorVisitor::visitConstExpr(HyraParser::ConstExprContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any StackCreatorVisitor::visitConstantExpr(HyraParser::ConstantExprContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any StackCreatorVisitor::visitStmt(HyraParser::StmtContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any StackCreatorVisitor::visitStmtList(HyraParser::StmtListContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any StackCreatorVisitor::visitAtomicStmt(HyraParser::AtomicStmtContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any StackCreatorVisitor::visitDecStmt(HyraParser::DecStmtContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any StackCreatorVisitor::visitBlockStmt(HyraParser::BlockStmtContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any StackCreatorVisitor::visitIfStmt(HyraParser::IfStmtContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any StackCreatorVisitor::visitSimpleBlockStmt(HyraParser::SimpleBlockStmtContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any StackCreatorVisitor::visitPattern(HyraParser::PatternContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any StackCreatorVisitor::visitLiteral(HyraParser::LiteralContext* context)
{
	return antlrcpp::Any();
}

antlrcpp::Any StackCreatorVisitor::visitIdentifier(HyraParser::IdentifierContext* context)
{
	return antlrcpp::Any();
}
