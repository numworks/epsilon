#ifndef ESCHER_INVOCATION_H
#define ESCHER_INVOCATION_H

class Invocation {
public:
  typedef bool (*Action)(void * context, void * sender);
  Invocation(Action a, void * c);
  bool perform(void * sender);
private:
  Action m_action;
  void * m_context;
};

#endif

