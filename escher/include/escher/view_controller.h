#ifndef ESCHER_VIEW_CONTROLLER_H
#define ESCHER_VIEW_CONTROLLER_H

#include <kandinsky.h>
#include <escher/i18n.h>
extern "C" {
#include <stdint.h>
}

/* ViewControllers are reponsible for
 *  - Building the view hierarchy
 *  - Handling user input
 *
 * The methods viewWillAppear is called in the following order relatively to
 * Responder's methods -didBecomeFirstResponder and didEnterResponderChain:
 * - viewWillAppear
 * - didEnterResponderChain
 * - didBecomeFirstResponder
 * The methods viewWillDisappear is called in the following order relatively to
 * Responder's methods -willResignFirstResponder and willExitResponderChain:
 * - viewWillDisappear
 * - willExitResponderChain
 * - willResignFirstResponder
 * Both methods are always called after setting a view and layouting it
 * subviews. */

#include <escher/view.h>
#include <escher/responder.h>

class ViewController : public Responder {
public:
  ViewController(Responder * parentResponder);
  virtual const char * title();
  virtual View * view() = 0;
  virtual void loadView();
  virtual void unloadView();
  virtual void viewWillAppear();
  virtual void viewDidDisappear();
};

#endif
