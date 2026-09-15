#include "clang/AST/ASTConsumer.h"
#include "clang/AST/Decl.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/Support/raw_ostream.h"

namespace {
class VarCounterVisitor : public clang::RecursiveASTVisitor<VarCounterVisitor> {
public:
  explicit VarCounterVisitor(clang::ASTContext * /*context*/)
      : globals(0), locals(0), statics(0), params(0) {}

  bool VisitVarDecl(clang::VarDecl *var) {
    if (clang::isa<clang::ParmVarDecl>(var))
      return true;

    // Ignore in-class declarations of static data members.
    // We count only the out-of-class definition to avoid double counting.
    if (var->isStaticDataMember() && !var->isThisDeclarationADefinition())
      return true;

    // File-scope declarations like `extern int X;` are not counted.
    if (var->isFileVarDecl() && !var->isThisDeclarationADefinition())
      return true;

    if (var->isFileVarDecl()) {
      if (var->getStorageClass() == clang::SC_Static)
        statics++;
      else
        globals++;
      return true;
    }

    if (var->isStaticLocal()) {
      const auto *func =
          llvm::dyn_cast_or_null<clang::FunctionDecl>(var->getDeclContext());
      if (func && llvm::isa<clang::CXXMethodDecl>(func))
        locals++;
      else
        statics++;
      return true;
    }

    if (var->isLocalVarDecl()) {
      locals++;
    }

    return true;
  }

  bool VisitParmVarDecl(clang::ParmVarDecl *parm) {
    const auto *func =
        llvm::dyn_cast_or_null<clang::FunctionDecl>(parm->getDeclContext());
    if (func && !llvm::isa<clang::CXXMethodDecl>(func) &&
        func->isThisDeclarationADefinition())
      params++;
    return true;
  }

  void printResults() {
    auto &out = llvm::outs();
    out << "Global variables: " << globals << "\n";
    out << "Local variables: " << locals << "\n";
    out << "Static variables: " << statics << "\n";
    out << "Function parameters: " << params << "\n";
    out << "Total: " << globals + locals + statics + params << "\n";
  }

private:
  size_t globals;
  size_t locals;
  size_t statics;
  size_t params;
};

class VarCounterConsumer : public clang::ASTConsumer {
public:
  explicit VarCounterConsumer(clang::ASTContext *context) : visitor(context) {}

  void HandleTranslationUnit(clang::ASTContext &context) override {
    visitor.TraverseDecl(context.getTranslationUnitDecl());
    visitor.printResults();
  }

private:
  VarCounterVisitor visitor;
};

class VarCounterAction : public clang::PluginASTAction {
public:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &ci, llvm::StringRef) override {
    return std::make_unique<VarCounterConsumer>(&ci.getASTContext());
  }

  bool ParseArgs(const clang::CompilerInstance &ci,
                 const std::vector<std::string> &args) override {
    return true;
  }
};
} // namespace

static clang::FrontendPluginRegistry::Add<VarCounterAction>
    X("var_counter_plugin", "counts different types of variables");
