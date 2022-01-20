#ifndef ESCHER_VIEW_CONTROLLER_H
#define ESCHER_VIEW_CONTROLLER_H

#include <kandinsky.h>
#include <escher/i18n.h>
#include <escher/telemetry.h>
extern "C" {
#include <stdint.h>
}

/* ViewControllers are responsible for
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
 *
 * Both methods are always called after setting a view and laying its subviews
 * out.
 *
 * The method initView is called before setting a View (or often sets itself)
 * and laying it out. */

#include <escher/view.h>
#include <escher/responder.h>

class ViewController : public Responder {
public:
  /* DisplayParameter is only used within StackViewController for now. It
   * modifies the stack headers display. */
  enum class DisplayParameter {
    Default,
    DoNotShowOwnTitle,
    /* With DoNotShowOwnTitle, the title of the ViewController is not added to
     * the stack headers. */
    WantsMaximumSpace
    /* With WantsMaximumSpace, no stack headers are displayed. */
  };

  ViewController(Responder * parentResponder) : Responder(parentResponder) {}
  virtual const char * title() { return nullptr; }
  virtual View * view() = 0;
  virtual void initView() {}
  virtual void viewWillAppear();
  virtual void viewDidDisappear() {}
  virtual DisplayParameter displayParameter() { return DisplayParameter::Default; }
protected:
  void telemetryReportEvent(const char * action, const char * label) const {}
};

#endif
