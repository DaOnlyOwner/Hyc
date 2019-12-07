#pragma once
#include "HyraVisitor.h"
#include "StackCollection.h"
#include "HyraParser.h"

class PipelineStage_TypeChecking : HyraVisitor
{
private:
	StackCollection m_stack;

public:
	StackCollection ObtainStack()
	{
		return std::move(m_stack);
	}


	// Geerbt über HyraVisitor
	virtual antlrcpp::Any visitCompilationUnit(HyraParser::CompilationUnitContext* context) override;
	virtual antlrcpp::Any visitAssignmentExpr(HyraParser::AssignmentExprContext* context) override;
	virtual antlrcpp::Any visitMulDivModExpr(HyraParser::MulDivModExprContext* context) override;
	virtual antlrcpp::Any visitBitwiseNegateExpr(HyraParser::BitwiseNegateExprContext* context) override;
	virtual antlrcpp::Any visitBitwiseOrExpr(HyraParser::BitwiseOrExprContext* context) override;
	virtual antlrcpp::Any visitBitwiseAndExpr(HyraParser::BitwiseAndExprContext* context) override;
	virtual antlrcpp::Any visitAddSubExpr(HyraParser::AddSubExprContext* context) override;
	virtual antlrcpp::Any visitParenExpr(HyraParser::ParenExprContext* context) override;
	virtual antlrcpp::Any visitConstExpr(HyraParser::ConstExprContext* context) override;
	virtual antlrcpp::Any visitConstantExpr(HyraParser::ConstantExprContext* context) override;
	virtual antlrcpp::Any visitStmt(HyraParser::StmtContext* context) override;
	virtual antlrcpp::Any visitStmtList(HyraParser::StmtListContext* context) override;
	virtual antlrcpp::Any visitAtomicStmt(HyraParser::AtomicStmtContext* context) override;
	virtual antlrcpp::Any visitDecStmt(HyraParser::DecStmtContext* context) override;
	virtual antlrcpp::Any visitBlockStmt(HyraParser::BlockStmtContext* context) override;
	virtual antlrcpp::Any visitIfStmt(HyraParser::IfStmtContext* context) override;
	virtual antlrcpp::Any visitSimpleBlockStmt(HyraParser::SimpleBlockStmtContext* context) override;
	virtual antlrcpp::Any visitPattern(HyraParser::PatternContext* context) override;
	virtual antlrcpp::Any visitLiteral(HyraParser::LiteralContext* context) override;
	virtual antlrcpp::Any visitIdentifier(HyraParser::IdentifierContext* context) override;
};
