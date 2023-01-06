#include "module.h"

static clang::tidy::ClangTidyModuleRegistry::Add<clang::tidy::epsilon::EpsilonModule> X("epsilon", "Adds Epsilon platform checks.");
