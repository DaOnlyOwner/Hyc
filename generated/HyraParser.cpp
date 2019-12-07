
// Generated from e:\C++Projects\Hyc\meta\Hyra.g4 by ANTLR 4.7.1


#include "HyraListener.h"
#include "HyraVisitor.h"

#include "HyraParser.h"


using namespace antlrcpp;
using namespace antlr4;

HyraParser::HyraParser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

HyraParser::~HyraParser() {
  delete _interpreter;
}

std::string HyraParser::getGrammarFileName() const {
  return "Hyra.g4";
}

const std::vector<std::string>& HyraParser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& HyraParser::getVocabulary() const {
  return _vocabulary;
}


//----------------- CompilationUnitContext ------------------------------------------------------------------

HyraParser::CompilationUnitContext::CompilationUnitContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

HyraParser::StmtListContext* HyraParser::CompilationUnitContext::stmtList() {
  return getRuleContext<HyraParser::StmtListContext>(0);
}

tree::TerminalNode* HyraParser::CompilationUnitContext::EOF() {
  return getToken(HyraParser::EOF, 0);
}


size_t HyraParser::CompilationUnitContext::getRuleIndex() const {
  return HyraParser::RuleCompilationUnit;
}

void HyraParser::CompilationUnitContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCompilationUnit(this);
}

void HyraParser::CompilationUnitContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCompilationUnit(this);
}


antlrcpp::Any HyraParser::CompilationUnitContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HyraVisitor*>(visitor))
    return parserVisitor->visitCompilationUnit(this);
  else
    return visitor->visitChildren(this);
}

HyraParser::CompilationUnitContext* HyraParser::compilationUnit() {
  CompilationUnitContext *_localctx = _tracker.createInstance<CompilationUnitContext>(_ctx, getState());
  enterRule(_localctx, 0, HyraParser::RuleCompilationUnit);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(26);
    stmtList();
    setState(27);
    match(HyraParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExprContext ------------------------------------------------------------------

HyraParser::ExprContext::ExprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t HyraParser::ExprContext::getRuleIndex() const {
  return HyraParser::RuleExpr;
}

void HyraParser::ExprContext::copyFrom(ExprContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- AssignmentExprContext ------------------------------------------------------------------

HyraParser::PatternContext* HyraParser::AssignmentExprContext::pattern() {
  return getRuleContext<HyraParser::PatternContext>(0);
}

HyraParser::ExprContext* HyraParser::AssignmentExprContext::expr() {
  return getRuleContext<HyraParser::ExprContext>(0);
}

HyraParser::AssignmentExprContext::AssignmentExprContext(ExprContext *ctx) { copyFrom(ctx); }

void HyraParser::AssignmentExprContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAssignmentExpr(this);
}
void HyraParser::AssignmentExprContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAssignmentExpr(this);
}

antlrcpp::Any HyraParser::AssignmentExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HyraVisitor*>(visitor))
    return parserVisitor->visitAssignmentExpr(this);
  else
    return visitor->visitChildren(this);
}
//----------------- MulDivModExprContext ------------------------------------------------------------------

std::vector<HyraParser::ExprContext *> HyraParser::MulDivModExprContext::expr() {
  return getRuleContexts<HyraParser::ExprContext>();
}

HyraParser::ExprContext* HyraParser::MulDivModExprContext::expr(size_t i) {
  return getRuleContext<HyraParser::ExprContext>(i);
}

HyraParser::MulDivModExprContext::MulDivModExprContext(ExprContext *ctx) { copyFrom(ctx); }

void HyraParser::MulDivModExprContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMulDivModExpr(this);
}
void HyraParser::MulDivModExprContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMulDivModExpr(this);
}

antlrcpp::Any HyraParser::MulDivModExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HyraVisitor*>(visitor))
    return parserVisitor->visitMulDivModExpr(this);
  else
    return visitor->visitChildren(this);
}
//----------------- BitwiseNegateExprContext ------------------------------------------------------------------

HyraParser::ExprContext* HyraParser::BitwiseNegateExprContext::expr() {
  return getRuleContext<HyraParser::ExprContext>(0);
}

HyraParser::BitwiseNegateExprContext::BitwiseNegateExprContext(ExprContext *ctx) { copyFrom(ctx); }

void HyraParser::BitwiseNegateExprContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBitwiseNegateExpr(this);
}
void HyraParser::BitwiseNegateExprContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBitwiseNegateExpr(this);
}

antlrcpp::Any HyraParser::BitwiseNegateExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HyraVisitor*>(visitor))
    return parserVisitor->visitBitwiseNegateExpr(this);
  else
    return visitor->visitChildren(this);
}
//----------------- BitwiseOrExprContext ------------------------------------------------------------------

std::vector<HyraParser::ExprContext *> HyraParser::BitwiseOrExprContext::expr() {
  return getRuleContexts<HyraParser::ExprContext>();
}

HyraParser::ExprContext* HyraParser::BitwiseOrExprContext::expr(size_t i) {
  return getRuleContext<HyraParser::ExprContext>(i);
}

