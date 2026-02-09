#include <poincare/init.h>
#include <poincare/src/memory/tree_stack.h>
#include <quiz.h>

#include "preferences.h"
#include "sequence_context.h"

namespace Poincare {

OMG::GlobalBox<MutablePreferences> MutablePreferences::s_instance;
OMG::GlobalBox<ForbiddenSequenceContext> ForbiddenSequenceContext::s_instance;

}  // namespace Poincare

namespace Quiz {

using namespace Poincare;

void Env::Init() {
  MutablePreferences::s_instance.init();
  ForbiddenSequenceContext::s_instance.init();
  Poincare::Init(MutablePreferences::s_instance, nullptr,
                 ForbiddenSequenceContext::s_instance);
}

void Env::Shutdown() {}

void Env::Flush() {
  Internal::SharedTreeStack->flush();

  MutablePreferences::s_instance.deinit();
  MutablePreferences::s_instance.init();
}

}  // namespace Quiz
