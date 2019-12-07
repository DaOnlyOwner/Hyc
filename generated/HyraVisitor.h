
// Generated from e:\C++Projects\Hyc\meta\Hyra.g4 by ANTLR 4.7.1

#pragma once


#include "antlr4-runtime.h"
#include "HyraParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by HyraParser.
 */
class  HyraVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by HyraParser.
   */
    virtual antlrcpp::Any visitCompilationUnit(HyraParser::CompilationUnitContext *context) = 0;

    virtual antlrcpp::Any visitAssignmentExpr(HyraParser::AssignmentExprContext *context) = 0;

    virtual antlrcpp::Any visitMulDivModExpr(HyraParser::MulDivModExprContext *context) = 0;

    virtual antlrcpp::Any visitBitwiseNegateExpr(HyraParser::BitwiseNegateExprContext *context) = 0;

    virtual antlrcpp::Any visitBitwiseOrExpr(HyraParser::BitwiseOrExprContext *context) = 0;

    virtual antlrcpp::Any visitBitwiseAndExpr(HyraParser::BitwiseAndExprContext *context) = 0;

    virtual antlrcpp::Any visitAddSubExpr(HyraParser::AddSubExprContext *context) = 0;

    virtual antlrcpp::Any visitParenExpr(HyraParser::ParenExprContext *context) = 0;

    virtual antlrcpp::Any visitConstExpr(HyraParser::ConstExprContext *context) = 0;

    virtual antlrcpp::Any visitConstantExpr(HyraParser::ConstantExprContext *context) = 0;

    virtual antlrcpp::Any visitStmt(HyraParser::StmtContext *context) = 0;

    virtual antlrcpp::Any visitStmtList(HyraParser::StmtListContext *context) = 0;

    virtual antlrcpp::Any visitAtomicStmt(HyraParser::AtomicStmtContext *context) = 0;

    virtual antlrcpp::Any visitDecStmt(HyraParser::DecStmtContext *context) = 0;

    virtual antlrcpp::Any visitBlockStmt(HyraParser::BlockStmtContext *context) = 0;

    virtual antlrcpp::Any visitIfStmt(HyraParser::IfStmtContext *context) = 0;

    virtual antlrcpp::Any visitSimpleBlockStmt(HyraParser::SimpleBlockStmtContext *context) = 0;

    virtual antlrcpp::Any visitPattern(HyraParser::PatternContext *context) = 0;

    virtual antlrcpp::Any visitLiteral(HyraParser::LiteralContext *context) = 0;

    virtual antlrcpp::Any visitIdentifier(HyraParser::IdentifierContext *context) = 0;


};

