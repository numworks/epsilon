#ifndef ESCHER_VIEW_CONTROLLER_H
#define ESCHER_VIEW_CONTROLLER_H

#include <escher/i18n.h>
#include <escher/telemetry.h>
#include <ion/events.h>
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

#include <escher/responder.h>
#include <escher/view.h>
#include <stdint.h>

namespace Escher {

class ViewController : public Responder {
 public:
  /* TitlesDisplay is only used within StackViewController for now. It
   * modifies the stack headers display. */
  enum class TitlesDisplay : uint8_t {
    DisplayAllTitles = 0b11111111,
    /* NeverDisplayOwnHeader is a special value, which not only hides the
     * current title, but also does not add it to the stack headers. */
    NeverDisplayOwnTitle = 0b11111110,
    /* Hide all previous headers but the last one. */
    DisplayLastFourTitles = 0b00001111,
    DisplayLastThreeTitles = 0b00000111,
    DisplaySecondAndThirdToLast = 0b00000110,
    DisplayLastAndThirdToLast = 0b00000101,
    DisplayLastTwoTitles = 0b00000011,
    DisplaySecondToLast = 0b00000010,
    DisplayLastTitle = 0b00000001,
    DisplayNoTitle = 0b00000000
  };

  ViewController(Responder* parentResponder) : Responder(parentResponder) {}
  virtual const char* title() { return nullptr; }
  virtual View* view() = 0;
  virtual void initView() {}
  virtual void viewWillAppear();
  virtual void viewDidDisappear() {}
  virtual TitlesDisplay titlesDisplay() {
    return TitlesDisplay::DisplayAllTitles;
  }
  /* Use these two functions only if the controller is in a stack hierarchy */
  // Pushes the given controller onto the parent StackViewController responder
  virtual void stackOpenPage(ViewController* nextPage);
  // Pop the parent StackViewController responder on a Left event
  bool popFromStackViewControllerOnLeftEvent(Ion::Events::Event event);

 protected:
#if EPSILON_TELEMETRY
  virtual const char* telemetryId() const { return nullptr; }
  void telemetryReportEvent(const char* action, const char* label) const;
#else
  void telemetryReportEvent(const char* action, const char* label) const {}
#endif
};

}  // namespace Escher
#endif
