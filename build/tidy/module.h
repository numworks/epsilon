#ifndef TIDY_MODULE_H
#define TIDY_MODULE_H

#include "reset_globals/check.h"
#include <clang-tidy/ClangTidyModule.h>
#include <clang-tidy/ClangTidyModuleRegistry.h>

namespace clang {
namespace tidy {
namespace epsilon {

class EpsilonModule : public ClangTidyModule {
 public:
  void addCheckFactories(ClangTidyCheckFactories &CheckFactories) override {
    CheckFactories.registerCheck<ResetGlobalsCheck>("epsilon-reset-globals");
  }
};

}
}
}

#endif
