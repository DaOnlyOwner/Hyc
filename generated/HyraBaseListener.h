
// Generated from e:\C++Projects\Hyc\meta\Hyra.g4 by ANTLR 4.7.1

#pragma once


#include "antlr4-runtime.h"
#include "HyraListener.h"


/**
 * This class provides an empty implementation of HyraListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  HyraBaseListener : public HyraListener {
public:

  virtual void enterCompilationUnit(HyraParser::CompilationUnitContext * /*ctx*/) override { }
  virtual void exitCompilationUnit(HyraParser::CompilationUnitContext * /*ctx*/) override { }

  virtual void enterAssignmentExpr(HyraParser::AssignmentExprContext * /*ctx*/) override { }
  virtual void exitAssignmentExpr(HyraParser::AssignmentExprContext * /*ctx*/) override { }

  virtual void enterMulDivModExpr(HyraParser::MulDivModExprContext * /*ctx*/) override { }
  virtual void exitMulDivModExpr(HyraParser::MulDivModExprContext * /*ctx*/) override { }

  virtual void enterBitwiseNegateExpr(HyraParser::BitwiseNegateExprContext * /*ctx*/) override { }
  virtual void exitBitwiseNegateExpr(HyraParser::BitwiseNegateExprContext * /*ctx*/) override { }

  virtual void enterBitwiseOrExpr(HyraParser::BitwiseOrExprContext * /*ctx*/) override { }
  virtual void exitBitwiseOrExpr(HyraParser::BitwiseOrExprContext * /*ctx*/) override { }

  virtual void enterBitwiseAndExpr(HyraParser::BitwiseAndExprContext * /*ctx*/) override { }
  virtual void exitBitwiseAndExpr(HyraParser::BitwiseAndExprContext * /*ctx*/) override { }

  virtual void enterAddSubExpr(HyraParser::AddSubExprContext * /*ctx*/) override { }
  virtual void exitAddSubExpr(HyraParser::AddSubExprContext * /*ctx*/) override { }

  virtual void enterParenExpr(HyraParser::ParenExprContext * /*ctx*/) override { }
  virtual void exitParenExpr(HyraParser::ParenExprContext * /*ctx*/) override { }

  virtual void enterConstExpr(HyraParser::ConstExprContext * /*ctx*/) override { }
  virtual void exitConstExpr(HyraParser::ConstExprContext * /*ctx*/) override { }

  virtual void enterConstantExpr(HyraParser::ConstantExprContext * /*ctx*/) override { }
  virtual void exitConstantExpr(HyraParser::ConstantExprContext * /*ctx*/) override { }

  virtual void enterStmt(HyraParser::StmtContext * /*ctx*/) override { }
  virtual void exitStmt(HyraParser::StmtContext * /*ctx*/) override { }

  virtual void enterStmtList(HyraParser::StmtListContext * /*ctx*/) override { }
  virtual void exitStmtList(HyraParser::StmtListContext * /*ctx*/) override { }

  virtual void enterAtomicStmt(HyraParser::AtomicStmtContext * /*ctx*/) override { }
  virtual void exitAtomicStmt(HyraParser::AtomicStmtContext * /*ctx*/) override { }

  virtual void enterDecStmt(HyraParser::DecStmtContext * /*ctx*/) override { }
  virtual void exitDecStmt(HyraParser::DecStmtContext * /*ctx*/) override { }

  virtual void enterBlockStmt(HyraParser::BlockStmtContext * /*ctx*/) override { }
  virtual void exitBlockStmt(HyraParser::BlockStmtContext * /*ctx*/) override { }

  virtual void enterIfStmt(HyraParser::IfStmtContext * /*ctx*/) override { }
  virtual void exitIfStmt(HyraParser::IfStmtContext * /*ctx*/) override { }

  virtual void enterSimpleBlockStmt(HyraParser::SimpleBlockStmtContext * /*ctx*/) override { }
  virtual void exitSimpleBlockStmt(HyraParser::SimpleBlockStmtContext * /*ctx*/) override { }

  virtual void enterPattern(HyraParser::PatternContext * /*ctx*/) override { }
  virtual void exitPattern(HyraParser::PatternContext * /*ctx*/) override { }

  virtual void enterLiteral(HyraParser::LiteralContext * /*ctx*/) override { }
  virtual void exitLiteral(HyraParser::LiteralContext * /*ctx*/) override { }

  virtual void enterIdentifier(HyraParser::IdentifierContext * /*ctx*/) override { }
  virtual void exitIdentifier(HyraParser::IdentifierContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

