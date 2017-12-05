#ifndef ESCHER_INVOCATION_H
#define ESCHER_INVOCATION_H

class Invocation final {
public:
  typedef void (*Action)(void * context, void * sender);
  Invocation(Action a, void * c) : m_action(a), m_context(c) {}
  void perform(void * sender) {
    (*m_action)(m_context, sender);
  }
private:
  Action m_action;
  void * m_context;
};

#endif

