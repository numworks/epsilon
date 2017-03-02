#ifndef ESCHER_VIEW_CONTROLLER_H
#define ESCHER_VIEW_CONTROLLER_H

#include <kandinsky.h>
extern "C" {
#include <stdint.h>
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
  virtual View * view() = 0;
  virtual void viewWillAppear();
  virtual void viewWillDisappear();
};

#endif