HyraParser::BitwiseOrExprContext::BitwiseOrExprContext(ExprContext *ctx) { copyFrom(ctx); }

void HyraParser::BitwiseOrExprContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBitwiseOrExpr(this);
}
void HyraParser::BitwiseOrExprContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBitwiseOrExpr(this);
}

antlrcpp::Any HyraParser::BitwiseOrExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HyraVisitor*>(visitor))
    return parserVisitor->visitBitwiseOrExpr(this);
  else
    return visitor->visitChildren(this);
}
//----------------- BitwiseAndExprContext ------------------------------------------------------------------

std::vector<HyraParser::ExprContext *> HyraParser::BitwiseAndExprContext::expr() {
  return getRuleContexts<HyraParser::ExprContext>();
}

HyraParser::ExprContext* HyraParser::BitwiseAndExprContext::expr(size_t i) {
  return getRuleContext<HyraParser::ExprContext>(i);
}

HyraParser::BitwiseAndExprContext::BitwiseAndExprContext(ExprContext *ctx) { copyFrom(ctx); }

void HyraParser::BitwiseAndExprContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBitwiseAndExpr(this);
}
void HyraParser::BitwiseAndExprContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBitwiseAndExpr(this);
}

antlrcpp::Any HyraParser::BitwiseAndExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HyraVisitor*>(visitor))
    return parserVisitor->visitBitwiseAndExpr(this);
  else
    return visitor->visitChildren(this);
}
//----------------- AddSubExprContext ------------------------------------------------------------------

std::vector<HyraParser::ExprContext *> HyraParser::AddSubExprContext::expr() {
  return getRuleContexts<HyraParser::ExprContext>();
}

HyraParser::ExprContext* HyraParser::AddSubExprContext::expr(size_t i) {
  return getRuleContext<HyraParser::ExprContext>(i);
}

HyraParser::AddSubExprContext::AddSubExprContext(ExprContext *ctx) { copyFrom(ctx); }

void HyraParser::AddSubExprContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAddSubExpr(this);
}
void HyraParser::AddSubExprContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAddSubExpr(this);
}

antlrcpp::Any HyraParser::AddSubExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HyraVisitor*>(visitor))
    return parserVisitor->visitAddSubExpr(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ParenExprContext ------------------------------------------------------------------

HyraParser::ExprContext* HyraParser::ParenExprContext::expr() {
  return getRuleContext<HyraParser::ExprContext>(0);
}

HyraParser::ParenExprContext::ParenExprContext(ExprContext *ctx) { copyFrom(ctx); }

void HyraParser::ParenExprContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterParenExpr(this);
}
void HyraParser::ParenExprContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitParenExpr(this);
}

antlrcpp::Any HyraParser::ParenExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HyraVisitor*>(visitor))
    return parserVisitor->visitParenExpr(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ConstExprContext ------------------------------------------------------------------

HyraParser::ConstantExprContext* HyraParser::ConstExprContext::constantExpr() {
  return getRuleContext<HyraParser::ConstantExprContext>(0);
}

HyraParser::ConstExprContext::ConstExprContext(ExprContext *ctx) { copyFrom(ctx); }

void HyraParser::ConstExprContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterConstExpr(this);
}
void HyraParser::ConstExprContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitConstExpr(this);
}

antlrcpp::Any HyraParser::ConstExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HyraVisitor*>(visitor))
    return parserVisitor->visitConstExpr(this);
  else
    return visitor->visitChildren(this);
}

HyraParser::ExprContext* HyraParser::expr() {
   return expr(0);
}

