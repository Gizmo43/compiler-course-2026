#include "clang/AST/ASTConsumer.h"
#include "clang/AST/Decl.h"
#include "clang/AST/Expr.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

namespace {

const Expr *IgnoreWrappers(const Expr *expr) {
  if (!expr)
    return nullptr;
  return expr->IgnoreParenImpCasts();
}

bool IsDeclRefToVar(const Expr *expr, const VarDecl *varDecl) {
  if (!expr || !varDecl)
    return false;
  expr = IgnoreWrappers(expr);
  if (!expr)
    return false;

  const auto *declRef = dyn_cast<DeclRefExpr>(expr);
  if (!declRef)
    return false;

  const Decl *decl = declRef->getDecl();
  if (!decl)
    return false;

  const auto *vd = dyn_cast<VarDecl>(decl);
  if (!vd)
    return false;

  return vd == varDecl;
}

bool IsTargetObjectExpr(const Expr *expr, const VarDecl *varDecl) {
  if (!expr || !varDecl)
    return false;
  expr = IgnoreWrappers(expr);
  if (!expr)
    return false;

  if (IsDeclRefToVar(expr, varDecl)) {
    return true;
  }

  if (const auto *unaryOp = dyn_cast<UnaryOperator>(expr)) {
    if (unaryOp->getOpcode() == UO_Deref) {
      return IsDeclRefToVar(unaryOp->getSubExpr(), varDecl);
    }
  }

  if (const auto *memberExpr = dyn_cast<MemberExpr>(expr)) {
    return IsTargetObjectExpr(memberExpr->getBase(), varDecl);
  }

  if (const auto *arraySubscript = dyn_cast<ArraySubscriptExpr>(expr)) {
    return IsTargetObjectExpr(arraySubscript->getBase(), varDecl);
  }

  return false;
}

class MutationAnalyzer : public RecursiveASTVisitor<MutationAnalyzer> {
public:
  MutationAnalyzer(const VarDecl *target) : target_(target) {}

  void analyze(const FunctionDecl *func) {
    if (func && func->hasBody()) {
      TraverseStmt(func->getBody());
    }
  }

  bool IsVariableModified() const { return variableModified_; }
  bool IsObjectModified() const { return objectModified_; }

  bool VisitBinaryOperator(BinaryOperator *BO) {
    if (!BO->isAssignmentOp())
      return true;

    const Expr *lhs = BO->getLHS();
    if (!lhs)
      return true;

    if (IsDeclRefToVar(lhs, target_)) {
      variableModified_ = true;
    }
    if (IsTargetObjectExpr(lhs, target_)) {
      objectModified_ = true;
    }
    return true;
  }

  bool VisitUnaryOperator(UnaryOperator *UO) {
    if (!UO->isIncrementDecrementOp())
      return true;

    const Expr *subExpr = UO->getSubExpr();
    if (!subExpr)
      return true;

    if (IsDeclRefToVar(subExpr, target_)) {
      variableModified_ = true;
    }
    if (IsTargetObjectExpr(subExpr, target_)) {
      objectModified_ = true;
    }
    return true;
  }

  bool VisitCallExpr(CallExpr *CE) {
    const FunctionDecl *callee = CE->getDirectCallee();
    if (!callee)
      return true;

    unsigned numArgs = CE->getNumArgs();
    unsigned numParams = callee->getNumParams();
    unsigned count = numArgs < numParams ? numArgs : numParams;

    for (unsigned i = 0; i < count; ++i) {
      const Expr *arg = CE->getArg(i);
      if (!arg)
        continue;

      const ParmVarDecl *param = callee->getParamDecl(i);
      if (!param)
        continue;

      QualType paramType = param->getType();
      if (paramType.isNull())
        continue;

      bool isNonConstRef = paramType->isReferenceType() &&
                           !paramType.getNonReferenceType().isConstQualified();
      bool isNonConstPtr = paramType->isPointerType() &&
                           !paramType->getPointeeType().isConstQualified();

      if (IsDeclRefToVar(arg, target_)) {
        if (isNonConstRef || isNonConstPtr) {
          variableModified_ = true;
        }
      }
      if (IsTargetObjectExpr(arg, target_)) {
        if (isNonConstRef || isNonConstPtr) {
          objectModified_ = true;
        }
      }
    }
    return true;
  }

private:
  const VarDecl *target_;
  bool variableModified_ = false;
  bool objectModified_ = false;
};

class ExampleVisitor final : public RecursiveASTVisitor<ExampleVisitor> {
public:
  explicit ExampleVisitor(ASTContext *context)
      : m_context(context),
        m_rewriter(context->getSourceManager(), context->getLangOpts()) {}

