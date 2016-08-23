#ifndef ESCHER_RESPONDER_H
#define ESCHER_RESPONDER_H

#include <ion/events.h>

class App;

class Responder {
public:
  Responder(Responder * parentResponder);
  virtual bool handleEvent(Ion::Events::Event event); // Default implementation does nothing
  virtual void setFocused(bool focused); // Default implementation does nothing. Used by subclasses to know when active or not
  Responder * parentResponder() const;
  void setParentResponder(Responder * responder);
  App * app();
private:
  Responder * m_parentResponder;
};

#endif
