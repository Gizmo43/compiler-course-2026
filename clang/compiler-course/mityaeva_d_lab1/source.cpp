#include "clang/AST/ASTConsumer.h"
#include "clang/AST/Expr.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Lex/Lexer.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/Support/raw_ostream.h"

#include <memory>
#include <string>

namespace {

enum class CastKind { Static, Const, Reinterpret, Dynamic };

llvm::StringRef getCastName(CastKind kind) {
  switch (kind) {
  case CastKind::Static:
    return "static_cast";
  case CastKind::Const:
    return "const_cast";
  case CastKind::Reinterpret:
    return "reinterpret_cast";
  case CastKind::Dynamic:
    return "dynamic_cast";
  }
  return "static_cast";
}

class CastClassifier {
public:
  explicit CastClassifier(clang::ASTContext &ctx) : Context(ctx) {}

  CastKind choose(const clang::CStyleCastExpr *cast) const {
    const clang::Expr *sub = cast->getSubExpr();
    if (!sub)
      return CastKind::Static;

    clang::QualType fromTy = sub->getType().getCanonicalType();
    clang::QualType toTy = cast->getTypeAsWritten().getCanonicalType();

    if (isConstVolatileAdjustment(fromTy, toTy))
      return CastKind::Const;
    if (isDowncastForPolymorphic(fromTy, toTy))
      return CastKind::Dynamic;
    if (isPointerIntegerMix(fromTy, toTy))
      return CastKind::Reinterpret;
    if (isVoidPointerConversion(fromTy, toTy))
      return CastKind::Static;
    if (isUnrelatedPointers(fromTy, toTy))
      return CastKind::Reinterpret;
    if (isArithmeticOrEnum(fromTy) && isArithmeticOrEnum(toTy))
      return CastKind::Static;
    return CastKind::Static;
  }

private:
  // Атрибут [[maybe_unused]] убирает предупреждение о неиспользуемом поле
  [[maybe_unused]] clang::ASTContext &Context;

  bool isArithmeticOrEnum(clang::QualType T) const {
    return T->isArithmeticType() || T->isEnumeralType();
  }

  bool isPointerIntegerMix(clang::QualType From, clang::QualType To) const {
    return (From->isPointerType() && To->isIntegerType()) ||
           (From->isIntegerType() && To->isPointerType());
  }

  bool isVoidPointerConversion(clang::QualType From, clang::QualType To) const {
    if (!From->isPointerType() || !To->isPointerType())
      return false;
    clang::QualType FromPointee = From->getPointeeType();
    clang::QualType ToPointee = To->getPointeeType();
    return FromPointee->isVoidType() || ToPointee->isVoidType();
  }

  bool isUnrelatedPointers(clang::QualType From, clang::QualType To) const {
    if (!From->isPointerType() || !To->isPointerType())
      return false;
    clang::QualType FromPointee = From->getPointeeType().getCanonicalType();
    clang::QualType ToPointee = To->getPointeeType().getCanonicalType();
    if (FromPointee.getUnqualifiedType() == ToPointee.getUnqualifiedType())
      return false;
    if (areRelatedClasses(FromPointee, ToPointee))
      return false;
    if (FromPointee->isVoidType() || ToPointee->isVoidType())
      return false;
    return true;
  }

  bool isConstVolatileAdjustment(clang::QualType From,
                                 clang::QualType To) const {
    From = From.getCanonicalType();
    To = To.getCanonicalType();

    if (From->isPointerType() && To->isPointerType()) {
      clang::QualType FromPointee = From->getPointeeType();
      clang::QualType ToPointee = To->getPointeeType();
      return FromPointee.getUnqualifiedType() ==
                 ToPointee.getUnqualifiedType() &&
             FromPointee != ToPointee;
    }

    if (To->isReferenceType()) {
      clang::QualType FromBase = From.getNonReferenceType();
      clang::QualType ToBase = To.getNonReferenceType();
      return FromBase.getUnqualifiedType() == ToBase.getUnqualifiedType() &&
             FromBase != ToBase;
    }

    return false;
  }

  bool areRelatedClasses(clang::QualType A, clang::QualType B) const {
    const auto *DeclA = A->getAsCXXRecordDecl();
    const auto *DeclB = B->getAsCXXRecordDecl();
    if (!DeclA || !DeclB)
      return false;
    DeclA = DeclA->getDefinition();
    DeclB = DeclB->getDefinition();
    if (!DeclA || !DeclB)
      return false;
    return DeclA == DeclB || DeclA->isDerivedFrom(DeclB) ||
           DeclB->isDerivedFrom(DeclA);
  }