HyraParser::ExprContext* HyraParser::expr(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  HyraParser::ExprContext *_localctx = _tracker.createInstance<ExprContext>(_ctx, parentState);
  HyraParser::ExprContext *previousContext = _localctx;
  size_t startState = 2;
  enterRecursionRule(_localctx, 2, HyraParser::RuleExpr, precedence);

    size_t _la = 0;

  auto onExit = finally([=] {
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(39);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 0, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<ParenExprContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;

      setState(30);
      match(HyraParser::T__0);
      setState(31);
      expr(0);
      setState(32);
      match(HyraParser::T__1);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<ConstExprContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(34);
      constantExpr();
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<AssignmentExprContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(35);
      dynamic_cast<AssignmentExprContext *>(_localctx)->lh = pattern();
      setState(36);
      match(HyraParser::T__10);
      setState(37);
      dynamic_cast<AssignmentExprContext *>(_localctx)->rh = expr(1);
      break;
    }

    }
    _ctx->stop = _input->LT(-1);
    setState(57);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(55);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx)) {
        case 1: {
          auto newContext = _tracker.createInstance<BitwiseOrExprContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          newContext->lh = previousContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(41);

          if (!(precpred(_ctx, 6))) throw FailedPredicateException(this, "precpred(_ctx, 6)");
          setState(42);
          match(HyraParser::T__2);
          setState(43);
          dynamic_cast<BitwiseOrExprContext *>(_localctx)->rh = expr(7);
          break;
        }

        case 2: {
          auto newContext = _tracker.createInstance<BitwiseAndExprContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          newContext->lh = previousContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(44);

          if (!(precpred(_ctx, 4))) throw FailedPredicateException(this, "precpred(_ctx, 4)");
          setState(45);
          match(HyraParser::T__4);
          setState(46);
          dynamic_cast<BitwiseAndExprContext *>(_localctx)->rh = expr(5);
          break;
        }

        case 3: {
          auto newContext = _tracker.createInstance<AddSubExprContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          newContext->lh = previousContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(47);

          if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");
          setState(48);
          dynamic_cast<AddSubExprContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == HyraParser::T__5

          || _la == HyraParser::T__6)) {
            dynamic_cast<AddSubExprContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(49);
          dynamic_cast<AddSubExprContext *>(_localctx)->rh = expr(4);
          break;
        }

        case 4: {
          auto newContext = _tracker.createInstance<MulDivModExprContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          newContext->lh = previousContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(50);

          if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
          setState(51);
          dynamic_cast<MulDivModExprContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!((((_la & ~ 0x3fULL) == 0) &&
            ((1ULL << _la) & ((1ULL << HyraParser::T__7)
            | (1ULL << HyraParser::T__8)
            | (1ULL << HyraParser::T__9))) != 0))) {
            dynamic_cast<MulDivModExprContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(52);
          dynamic_cast<MulDivModExprContext *>(_localctx)->rh = expr(3);
          break;
        }

        case 5: {
          auto newContext = _tracker.createInstance<BitwiseNegateExprContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          newContext->lh = previousContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(53);

          if (!(precpred(_ctx, 5))) throw FailedPredicateException(this, "precpred(_ctx, 5)");
          setState(54);
          match(HyraParser::T__3);
          break;
        }

        } 
      }
      setState(59);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- ConstantExprContext ------------------------------------------------------------------

HyraParser::ConstantExprContext::ConstantExprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

HyraParser::LiteralContext* HyraParser::ConstantExprContext::literal() {
  return getRuleContext<HyraParser::LiteralContext>(0);
}

HyraParser::IdentifierContext* HyraParser::ConstantExprContext::identifier() {
  return getRuleContext<HyraParser::IdentifierContext>(0);
}


size_t HyraParser::ConstantExprContext::getRuleIndex() const {
  return HyraParser::RuleConstantExpr;
}

void HyraParser::ConstantExprContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterConstantExpr(this);
}

void HyraParser::ConstantExprContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitConstantExpr(this);
}


antlrcpp::Any HyraParser::ConstantExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HyraVisitor*>(visitor))
    return parserVisitor->visitConstantExpr(this);
  else
    return visitor->visitChildren(this);
}

HyraParser::ConstantExprContext* HyraParser::constantExpr() {
  ConstantExprContext *_localctx = _tracker.createInstance<ConstantExprContext>(_ctx, getState());
  enterRule(_localctx, 4, HyraParser::RuleConstantExpr);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(62);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case HyraParser::DecIntegerLiteral:
      case HyraParser::FloatLiteral: {
        enterOuterAlt(_localctx, 1);
        setState(60);
        literal();
        break;
      }

      case HyraParser::Identifier: {
        enterOuterAlt(_localctx, 2);
        setState(61);
        identifier();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StmtContext ------------------------------------------------------------------

HyraParser::StmtContext::StmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

HyraParser::AtomicStmtContext* HyraParser::StmtContext::atomicStmt() {
  return getRuleContext<HyraParser::AtomicStmtContext>(0);
}

HyraParser::DecStmtContext* HyraParser::StmtContext::decStmt() {
  return getRuleContext<HyraParser::DecStmtContext>(0);
}

HyraParser::BlockStmtContext* HyraParser::StmtContext::blockStmt() {
  return getRuleContext<HyraParser::BlockStmtContext>(0);
}


size_t HyraParser::StmtContext::getRuleIndex() const {
  return HyraParser::RuleStmt;
}

void HyraParser::StmtContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStmt(this);
}

void HyraParser::StmtContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStmt(this);
}


antlrcpp::Any HyraParser::StmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HyraVisitor*>(visitor))
    return parserVisitor->visitStmt(this);
  else
    return visitor->visitChildren(this);
}

HyraParser::StmtContext* HyraParser::stmt() {
  StmtContext *_localctx = _tracker.createInstance<StmtContext>(_ctx, getState());
  enterRule(_localctx, 6, HyraParser::RuleStmt);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(67);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case HyraParser::T__0:
      case HyraParser::Identifier:
      case HyraParser::DecIntegerLiteral:
      case HyraParser::FloatLiteral: {
        enterOuterAlt(_localctx, 1);
        setState(64);
        atomicStmt();
        break;
      }

      case HyraParser::T__12: {
        enterOuterAlt(_localctx, 2);
        setState(65);
        decStmt();
        break;
      }

      case HyraParser::T__13:
      case HyraParser::T__14: {
        enterOuterAlt(_localctx, 3);
        setState(66);
        blockStmt();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StmtListContext ------------------------------------------------------------------

HyraParser::StmtListContext::StmtListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<HyraParser::StmtContext *> HyraParser::StmtListContext::stmt() {
  return getRuleContexts<HyraParser::StmtContext>();
}

HyraParser::StmtContext* HyraParser::StmtListContext::stmt(size_t i) {
  return getRuleContext<HyraParser::StmtContext>(i);
}


size_t HyraParser::StmtListContext::getRuleIndex() const {
  return HyraParser::RuleStmtList;
}

void HyraParser::StmtListContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStmtList(this);
}

void HyraParser::StmtListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStmtList(this);
}


