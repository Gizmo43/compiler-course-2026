#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/Support/raw_ostream.h"

namespace {

class ScopeScanner final : public clang::RecursiveASTVisitor<ScopeScanner> {
private:
  size_t GlobalVars = 0;
  size_t StaticVars = 0;
  size_t LocalVars = 0;
  size_t FuncParams = 0;

public:
  explicit ScopeScanner(clang::ASTContext *Ctx) {}

  bool shouldVisitTemplateInstantiations() const { return false; }

  bool VisitVarDecl(clang::VarDecl *VD) {

    if (VD->isThisDeclarationADefinition() != clang::VarDecl::Definition) {
      return true;
    }

    if (const auto *PVD = llvm::dyn_cast<clang::ParmVarDecl>(VD)) {

      if (const auto *FD =
              llvm::dyn_cast<clang::FunctionDecl>(PVD->getDeclContext())) {
        if (FD->isThisDeclarationADefinition()) {
          FuncParams++;
        }
      }
    } else if (VD->isStaticLocal() ||
               VD->getStorageClass() == clang::SC_Static) {
      StaticVars++;
    } else if (VD->isLocalVarDecl()) {
      LocalVars++;
    } else if (VD->hasGlobalStorage()) {
      GlobalVars++;
    }

    return true;
  }

  void ReportResults() const {
    size_t Total = GlobalVars + StaticVars + LocalVars + FuncParams;

    llvm::errs() << "--- Unit Statistics (Kutergin Anton) ---\n"
                 << "Total Declarations : " << Total << "\n"
                 << "Global scope       : " << GlobalVars << "\n"
                 << "Static storage     : " << StaticVars << "\n"
                 << "Local variables    : " << LocalVars << "\n"
                 << "Function parameters: " << FuncParams << "\n"
                 << "---------------------------------------\n";
  }
};

class TranslationUnitInspector final : public clang::ASTConsumer {
public:
  explicit TranslationUnitInspector(clang::ASTContext *Ctx) : Scanner(Ctx) {}

  void HandleTranslationUnit(clang::ASTContext &Ctx) override {
    Scanner.TraverseDecl(Ctx.getTranslationUnitDecl());
    Scanner.ReportResults();
  }

private:
  ScopeScanner Scanner;
};

class VarScopeAnalysisAction final : public clang::PluginASTAction {
public:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef) override {
    return std::make_unique<TranslationUnitInspector>(&CI.getASTContext());
  }

  bool ParseArgs(const clang::CompilerInstance &CI,
                 const std::vector<std::string> &Args) override {
    return true;
  }
};

} // namespace

static clang::FrontendPluginRegistry::Add<VarScopeAnalysisAction>
    X("var-scope-stats",
      "Analyzes and counts variables by their storage and scope");