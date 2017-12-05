#ifndef ESCHER_RESPONDER_H
#define ESCHER_RESPONDER_H

#include <ion/events.h>

class App;
class Toolbox;

class Responder {
public:
  Responder(Responder * parentResponder) : m_parentResponder(parentResponder) {}
  virtual bool handleEvent(Ion::Events::Event event); // Default implementation does nothing
  virtual bool handleEventWithText(const char * text, bool indentation = false, bool forceCursorRightOfText = false) { return false; }
  virtual void didBecomeFirstResponder();
  virtual void willResignFirstResponder();
  virtual void didEnterResponderChain(Responder * previousFirstResponder);
  virtual void willExitResponderChain(Responder * nextFirstResponder);
  Responder * parentResponder() const {
    return m_parentResponder;
  }
  Responder * commonAncestorWith(Responder * responder);
  void setParentResponder(Responder * responder) {
    m_parentResponder = responder;
  }
  App * app();
  virtual Toolbox * toolbox() { return nullptr; }
private:
  Responder * m_parentResponder;
};

#endif