antlrcpp::Any HyraParser::StmtListContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HyraVisitor*>(visitor))
    return parserVisitor->visitStmtList(this);
  else
    return visitor->visitChildren(this);
}

HyraParser::StmtListContext* HyraParser::stmtList() {
  StmtListContext *_localctx = _tracker.createInstance<StmtListContext>(_ctx, getState());
  enterRule(_localctx, 8, HyraParser::RuleStmtList);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(72);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << HyraParser::T__0)
      | (1ULL << HyraParser::T__12)
      | (1ULL << HyraParser::T__13)
      | (1ULL << HyraParser::T__14)
      | (1ULL << HyraParser::Identifier)
      | (1ULL << HyraParser::DecIntegerLiteral)
      | (1ULL << HyraParser::FloatLiteral))) != 0)) {
      setState(69);
      stmt();
      setState(74);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AtomicStmtContext ------------------------------------------------------------------

HyraParser::AtomicStmtContext::AtomicStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

HyraParser::ExprContext* HyraParser::AtomicStmtContext::expr() {
  return getRuleContext<HyraParser::ExprContext>(0);
}


size_t HyraParser::AtomicStmtContext::getRuleIndex() const {
  return HyraParser::RuleAtomicStmt;
}

void HyraParser::AtomicStmtContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAtomicStmt(this);
}

void HyraParser::AtomicStmtContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAtomicStmt(this);
}


antlrcpp::Any HyraParser::AtomicStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HyraVisitor*>(visitor))
    return parserVisitor->visitAtomicStmt(this);
  else
    return visitor->visitChildren(this);
}

HyraParser::AtomicStmtContext* HyraParser::atomicStmt() {
  AtomicStmtContext *_localctx = _tracker.createInstance<AtomicStmtContext>(_ctx, getState());
  enterRule(_localctx, 10, HyraParser::RuleAtomicStmt);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(75);
    expr(0);
    setState(76);
    match(HyraParser::T__11);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DecStmtContext ------------------------------------------------------------------

HyraParser::DecStmtContext::DecStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

HyraParser::PatternContext* HyraParser::DecStmtContext::pattern() {
  return getRuleContext<HyraParser::PatternContext>(0);
}

HyraParser::ExprContext* HyraParser::DecStmtContext::expr() {
  return getRuleContext<HyraParser::ExprContext>(0);
}


size_t HyraParser::DecStmtContext::getRuleIndex() const {
  return HyraParser::RuleDecStmt;
}

void HyraParser::DecStmtContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDecStmt(this);
}

void HyraParser::DecStmtContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDecStmt(this);
}


antlrcpp::Any HyraParser::DecStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HyraVisitor*>(visitor))
    return parserVisitor->visitDecStmt(this);
  else
    return visitor->visitChildren(this);
}

HyraParser::DecStmtContext* HyraParser::decStmt() {
  DecStmtContext *_localctx = _tracker.createInstance<DecStmtContext>(_ctx, getState());
  enterRule(_localctx, 12, HyraParser::RuleDecStmt);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(78);
    match(HyraParser::T__12);
    setState(79);
    pattern();
    setState(80);
    match(HyraParser::T__10);
    setState(81);
    expr(0);
    setState(82);
    match(HyraParser::T__11);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BlockStmtContext ------------------------------------------------------------------

HyraParser::BlockStmtContext::BlockStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

HyraParser::IfStmtContext* HyraParser::BlockStmtContext::ifStmt() {
  return getRuleContext<HyraParser::IfStmtContext>(0);
}

HyraParser::SimpleBlockStmtContext* HyraParser::BlockStmtContext::simpleBlockStmt() {
  return getRuleContext<HyraParser::SimpleBlockStmtContext>(0);
}


size_t HyraParser::BlockStmtContext::getRuleIndex() const {
  return HyraParser::RuleBlockStmt;
}

void HyraParser::BlockStmtContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBlockStmt(this);
}

void HyraParser::BlockStmtContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBlockStmt(this);
}


antlrcpp::Any HyraParser::BlockStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HyraVisitor*>(visitor))
    return parserVisitor->visitBlockStmt(this);
  else
    return visitor->visitChildren(this);
}

