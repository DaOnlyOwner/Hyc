
// Generated from e:\C++Projects\Hyc\meta\Hyra.g4 by ANTLR 4.7.1

#pragma once


#include "antlr4-runtime.h"
#include "HyraParser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by HyraParser.
 */
class  HyraListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterCompilationUnit(HyraParser::CompilationUnitContext *ctx) = 0;
  virtual void exitCompilationUnit(HyraParser::CompilationUnitContext *ctx) = 0;

  virtual void enterAssignmentExpr(HyraParser::AssignmentExprContext *ctx) = 0;
  virtual void exitAssignmentExpr(HyraParser::AssignmentExprContext *ctx) = 0;

  virtual void enterMulDivModExpr(HyraParser::MulDivModExprContext *ctx) = 0;
  virtual void exitMulDivModExpr(HyraParser::MulDivModExprContext *ctx) = 0;

  virtual void enterBitwiseNegateExpr(HyraParser::BitwiseNegateExprContext *ctx) = 0;
  virtual void exitBitwiseNegateExpr(HyraParser::BitwiseNegateExprContext *ctx) = 0;

  virtual void enterBitwiseOrExpr(HyraParser::BitwiseOrExprContext *ctx) = 0;
  virtual void exitBitwiseOrExpr(HyraParser::BitwiseOrExprContext *ctx) = 0;

  virtual void enterBitwiseAndExpr(HyraParser::BitwiseAndExprContext *ctx) = 0;
  virtual void exitBitwiseAndExpr(HyraParser::BitwiseAndExprContext *ctx) = 0;

  virtual void enterAddSubExpr(HyraParser::AddSubExprContext *ctx) = 0;
  virtual void exitAddSubExpr(HyraParser::AddSubExprContext *ctx) = 0;

  virtual void enterParenExpr(HyraParser::ParenExprContext *ctx) = 0;
  virtual void exitParenExpr(HyraParser::ParenExprContext *ctx) = 0;

  virtual void enterConstExpr(HyraParser::ConstExprContext *ctx) = 0;
  virtual void exitConstExpr(HyraParser::ConstExprContext *ctx) = 0;

  virtual void enterConstantExpr(HyraParser::ConstantExprContext *ctx) = 0;
  virtual void exitConstantExpr(HyraParser::ConstantExprContext *ctx) = 0;

  virtual void enterStmt(HyraParser::StmtContext *ctx) = 0;
  virtual void exitStmt(HyraParser::StmtContext *ctx) = 0;

  virtual void enterStmtList(HyraParser::StmtListContext *ctx) = 0;
  virtual void exitStmtList(HyraParser::StmtListContext *ctx) = 0;

  virtual void enterAtomicStmt(HyraParser::AtomicStmtContext *ctx) = 0;
  virtual void exitAtomicStmt(HyraParser::AtomicStmtContext *ctx) = 0;

  virtual void enterDecStmt(HyraParser::DecStmtContext *ctx) = 0;
  virtual void exitDecStmt(HyraParser::DecStmtContext *ctx) = 0;

  virtual void enterBlockStmt(HyraParser::BlockStmtContext *ctx) = 0;
  virtual void exitBlockStmt(HyraParser::BlockStmtContext *ctx) = 0;

  virtual void enterIfStmt(HyraParser::IfStmtContext *ctx) = 0;
  virtual void exitIfStmt(HyraParser::IfStmtContext *ctx) = 0;

  virtual void enterSimpleBlockStmt(HyraParser::SimpleBlockStmtContext *ctx) = 0;
  virtual void exitSimpleBlockStmt(HyraParser::SimpleBlockStmtContext *ctx) = 0;

  virtual void enterPattern(HyraParser::PatternContext *ctx) = 0;
  virtual void exitPattern(HyraParser::PatternContext *ctx) = 0;

  virtual void enterLiteral(HyraParser::LiteralContext *ctx) = 0;
  virtual void exitLiteral(HyraParser::LiteralContext *ctx) = 0;

  virtual void enterIdentifier(HyraParser::IdentifierContext *ctx) = 0;
  virtual void exitIdentifier(HyraParser::IdentifierContext *ctx) = 0;


};

