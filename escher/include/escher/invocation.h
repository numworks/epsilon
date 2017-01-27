#ifndef ESCHER_INVOCATION_H
#define ESCHER_INVOCATION_H

class Invocation {
public:
  typedef void (*Action)(void * context, void * sender);
  Invocation(Action a, void * c);
  void perform(void * sender);
private:
  Action m_action;
  void * m_context;
};

#endif

