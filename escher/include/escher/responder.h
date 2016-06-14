#ifndef ESCHER_RESPONDER_H
#define ESCHER_RESPONDER_H

extern "C" {
#include <ion.h>
}

class Responder {
public:
  Responder();
  virtual bool handleEvent(ion_event_t event); // Default implementation does nothing
  virtual void setFocused(bool focused); // Default implementation does nothing. Used by subclasses to know when active or not
  Responder * parentResponder();
  void setParentResponder(Responder * responder);
private:
  Responder * m_parentResponder;
};

#endif
