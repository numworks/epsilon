#ifndef ESCHER_RESPONDER_H
#define ESCHER_RESPONDER_H

#include <ion/events.h>

namespace Escher {

class Responder {
public:
  Responder(Responder * parentResponder) : m_parentResponder(parentResponder) {}
  virtual bool handleEvent(Ion::Events::Event event) { return false; }; // Default implementation does nothing
  virtual void didBecomeFirstResponder() {}
  virtual void willResignFirstResponder() {}
  virtual void didEnterResponderChain(Responder * previousFirstResponder) {}
  virtual void willExitResponderChain(Responder * nextFirstResponder) {}
  Responder * parentResponder() const { return m_parentResponder; }
  bool hasAncestor(Responder * responder) const;
  Responder * commonAncestorWith(Responder * responder);
  void setParentResponder(Responder * responder) { m_parentResponder = responder; }
private:
  bool privateHasAncestor(Responder * responder) const;
  Responder * m_parentResponder;
};

}
#endif
