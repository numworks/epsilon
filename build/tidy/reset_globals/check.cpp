#include "check.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace epsilon {

namespace {
AST_MATCHER(VarDecl, isLocalVarDecl) { return Node.isLocalVarDecl(); }
AST_MATCHER(VarDecl, hasLinkage) { return Node.hasLinkage(); }
}  // namespace

void ResetGlobalsCheck::registerMatchers(MatchFinder* Finder) {
  Finder->addMatcher(
      varDecl(isDefinition(),  // Ignore forward-declared global variables (e.g.
                               // "extern int foobar;")
              hasGlobalStorage(), unless(hasType(isConstQualified())))
          .bind("bss_var"),
      this);
}

static inline bool isOMGGlobalBox(const VarDecl* decl) {
  const TemplateSpecializationType* templateSpecializationType =
      decl->getType()->getAs<TemplateSpecializationType>();
  if (!templateSpecializationType) {
    return false;
  }
  TemplateDecl* templateDecl =
      templateSpecializationType->getTemplateName().getAsTemplateDecl();
  if (!templateDecl) {
    return false;
  }
  return templateDecl->getName().equals("GlobalBox");
}

void ResetGlobalsCheck::check(const MatchFinder::MatchResult& Result) {
  if (const VarDecl* decl = Result.Nodes.getNodeAs<VarDecl>("bss_var")) {
    if (decl->isLocalVarDecl()) {
      diagnose(decl, "local variable with global storage cannot be reset");
    } else if (!isOMGGlobalBox(decl)) {
      diagnose(decl,
               "non-constant global variable not wrapped in OMG::GlobalBox");
    }
  }
}

void ResetGlobalsCheck::diagnose(const VarDecl* decl, const char* message) {
  diag(decl->getLocation(), message) << decl->getSourceRange();
}

}  // namespace epsilon
}  // namespace tidy
}  // namespace clang
