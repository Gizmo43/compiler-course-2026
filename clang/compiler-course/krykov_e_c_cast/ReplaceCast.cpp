#include "clang/AST/ASTConsumer.h"
#include "clang/AST/Expr.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Lex/Lexer.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

namespace {

class CStyleCastVisitor : public RecursiveASTVisitor<CStyleCastVisitor> {
public:
  CStyleCastVisitor(ASTContext *Ctx, clang::Rewriter &R)
      : Context(Ctx), RW(R) {}

  bool VisitCStyleCastExpr(CStyleCastExpr *Node) {

    SourceManager &SM = Context->getSourceManager();


    if (SM.isInSystemHeader(Node->getBeginLoc()))
      return true;

    const Expr *SubExpr = Node->getSubExpr();

    std::string CastName = determineCastKind(Node, SubExpr);
    std::string DestType = Node->getTypeAsWritten().getAsString();

    std::string Replacement = CastName + "<" + DestType + ">(";

    SourceRange ParenRange(Node->getLParenLoc(), Node->getRParenLoc());
    RW.ReplaceText(ParenRange, Replacement);

    SourceLocation EndLoc =
        Lexer::getLocForEndOfToken(SubExpr->getEndLoc(), 0, SM,
                                   Context->getLangOpts());

    RW.InsertTextAfterToken(EndLoc, ")");

    return true;
  }

private:
  std::string determineCastKind(CStyleCastExpr *Node, const Expr *SubExpr) {

    CastKind Kind = Node->getCastKind();


    if (Kind == CK_BitCast ||
        Kind == CK_LValueBitCast ||
        Kind == CK_PointerToIntegral ||
        Kind == CK_IntegralToPointer)
      return "reinterpret_cast";

    QualType SrcType = SubExpr->getType();
    QualType DstType = Node->getType();


    if (SrcType.isConstQualified() != DstType.isConstQualified() ||
        SrcType.isVolatileQualified() != DstType.isVolatileQualified())
      return "const_cast";


    return "static_cast";
  }

  ASTContext *Context;
  clang::Rewriter &RW;
};

class CStyleCastConsumer : public ASTConsumer {
public:
  CStyleCastConsumer(ASTContext *Ctx, clang::Rewriter &R)
      : Visitor(Ctx, R) {}

  void HandleTranslationUnit(ASTContext &Ctx) override {
    Visitor.TraverseDecl(Ctx.getTranslationUnitDecl());
  }

private:
  CStyleCastVisitor Visitor;
};

class CStyleCastAction : public clang::PluginASTAction {
public:
  std::unique_ptr<ASTConsumer>
  CreateASTConsumer(CompilerInstance &CI, llvm::StringRef) override {

    RewriterInstance.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());

    return std::make_unique<CStyleCastConsumer>(&CI.getASTContext(),
                                                RewriterInstance);
  }

  bool ParseArgs(const CompilerInstance &,
                 const std::vector<std::string> &) override {
    return true;
  }

  void EndSourceFileAction() override {

    SourceManager &SM = RewriterInstance.getSourceMgr();

    RewriterInstance.getEditBuffer(SM.getMainFileID()).write(llvm::outs());
  }

private:
  clang::Rewriter RewriterInstance;
};

} // namespace

static clang::FrontendPluginRegistry::Add<CStyleCastAction>
    X("replace_c_cast", "Replace C-style casts with C++ casts");