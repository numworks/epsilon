#include <poincare/init.h>
#include <poincare/src/memory/tree_stack.h>
#include <quiz.h>

namespace Quiz {

void Env::Init() { Poincare::Init(nullptr); }

void Env::Shutdown() {}

void Env::Flush() { Poincare::Internal::SharedTreeStack->flush(); }

}  // namespace Quiz