HyraParser::BlockStmtContext* HyraParser::blockStmt() {
  BlockStmtContext *_localctx = _tracker.createInstance<BlockStmtContext>(_ctx, getState());
  enterRule(_localctx, 14, HyraParser::RuleBlockStmt);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(86);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case HyraParser::T__13: {
        enterOuterAlt(_localctx, 1);
        setState(84);
        ifStmt();
        break;
      }

      case HyraParser::T__14: {
        enterOuterAlt(_localctx, 2);
        setState(85);
        simpleBlockStmt();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IfStmtContext ------------------------------------------------------------------

HyraParser::IfStmtContext::IfStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

HyraParser::ExprContext* HyraParser::IfStmtContext::expr() {
  return getRuleContext<HyraParser::ExprContext>(0);
}

HyraParser::StmtListContext* HyraParser::IfStmtContext::stmtList() {
  return getRuleContext<HyraParser::StmtListContext>(0);
}

HyraParser::StmtContext* HyraParser::IfStmtContext::stmt() {
  return getRuleContext<HyraParser::StmtContext>(0);
}


size_t HyraParser::IfStmtContext::getRuleIndex() const {
  return HyraParser::RuleIfStmt;
}

void HyraParser::IfStmtContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIfStmt(this);
}

void HyraParser::IfStmtContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIfStmt(this);
}


antlrcpp::Any HyraParser::IfStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HyraVisitor*>(visitor))
    return parserVisitor->visitIfStmt(this);
  else
    return visitor->visitChildren(this);
}

HyraParser::IfStmtContext* HyraParser::ifStmt() {
  IfStmtContext *_localctx = _tracker.createInstance<IfStmtContext>(_ctx, getState());
  enterRule(_localctx, 16, HyraParser::RuleIfStmt);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(88);
    match(HyraParser::T__13);
    setState(89);
    match(HyraParser::T__0);
    setState(90);
    expr(0);
    setState(91);
    match(HyraParser::T__1);
    setState(100);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case HyraParser::T__14: {
        setState(92);
        match(HyraParser::T__14);
        setState(93);
        stmtList();
        setState(94);
        match(HyraParser::T__15);
        break;
      }

      case HyraParser::T__16: {
        setState(96);
        match(HyraParser::T__16);
        setState(97);
        stmt();
        setState(98);
        match(HyraParser::T__11);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SimpleBlockStmtContext ------------------------------------------------------------------

HyraParser::SimpleBlockStmtContext::SimpleBlockStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

HyraParser::StmtListContext* HyraParser::SimpleBlockStmtContext::stmtList() {
  return getRuleContext<HyraParser::StmtListContext>(0);
}


size_t HyraParser::SimpleBlockStmtContext::getRuleIndex() const {
  return HyraParser::RuleSimpleBlockStmt;
}

void HyraParser::SimpleBlockStmtContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSimpleBlockStmt(this);
}

void HyraParser::SimpleBlockStmtContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSimpleBlockStmt(this);
}


antlrcpp::Any HyraParser::SimpleBlockStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HyraVisitor*>(visitor))
    return parserVisitor->visitSimpleBlockStmt(this);
  else
    return visitor->visitChildren(this);
}

HyraParser::SimpleBlockStmtContext* HyraParser::simpleBlockStmt() {
  SimpleBlockStmtContext *_localctx = _tracker.createInstance<SimpleBlockStmtContext>(_ctx, getState());
  enterRule(_localctx, 18, HyraParser::RuleSimpleBlockStmt);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(102);
    match(HyraParser::T__14);
    setState(103);
    stmtList();
    setState(104);
    match(HyraParser::T__15);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PatternContext ------------------------------------------------------------------

HyraParser::PatternContext::PatternContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

HyraParser::LiteralContext* HyraParser::PatternContext::literal() {
  return getRuleContext<HyraParser::LiteralContext>(0);
}

HyraParser::IdentifierContext* HyraParser::PatternContext::identifier() {
  return getRuleContext<HyraParser::IdentifierContext>(0);
}


size_t HyraParser::PatternContext::getRuleIndex() const {
  return HyraParser::RulePattern;
}

void HyraParser::PatternContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPattern(this);
}

void HyraParser::PatternContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPattern(this);
}


antlrcpp::Any HyraParser::PatternContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HyraVisitor*>(visitor))
    return parserVisitor->visitPattern(this);
  else
    return visitor->visitChildren(this);
}

HyraParser::PatternContext* HyraParser::pattern() {
  PatternContext *_localctx = _tracker.createInstance<PatternContext>(_ctx, getState());
  enterRule(_localctx, 20, HyraParser::RulePattern);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(108);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case HyraParser::DecIntegerLiteral:
      case HyraParser::FloatLiteral: {
        enterOuterAlt(_localctx, 1);
        setState(106);
        literal();
        break;
      }

      case HyraParser::Identifier: {
        enterOuterAlt(_localctx, 2);
        setState(107);
        identifier();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LiteralContext ------------------------------------------------------------------

HyraParser::LiteralContext::LiteralContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* HyraParser::LiteralContext::DecIntegerLiteral() {
  return getToken(HyraParser::DecIntegerLiteral, 0);
}

tree::TerminalNode* HyraParser::LiteralContext::FloatLiteral() {
  return getToken(HyraParser::FloatLiteral, 0);
}


size_t HyraParser::LiteralContext::getRuleIndex() const {
  return HyraParser::RuleLiteral;
}

void HyraParser::LiteralContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLiteral(this);
}

void HyraParser::LiteralContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLiteral(this);
}