  bool VisitVarDecl(VarDecl *varDecl) {
    if (!varDecl->isLocalVarDecl() && !isa<ParmVarDecl>(varDecl))
      return true;

    if (m_context->getSourceManager().isInSystemHeader(varDecl->getLocation()))
      return true;

    QualType type = varDecl->getType();
    bool isRef = type->isReferenceType();
    bool isPtr = type->isPointerType();

    if (!isRef && !isPtr)
      return true;

    if (type.isConstQualified())
      return true;
    if (isRef && type.getNonReferenceType().isConstQualified())
      return true;

    const FunctionDecl *funcDecl = getParentFunction(varDecl);
    if (!funcDecl || !funcDecl->hasBody())
      return true;

    MutationAnalyzer analyzer(varDecl);
    analyzer.analyze(funcDecl);

    if (isRef) {
      if (!analyzer.IsObjectModified()) {
        insertConstBeforeType(varDecl);
      }
    } else if (isPtr) {
      bool pointeeConst = type->getPointeeType().isConstQualified();
      bool pointerConst = type.isConstQualified();

      if (!analyzer.IsObjectModified() && !pointeeConst) {
        insertConstBeforeType(varDecl);
      }
      if (!analyzer.IsVariableModified() && !pointerConst) {
        insertConstAfterType(varDecl);
      }
    }

    return true;
  }

  void outputResult() {
    FileID mainFile = m_context->getSourceManager().getMainFileID();
    if (const auto *buffer = m_rewriter.getRewriteBufferFor(mainFile)) {
      llvm::outs() << std::string(buffer->begin(), buffer->end());
    } else {
      bool invalid = false;
      StringRef original =
          m_context->getSourceManager().getBufferData(mainFile, &invalid);
      if (!invalid) {
        llvm::outs() << original;
      }
    }
  }

private:
  const FunctionDecl *getParentFunction(const VarDecl *varDecl) {
    const DeclContext *context = varDecl->getDeclContext();
    while (context) {
      if (const auto *funcDecl = dyn_cast<FunctionDecl>(context)) {
        return funcDecl;
      }
      context = context->getParent();
    }
    return nullptr;
  }

  void insertConstBeforeType(const VarDecl *varDecl) {
    TypeSourceInfo *TSI = varDecl->getTypeSourceInfo();
    if (!TSI)
      return;

    SourceLocation loc = TSI->getTypeLoc().getBeginLoc();
    if (loc.isInvalid())
      return;

    m_rewriter.InsertTextBefore(loc, "const ");
  }

  void insertConstAfterType(const VarDecl *varDecl) {
    SourceLocation loc = varDecl->getLocation();
    if (loc.isInvalid())
      return;

    m_rewriter.InsertTextBefore(loc, "const ");
  }

  ASTContext *m_context;
  Rewriter m_rewriter;
};

class ExampleConsumer final : public ASTConsumer {
public:
  void HandleTranslationUnit(ASTContext &context) override {
    ExampleVisitor visitor(&context);
    visitor.TraverseDecl(context.getTranslationUnitDecl());
    visitor.outputResult();
  }
};

class ExampleAction final : public PluginASTAction {
public:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override {
    return std::make_unique<ExampleConsumer>();
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    return true;
  }

  ActionType getActionType() override { return ReplaceAction; }
};

} // namespace

static FrontendPluginRegistry::Add<ExampleAction>
    X("nikitin_a_lab_1_plugin",
      "Add const to unmodified pointers and references");