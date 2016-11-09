#ifndef ESCHER_RESPONDER_H
#define ESCHER_RESPONDER_H

#include <ion.h>

class App;

class Responder {
public:
  Responder(Responder * parentResponder);
  virtual bool handleEvent(Ion::Events::Event event); // Default implementation does nothing
  virtual void didBecomeFirstResponder();
  virtual void didResignFirstResponder();

  Responder * parentResponder() const;
  void setParentResponder(Responder * responder);
  App * app();
private:
  Responder * m_parentResponder;
};

#endif
