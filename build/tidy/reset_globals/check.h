#ifndef TIDY_RESET_GLOBALS_H
#define TIDY_RESET_GLOBALS_H

#include <clang-tidy/ClangTidyCheck.h>

namespace clang {
namespace tidy {
namespace epsilon {

/* This checks allows us to track which global variables have "state" and should
 * be reset when Epsilon is restarted. */

class ResetGlobalsCheck : public ClangTidyCheck {
 public:
  ResetGlobalsCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

 private:
  void diagnose(const VarDecl *decl, const char *message);
};

}  // namespace epsilon
}  // namespace tidy
}  // namespace clang

#endif
