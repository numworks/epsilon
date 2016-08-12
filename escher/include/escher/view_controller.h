#ifndef ESCHER_VIEW_CONTROLLER_H
#define ESCHER_VIEW_CONTROLLER_H

extern "C" {
#include <stdint.h>
#include <kandinsky.h>
}

/* ViewControllers are reponsible for
 *  - Building the view hierarchy
 *  - Handling user input
 */

#include <escher/view.h>
#include <escher/responder.h>

class ViewController : public Responder {
public:
  ViewController(Responder * parentResponder);
  virtual const char * title() const;
  virtual void handleKeyEvent(int key);
  virtual View * view() = 0;
};

#endif
