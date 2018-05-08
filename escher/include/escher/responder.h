#ifndef ESCHER_RESPONDER_H
#define ESCHER_RESPONDER_H

#include <ion.h>

class App;
class Toolbox;

class Responder {
public:
  Responder(Responder * parentResponder);
  virtual bool handleEvent(Ion::Events::Event event); // Default implementation does nothing
  virtual void didBecomeFirstResponder();
  virtual void willResignFirstResponder();
  virtual void didEnterResponderChain(Responder * previousFirstResponder);
  virtual void willExitResponderChain(Responder * nextFirstResponder);
  Responder * parentResponder() const;
  Responder * commonAncestorWith(Responder * responder);
  void setParentResponder(Responder * responder);
  App * app();
  virtual Toolbox * toolbox() { return nullptr; }
private:
  Responder * m_parentResponder;
};

#endif
