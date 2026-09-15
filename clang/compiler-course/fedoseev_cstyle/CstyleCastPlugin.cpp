#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Lex/Lexer.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/ADT/RewriteBuffer.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

namespace {

class CstyleCastVisitor : public RecursiveASTVisitor<CstyleCastVisitor> {
public:
  explicit CstyleCastVisitor(ASTContext *context, Rewriter &rewriter)
      : m_context(context), m_rewriter(rewriter) {}

  bool VisitCStyleCastExpr(CStyleCastExpr *cast) {
    QualType srcType = cast->getSubExpr()->getType();
    QualType dstType = cast->getTypeAsWritten();

    srcType = srcType.getCanonicalType();
    dstType = dstType.getCanonicalType();

    const char *castName = nullptr;

    if (srcType == dstType) {
      return true;
    }

    bool onlyCvDifference = false;
    if (srcType->isPointerType() && dstType->isPointerType()) {
      QualType srcPointee = srcType->getPointeeType().getUnqualifiedType();
      QualType dstPointee = dstType->getPointeeType().getUnqualifiedType();
      onlyCvDifference = m_context->hasSameType(srcPointee, dstPointee);
    } else {
      onlyCvDifference = m_context->hasSameUnqualifiedType(srcType, dstType);
    }
    if (onlyCvDifference) {
      castName = "const_cast";
    } else if (dstType->isPointerType() && srcType->isPointerType()) {
      const CXXRecordDecl *srcClass =
          srcType->getPointeeType()->getAsCXXRecordDecl();
      const CXXRecordDecl *dstClass =
          dstType->getPointeeType()->getAsCXXRecordDecl();
      if (srcClass && dstClass && srcClass->isPolymorphic()) {
        castName = "dynamic_cast";
      } else {
        castName = "reinterpret_cast";
      }
    } else {
      castName = "static_cast";
    }

    if (!castName)
      return true;

    SourceManager &sm = m_context->getSourceManager();
    const LangOptions &lo = m_context->getLangOpts();
    CharSourceRange subExprRange =
        CharSourceRange::getTokenRange(cast->getSubExpr()->getSourceRange());
    std::string subExprStr = Lexer::getSourceText(subExprRange, sm, lo).str();

    std::string replacement = std::string(castName) + "<" +
                              dstType.getAsString() + ">(" + subExprStr + ")";

    SourceRange castRange = cast->getSourceRange();
    m_rewriter.ReplaceText(castRange, replacement);

    return true;
  }

private:
  ASTContext *m_context;
  Rewriter &m_rewriter;
};

class CstyleCastConsumer : public ASTConsumer {
public:
  explicit CstyleCastConsumer(ASTContext *context, Rewriter &rewriter)
      : m_visitor(context, rewriter) {}

  void HandleTranslationUnit(ASTContext &context) override {
    m_visitor.TraverseDecl(context.getTranslationUnitDecl());
  }

private:
  CstyleCastVisitor m_visitor;
};

class CstyleCastAction : public PluginASTAction {
public:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &ci,
                                                 StringRef) override {
    m_rewriter.setSourceMgr(ci.getSourceManager(), ci.getLangOpts());
    return std::make_unique<CstyleCastConsumer>(&ci.getASTContext(),
                                                m_rewriter);
  }

  void EndSourceFileAction() override {
    SourceManager &sm = m_rewriter.getSourceMgr();
    FileID mainFileID = sm.getMainFileID();
    const llvm::RewriteBuffer *buf = m_rewriter.getRewriteBufferFor(mainFileID);
    if (buf) {
      llvm::outs() << std::string(buf->begin(), buf->end());
    } else {
      llvm::outs() << sm.getBufferData(mainFileID);
    }
  }

  bool ParseArgs(const CompilerInstance &ci,
                 const std::vector<std::string> &args) override {
    return true;
  }

private:
  Rewriter m_rewriter;
};

} // namespace

static FrontendPluginRegistry::Add<CstyleCastAction>
    X("cstyle_cast_plugin", "Replace C-style casts with C++ casts");