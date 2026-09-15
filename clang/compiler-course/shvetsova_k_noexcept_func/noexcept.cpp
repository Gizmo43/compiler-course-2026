#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/raw_ostream.h"
#include <vector>

namespace {

class NoexceptScanner final
    : public clang::RecursiveASTVisitor<NoexceptScanner> {
public:
  NoexceptScanner(const clang::FunctionDecl *current,
                  const llvm::DenseMap<const clang::FunctionDecl *, bool> &safe)
      : m_current(current), m_safe(safe) {}

  bool VisitCXXThrowExpr(clang::CXXThrowExpr *) {
    m_noexcept = false;
    return false;
  }

  bool VisitCXXTryStmt(clang::CXXTryStmt *) {
    m_noexcept = false;
    return false;
  }

  bool VisitCXXConstructExpr(clang::CXXConstructExpr *E) {
    const auto *ctor = E->getConstructor();
    if (!ctor) {
      m_noexcept = false;
      return false;
    }
    if (ctor == m_current)
      return true;

    if (m_safe.lookup(ctor))
      return true;

    const auto *fpt = ctor->getType()->getAs<clang::FunctionProtoType>();
    if (fpt && isNoexceptProto(fpt))
      return true;
    if (ctor->isTrivial() || ctor->isDefaultConstructor())
      return true;

    m_noexcept = false;
    return false;
  }

  bool VisitCallExpr(clang::CallExpr *call) {
    const auto *callee = call->getDirectCallee();
    if (!callee) {
      m_noexcept = false;
      return false;
    }
    if (callee == m_current)
      return true;

    if (m_safe.lookup(callee))
      return true;

    const auto *fpt = callee->getType()->getAs<clang::FunctionProtoType>();
    if (fpt && isNoexceptProto(fpt))
      return true;

    m_noexcept = false;
    return false;
  }

  bool isNoexcept() const { return m_noexcept; }

private:
  bool isNoexceptProto(const clang::FunctionProtoType *fpt) const {
    return fpt->getExceptionSpecType() == clang::EST_BasicNoexcept ||
           fpt->getExceptionSpecType() == clang::EST_NoexceptTrue;
  }

  const clang::FunctionDecl *m_current;
  const llvm::DenseMap<const clang::FunctionDecl *, bool> &m_safe;
  bool m_noexcept = true;
};

class NoexceptVisitor final
    : public clang::RecursiveASTVisitor<NoexceptVisitor> {
public:
  explicit NoexceptVisitor(std::vector<clang::FunctionDecl *> &functions)
      : m_functions(functions) {}

  bool VisitFunctionDecl(clang::FunctionDecl *func) {
    if (func && func->hasBody() && !func->isImplicit())
      m_functions.push_back(func);
    return true;
  }

private:
  std::vector<clang::FunctionDecl *> &m_functions;
};

class NoexceptConsumer final : public clang::ASTConsumer {
public:
  NoexceptConsumer(clang::ASTContext &context)
      : m_context(context), m_visitor(m_functions) {}

  void HandleTranslationUnit(clang::ASTContext &context) override {
    m_functions.clear();
    m_safe.clear();
    m_visitor.TraverseDecl(context.getTranslationUnitDecl());

    for (auto *func : m_functions) {
      if (isAlreadyNoexcept(func))
        m_safe[func] = true;
    }

    bool changed = true;
    while (changed) {
      changed = false;
      for (auto *func : m_functions) {
        if (m_safe.lookup(func))
          continue;
        if (functionDoesNotThrow(func)) {
          m_safe[func] = true;
          updateToNoexcept(func);
          changed = true;
        }
      }
    }
    context.getTranslationUnitDecl()->dump(llvm::outs());
  }

private:
  bool isAlreadyNoexcept(const clang::FunctionDecl *func) const {
    const auto *fpt = func->getType()->getAs<clang::FunctionProtoType>();
    if (!fpt)
      return false;
    return fpt->getExceptionSpecType() == clang::EST_BasicNoexcept ||
           fpt->getExceptionSpecType() == clang::EST_NoexceptTrue;
  }

  bool functionDoesNotThrow(const clang::FunctionDecl *func) const {
    NoexceptScanner scanner(func, m_safe);
    scanner.TraverseStmt(const_cast<clang::Stmt *>(func->getBody()));
    return scanner.isNoexcept();
  }

  void updateToNoexcept(clang::FunctionDecl *func) {
    const auto *fpt = func->getType()->getAs<clang::FunctionProtoType>();
    if (!fpt)
      return;

    clang::FunctionProtoType::ExtProtoInfo epi = fpt->getExtProtoInfo();
    epi.ExceptionSpec.Type = clang::EST_BasicNoexcept;
    func->setType(m_context.getFunctionType(fpt->getReturnType(),
                                            fpt->getParamTypes(), epi));
  }

  clang::ASTContext &m_context;
  std::vector<clang::FunctionDecl *> m_functions;
  llvm::DenseMap<const clang::FunctionDecl *, bool> m_safe;
  NoexceptVisitor m_visitor;
};

class NoexceptAction final : public clang::PluginASTAction {
public:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &ci, llvm::StringRef) override {
    return std::make_unique<NoexceptConsumer>(ci.getASTContext());
  }
  bool ParseArgs(const clang::CompilerInstance &,
                 const std::vector<std::string> &) override {
    return true;
  }
};

} // namespace

static clang::FrontendPluginRegistry::Add<NoexceptAction> X("noexcept_plugin",
                                                            "Add noexcept");