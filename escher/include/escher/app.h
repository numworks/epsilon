#ifndef ESCHER_APP_H
#define ESCHER_APP_H

#include <escher/responder.h>
#include <escher/view_controller.h>

class App {
public:
  void run();
  void focus(Responder * responder);
protected:
  virtual ViewController * rootViewController() = 0;
private:
  void dispatchEvent(ion_event_t event);
  Responder * m_focusedResponder;
};

/*

void handleEvent(event) {
  // BACK: I want to do stuff!
   1/ Reset my text to what it was "before"
   2/ dismiss
   dismiss = app->focus(parentResponder());
}
*/

#endif
