#ifndef ESCHER_VIEW_CONTROLLER_H
#define ESCHER_VIEW_CONTROLLER_H

#include <escher/i18n.h>
#include <escher/telemetry.h>
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
 *
 * Both methods are always called after setting a view and laying its subwiews
 * out.
 *
 * The method initView is called before setting a View (or often sets itself)
 * and laying it out. */

#include <escher/view.h>
#include <escher/responder.h>
#include <stdint.h>

namespace Escher {

class ViewController : public Responder {
public:
  /* DisplayParameter is only used within StackViewController for now. It
   * modifies the stack headers display. */
  enum class DisplayParameter : uint8_t {
    Default = 0b11111111,
    /* With DoNotShowOwnTitle, the title of the ViewController is not added to
     * the stack headers. */
    DoNotShowOwnTitle = 0b11111110,
    /* Hide all previous headers but the last one. */
    OnlyShowLastTitle = 0b00000001,
    OnlyShowLastTwoTitles = 0b00000011,
    OnlyShowLastThreeTitles = 0b00000111,
    /* With WantsMaximumSpace, no stack headers are displayed. */
    WantsMaximumSpace = 0b00000000
  };

  ViewController(Responder * parentResponder) : Responder(parentResponder) {}
  virtual const char * title() { return nullptr; }
  virtual View * view() = 0;
  virtual void initView() {}
  virtual void viewWillAppear();
  virtual void viewDidDisappear() {}
  virtual DisplayParameter displayParameter() { return DisplayParameter::Default; }
protected:
#if EPSILON_TELEMETRY
  virtual const char * telemetryId() const { return nullptr; }
  void telemetryReportEvent(const char * action, const char * label) const;
#else
  void telemetryReportEvent(const char * action, const char * label) const {}
#endif
};

}
#endif