antlrcpp::Any HyraParser::LiteralContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HyraVisitor*>(visitor))
    return parserVisitor->visitLiteral(this);
  else
    return visitor->visitChildren(this);
}

HyraParser::LiteralContext* HyraParser::literal() {
  LiteralContext *_localctx = _tracker.createInstance<LiteralContext>(_ctx, getState());
  enterRule(_localctx, 22, HyraParser::RuleLiteral);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(110);
    _la = _input->LA(1);
    if (!(_la == HyraParser::DecIntegerLiteral

    || _la == HyraParser::FloatLiteral)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IdentifierContext ------------------------------------------------------------------

HyraParser::IdentifierContext::IdentifierContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* HyraParser::IdentifierContext::Identifier() {
  return getToken(HyraParser::Identifier, 0);
}


size_t HyraParser::IdentifierContext::getRuleIndex() const {
  return HyraParser::RuleIdentifier;
}

void HyraParser::IdentifierContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIdentifier(this);
}

void HyraParser::IdentifierContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HyraListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIdentifier(this);
}


antlrcpp::Any HyraParser::IdentifierContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HyraVisitor*>(visitor))
    return parserVisitor->visitIdentifier(this);
  else
    return visitor->visitChildren(this);
}

HyraParser::IdentifierContext* HyraParser::identifier() {
  IdentifierContext *_localctx = _tracker.createInstance<IdentifierContext>(_ctx, getState());
  enterRule(_localctx, 24, HyraParser::RuleIdentifier);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(112);
    match(HyraParser::Identifier);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

bool HyraParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 1: return exprSempred(dynamic_cast<ExprContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool HyraParser::exprSempred(ExprContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 6);
    case 1: return precpred(_ctx, 4);
    case 2: return precpred(_ctx, 3);
    case 3: return precpred(_ctx, 2);
    case 4: return precpred(_ctx, 5);

  default:
    break;
  }
  return true;
}

// Static vars and initialization.
std::vector<dfa::DFA> HyraParser::_decisionToDFA;
atn::PredictionContextCache HyraParser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN HyraParser::_atn;
std::vector<uint16_t> HyraParser::_serializedATN;

std::vector<std::string> HyraParser::_ruleNames = {
  "compilationUnit", "expr", "constantExpr", "stmt", "stmtList", "atomicStmt", 
  "decStmt", "blockStmt", "ifStmt", "simpleBlockStmt", "pattern", "literal", 
  "identifier"
};

std::vector<std::string> HyraParser::_literalNames = {
  "", "'('", "')'", "'|'", "'^'", "'&'", "'+'", "'-'", "'%'", "'*'", "'/'", 
  "'='", "';'", "'var'", "'if'", "'{'", "'}'", "'then'"
};

std::vector<std::string> HyraParser::_symbolicNames = {
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
  "Identifier", "DecIntegerLiteral", "FloatLiteral", "Whitespace", "Newline", 
  "BlockComment", "LineComment"
};

dfa::Vocabulary HyraParser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> HyraParser::_tokenNames;