  bool isDowncastForPolymorphic(clang::QualType From,
                                clang::QualType To) const {
    clang::QualType FromBase = From;
    clang::QualType ToBase = To;
    if (FromBase->isPointerType() && ToBase->isPointerType()) {
      FromBase = FromBase->getPointeeType();
      ToBase = ToBase->getPointeeType();
    } else if (FromBase->isReferenceType() && ToBase->isReferenceType()) {
      FromBase = FromBase->getPointeeType();
      ToBase = ToBase->getPointeeType();
    } else {
      return false;
    }
    const auto *FromDecl = FromBase->getAsCXXRecordDecl();
    const auto *ToDecl = ToBase->getAsCXXRecordDecl();
    if (!FromDecl || !ToDecl)
      return false;
    FromDecl = FromDecl->getDefinition();
    ToDecl = ToDecl->getDefinition();
    if (!FromDecl || !ToDecl)
      return false;
    if (!FromDecl->isPolymorphic())
      return false;
    return ToDecl->isDerivedFrom(FromDecl);
  }
};

class CastTextBuilder {
public:
  explicit CastTextBuilder(clang::ASTContext &ctx) : Context(ctx) {}

  std::string build(const clang::CStyleCastExpr *cast, CastKind kind) const {
    std::string typeText = getTypeTextFromCast(cast);
    std::string exprText = getSourceText(cast->getSubExpr()->getSourceRange());
    if (typeText.empty() || exprText.empty())
      return {};
    return getCastName(kind).str() + "<" + typeText + ">(" + exprText + ")";
  }

private:
  clang::ASTContext &Context;

  std::string getTypeTextFromCast(const clang::CStyleCastExpr *cast) const {
    clang::SourceRange typeRange =
        cast->getTypeInfoAsWritten()->getTypeLoc().getSourceRange();
    return getSourceText(typeRange);
  }

  std::string getSourceText(clang::SourceRange range) const {
    if (range.isInvalid())
      return {};
    clang::SourceManager &SM = Context.getSourceManager();
    clang::LangOptions LO = Context.getLangOpts();
    return clang::Lexer::getSourceText(
               clang::CharSourceRange::getTokenRange(range), SM, LO)
        .str();
  }
};

class CastRewriterVisitor
    : public clang::RecursiveASTVisitor<CastRewriterVisitor> {
public:
  CastRewriterVisitor(clang::ASTContext &ctx, clang::Rewriter &rewriter)
      : Context(ctx), Rewriter(rewriter), Classifier(ctx), TextBuilder(ctx) {}

  bool VisitCStyleCastExpr(clang::CStyleCastExpr *cast) {
    if (!cast)
      return true;
    if (!shouldProcess(cast))
      return true;
    CastKind kind = Classifier.choose(cast);
    std::string replacement = TextBuilder.build(cast, kind);
    if (replacement.empty())
      return true;
    clang::CharSourceRange range =
        clang::CharSourceRange::getTokenRange(cast->getSourceRange());
    Rewriter.ReplaceText(range, replacement);
    return true;
  }

private:
  clang::ASTContext &Context;
  clang::Rewriter &Rewriter;
  CastClassifier Classifier;
  CastTextBuilder TextBuilder;

  bool shouldProcess(const clang::CStyleCastExpr *cast) const {
    clang::SourceManager &SM = Context.getSourceManager();
    clang::SourceLocation loc = cast->getBeginLoc();
    if (loc.isInvalid())
      return false;
    if (loc.isMacroID())
      return false;
    if (SM.isInSystemHeader(loc))
      return false;
    return SM.isWrittenInMainFile(SM.getSpellingLoc(loc));
  }
};

class CastRewriterConsumer : public clang::ASTConsumer {
public:
  CastRewriterConsumer(clang::ASTContext &ctx, clang::Rewriter &rewriter)
      : Visitor(ctx, rewriter) {}

  void HandleTranslationUnit(clang::ASTContext &ctx) override {
    Visitor.TraverseDecl(ctx.getTranslationUnitDecl());
  }

private:
  CastRewriterVisitor Visitor;
};

class CastRewriterAction : public clang::PluginASTAction {
public:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef) override {
    Rewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return std::make_unique<CastRewriterConsumer>(CI.getASTContext(), Rewriter);
  }

  bool ParseArgs(const clang::CompilerInstance &,
                 const std::vector<std::string> &) override {
    return true;
  }

  void EndSourceFileAction() override {
    clang::SourceManager &SM = Rewriter.getSourceMgr();
    Rewriter.getEditBuffer(SM.getMainFileID()).write(llvm::outs());
  }

private:
  clang::Rewriter Rewriter;
};

} // namespace

static clang::FrontendPluginRegistry::Add<CastRewriterAction>
    X("cast_rewriter_mityaeva",
      "Rewrite C-style casts to C++ casts (Mityaeva Darya)");