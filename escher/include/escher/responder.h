#ifndef ESCHER_RESPONDER_H
#define ESCHER_RESPONDER_H

#include <ion/events.h>

namespace Escher {

class Responder {
 public:
  Responder(Responder* parentResponder) : m_parentResponder(parentResponder) {}
  // Default implementation does nothing
  virtual bool handleEvent(Ion::Events::Event event) { return false; };
  virtual void didBecomeFirstResponder() {}
  virtual void willResignFirstResponder() {}
  virtual void didEnterResponderChain(Responder* previousFirstResponder) {}
  virtual void willExitResponderChain(Responder* nextFirstResponder) {}

  enum class FirstResponderAlteration { WillSpoil, DidRestore };
  void modalViewAltersFirstResponder(FirstResponderAlteration alteration);

  Responder* parentResponder() const { return m_parentResponder; }
  bool hasAncestor(Responder* responder) const;
  Responder* commonAncestorWith(Responder* responder);
  void setParentResponder(Responder* responder) {
    m_parentResponder = responder;
  }

 private:
  bool privateHasAncestor(Responder* responder) const;
  virtual void privateModalViewAltersFirstResponder(
      FirstResponderAlteration alteration) {}

  Responder* m_parentResponder;
};

}  // namespace Escher
#endif
