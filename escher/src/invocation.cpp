#include <escher/invocation.h>

Invocation::Invocation(Action a, void * c) :
  m_action(a),
  m_context(c)
{
}

bool Invocation::perform(void * sender) {
  return (*m_action)(m_context, sender);
}