HyraParser::Initializer::Initializer() {
	for (size_t i = 0; i < _symbolicNames.size(); ++i) {
		std::string name = _vocabulary.getLiteralName(i);
		if (name.empty()) {
			name = _vocabulary.getSymbolicName(i);
		}

		if (name.empty()) {
			_tokenNames.push_back("<INVALID>");
		} else {
      _tokenNames.push_back(name);
    }
	}

  _serializedATN = {
    0x3, 0x608b, 0xa72a, 0x8133, 0xb9ed, 0x417c, 0x3be7, 0x7786, 0x5964, 
    0x3, 0x1a, 0x75, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 0x9, 
    0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x4, 0x7, 0x9, 0x7, 0x4, 
    0x8, 0x9, 0x8, 0x4, 0x9, 0x9, 0x9, 0x4, 0xa, 0x9, 0xa, 0x4, 0xb, 0x9, 
    0xb, 0x4, 0xc, 0x9, 0xc, 0x4, 0xd, 0x9, 0xd, 0x4, 0xe, 0x9, 0xe, 0x3, 
    0x2, 0x3, 0x2, 0x3, 0x2, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x5, 0x3, 0x2a, 
    0xa, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 
    0x3, 0x3, 0x7, 0x3, 0x3a, 0xa, 0x3, 0xc, 0x3, 0xe, 0x3, 0x3d, 0xb, 0x3, 
    0x3, 0x4, 0x3, 0x4, 0x5, 0x4, 0x41, 0xa, 0x4, 0x3, 0x5, 0x3, 0x5, 0x3, 
    0x5, 0x5, 0x5, 0x46, 0xa, 0x5, 0x3, 0x6, 0x7, 0x6, 0x49, 0xa, 0x6, 0xc, 
    0x6, 0xe, 0x6, 0x4c, 0xb, 0x6, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x3, 0x8, 
    0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x9, 0x3, 0x9, 
    0x5, 0x9, 0x59, 0xa, 0x9, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 
    0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 
    0xa, 0x5, 0xa, 0x67, 0xa, 0xa, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 
    0x3, 0xc, 0x3, 0xc, 0x5, 0xc, 0x6f, 0xa, 0xc, 0x3, 0xd, 0x3, 0xd, 0x3, 
    0xe, 0x3, 0xe, 0x3, 0xe, 0x2, 0x3, 0x4, 0xf, 0x2, 0x4, 0x6, 0x8, 0xa, 
    0xc, 0xe, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x2, 0x5, 0x3, 0x2, 0x8, 
    0x9, 0x3, 0x2, 0xa, 0xc, 0x3, 0x2, 0x15, 0x16, 0x2, 0x75, 0x2, 0x1c, 
    0x3, 0x2, 0x2, 0x2, 0x4, 0x29, 0x3, 0x2, 0x2, 0x2, 0x6, 0x40, 0x3, 0x2, 
    0x2, 0x2, 0x8, 0x45, 0x3, 0x2, 0x2, 0x2, 0xa, 0x4a, 0x3, 0x2, 0x2, 0x2, 
    0xc, 0x4d, 0x3, 0x2, 0x2, 0x2, 0xe, 0x50, 0x3, 0x2, 0x2, 0x2, 0x10, 
    0x58, 0x3, 0x2, 0x2, 0x2, 0x12, 0x5a, 0x3, 0x2, 0x2, 0x2, 0x14, 0x68, 
    0x3, 0x2, 0x2, 0x2, 0x16, 0x6e, 0x3, 0x2, 0x2, 0x2, 0x18, 0x70, 0x3, 
    0x2, 0x2, 0x2, 0x1a, 0x72, 0x3, 0x2, 0x2, 0x2, 0x1c, 0x1d, 0x5, 0xa, 
    0x6, 0x2, 0x1d, 0x1e, 0x7, 0x2, 0x2, 0x3, 0x1e, 0x3, 0x3, 0x2, 0x2, 
    0x2, 0x1f, 0x20, 0x8, 0x3, 0x1, 0x2, 0x20, 0x21, 0x7, 0x3, 0x2, 0x2, 
    0x21, 0x22, 0x5, 0x4, 0x3, 0x2, 0x22, 0x23, 0x7, 0x4, 0x2, 0x2, 0x23, 
    0x2a, 0x3, 0x2, 0x2, 0x2, 0x24, 0x2a, 0x5, 0x6, 0x4, 0x2, 0x25, 0x26, 
    0x5, 0x16, 0xc, 0x2, 0x26, 0x27, 0x7, 0xd, 0x2, 0x2, 0x27, 0x28, 0x5, 
    0x4, 0x3, 0x3, 0x28, 0x2a, 0x3, 0x2, 0x2, 0x2, 0x29, 0x1f, 0x3, 0x2, 
    0x2, 0x2, 0x29, 0x24, 0x3, 0x2, 0x2, 0x2, 0x29, 0x25, 0x3, 0x2, 0x2, 
    0x2, 0x2a, 0x3b, 0x3, 0x2, 0x2, 0x2, 0x2b, 0x2c, 0xc, 0x8, 0x2, 0x2, 
    0x2c, 0x2d, 0x7, 0x5, 0x2, 0x2, 0x2d, 0x3a, 0x5, 0x4, 0x3, 0x9, 0x2e, 
    0x2f, 0xc, 0x6, 0x2, 0x2, 0x2f, 0x30, 0x7, 0x7, 0x2, 0x2, 0x30, 0x3a, 
    0x5, 0x4, 0x3, 0x7, 0x31, 0x32, 0xc, 0x5, 0x2, 0x2, 0x32, 0x33, 0x9, 
    0x2, 0x2, 0x2, 0x33, 0x3a, 0x5, 0x4, 0x3, 0x6, 0x34, 0x35, 0xc, 0x4, 
    0x2, 0x2, 0x35, 0x36, 0x9, 0x3, 0x2, 0x2, 0x36, 0x3a, 0x5, 0x4, 0x3, 
    0x5, 0x37, 0x38, 0xc, 0x7, 0x2, 0x2, 0x38, 0x3a, 0x7, 0x6, 0x2, 0x2, 
    0x39, 0x2b, 0x3, 0x2, 0x2, 0x2, 0x39, 0x2e, 0x3, 0x2, 0x2, 0x2, 0x39, 
    0x31, 0x3, 0x2, 0x2, 0x2, 0x39, 0x34, 0x3, 0x2, 0x2, 0x2, 0x39, 0x37, 
    0x3, 0x2, 0x2, 0x2, 0x3a, 0x3d, 0x3, 0x2, 0x2, 0x2, 0x3b, 0x39, 0x3, 
    0x2, 0x2, 0x2, 0x3b, 0x3c, 0x3, 0x2, 0x2, 0x2, 0x3c, 0x5, 0x3, 0x2, 
    0x2, 0x2, 0x3d, 0x3b, 0x3, 0x2, 0x2, 0x2, 0x3e, 0x41, 0x5, 0x18, 0xd, 
    0x2, 0x3f, 0x41, 0x5, 0x1a, 0xe, 0x2, 0x40, 0x3e, 0x3, 0x2, 0x2, 0x2, 
    0x40, 0x3f, 0x3, 0x2, 0x2, 0x2, 0x41, 0x7, 0x3, 0x2, 0x2, 0x2, 0x42, 
    0x46, 0x5, 0xc, 0x7, 0x2, 0x43, 0x46, 0x5, 0xe, 0x8, 0x2, 0x44, 0x46, 
    0x5, 0x10, 0x9, 0x2, 0x45, 0x42, 0x3, 0x2, 0x2, 0x2, 0x45, 0x43, 0x3, 
    0x2, 0x2, 0x2, 0x45, 0x44, 0x3, 0x2, 0x2, 0x2, 0x46, 0x9, 0x3, 0x2, 
    0x2, 0x2, 0x47, 0x49, 0x5, 0x8, 0x5, 0x2, 0x48, 0x47, 0x3, 0x2, 0x2, 
    0x2, 0x49, 0x4c, 0x3, 0x2, 0x2, 0x2, 0x4a, 0x48, 0x3, 0x2, 0x2, 0x2, 
    0x4a, 0x4b, 0x3, 0x2, 0x2, 0x2, 0x4b, 0xb, 0x3, 0x2, 0x2, 0x2, 0x4c, 
    0x4a, 0x3, 0x2, 0x2, 0x2, 0x4d, 0x4e, 0x5, 0x4, 0x3, 0x2, 0x4e, 0x4f, 
    0x7, 0xe, 0x2, 0x2, 0x4f, 0xd, 0x3, 0x2, 0x2, 0x2, 0x50, 0x51, 0x7, 
    0xf, 0x2, 0x2, 0x51, 0x52, 0x5, 0x16, 0xc, 0x2, 0x52, 0x53, 0x7, 0xd, 
    0x2, 0x2, 0x53, 0x54, 0x5, 0x4, 0x3, 0x2, 0x54, 0x55, 0x7, 0xe, 0x2, 
    0x2, 0x55, 0xf, 0x3, 0x2, 0x2, 0x2, 0x56, 0x59, 0x5, 0x12, 0xa, 0x2, 
    0x57, 0x59, 0x5, 0x14, 0xb, 0x2, 0x58, 0x56, 0x3, 0x2, 0x2, 0x2, 0x58, 
    0x57, 0x3, 0x2, 0x2, 0x2, 0x59, 0x11, 0x3, 0x2, 0x2, 0x2, 0x5a, 0x5b, 
    0x7, 0x10, 0x2, 0x2, 0x5b, 0x5c, 0x7, 0x3, 0x2, 0x2, 0x5c, 0x5d, 0x5, 
    0x4, 0x3, 0x2, 0x5d, 0x66, 0x7, 0x4, 0x2, 0x2, 0x5e, 0x5f, 0x7, 0x11, 
    0x2, 0x2, 0x5f, 0x60, 0x5, 0xa, 0x6, 0x2, 0x60, 0x61, 0x7, 0x12, 0x2, 
    0x2, 0x61, 0x67, 0x3, 0x2, 0x2, 0x2, 0x62, 0x63, 0x7, 0x13, 0x2, 0x2, 
    0x63, 0x64, 0x5, 0x8, 0x5, 0x2, 0x64, 0x65, 0x7, 0xe, 0x2, 0x2, 0x65, 
    0x67, 0x3, 0x2, 0x2, 0x2, 0x66, 0x5e, 0x3, 0x2, 0x2, 0x2, 0x66, 0x62, 
    0x3, 0x2, 0x2, 0x2, 0x67, 0x13, 0x3, 0x2, 0x2, 0x2, 0x68, 0x69, 0x7, 
    0x11, 0x2, 0x2, 0x69, 0x6a, 0x5, 0xa, 0x6, 0x2, 0x6a, 0x6b, 0x7, 0x12, 
    0x2, 0x2, 0x6b, 0x15, 0x3, 0x2, 0x2, 0x2, 0x6c, 0x6f, 0x5, 0x18, 0xd, 
    0x2, 0x6d, 0x6f, 0x5, 0x1a, 0xe, 0x2, 0x6e, 0x6c, 0x3, 0x2, 0x2, 0x2, 
    0x6e, 0x6d, 0x3, 0x2, 0x2, 0x2, 0x6f, 0x17, 0x3, 0x2, 0x2, 0x2, 0x70, 
    0x71, 0x9, 0x4, 0x2, 0x2, 0x71, 0x19, 0x3, 0x2, 0x2, 0x2, 0x72, 0x73, 
    0x7, 0x14, 0x2, 0x2, 0x73, 0x1b, 0x3, 0x2, 0x2, 0x2, 0xb, 0x29, 0x39, 
    0x3b, 0x40, 0x45, 0x4a, 0x58, 0x66, 0x6e, 
  };

  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

HyraParser::Initializer HyraParser::_init;
