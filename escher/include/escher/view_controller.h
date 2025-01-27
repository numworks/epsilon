#ifndef ESCHER_VIEW_CONTROLLER_H
#define ESCHER_VIEW_CONTROLLER_H

#include <escher/i18n.h>
#include <ion/events.h>
extern "C" {
#include <stdint.h>
}

/* ViewControllers are reponsible for
 *  - Building the view hierarchy
 *  - Handling user input
 *
 * The methods viewWillAppear is called in the following order relatively to
 * Responder's methods -didBecomeFirstResponder and handleResponderChainEvent:
 * - viewWillAppear
 * - handleResponderChainEvent(didEnter)
 * - didBecomeFirstResponder
 * The methods viewWillDisappear is called in the following order relatively to
 * Responder's methods -willResignFirstResponder and handleResponderChainEvent:
 * - viewWillDisappear
 * - handleResponderChainEvent(wilExit)
 * - willResignFirstResponder
 *
 * Both methods are always called after setting a view and laying its subwiews
 * out.
 *
 * The method initView is called before setting a View (or often sets itself)
 * and laying it out. */

#include <escher/responder.h>
#include <escher/stack_view.h>
#include <escher/view.h>
#include <stdint.h>

namespace Escher {

class ViewController : public Responder {
 public:
  /* TitlesDisplay is only used within StackViewController for now. It
   * modifies the stack headers display. */
  enum class TitlesDisplay : StackView::Mask {
    DisplayAllTitles = StackView::Mask(~0u),  // 0b11111111
    /* NeverDisplayOwnHeader is a special value, which not only hides the
     * current title, but also does not add it to the stack headers. */
    NeverDisplayOwnTitle = StackView::Mask(~1u),  // 0b11111110
    /* Hide all previous headers but the last one. */
    DisplayLastFourTitles = StackView::Mask(0b1111),
    DisplayLastThreeTitles = StackView::Mask(0b111),
    DisplaySecondAndThirdToLast = StackView::Mask(0b110),
    DisplayLastAndThirdToLast = StackView::Mask(0b101),
    DisplayLastTwoTitles = StackView::Mask(0b11),
    DisplaySecondToLast = StackView::Mask(0b10),
    DisplayLastTitle = StackView::Mask(0b1),
    DisplayNoTitle = StackView::Mask(0),
    /* Special value to display the same titles as the previous page on the
     * stack. */
    SameAsPreviousPage = StackView::Mask(0b10000000)
  };

  ViewController(Responder* parentResponder) : Responder(parentResponder) {}
  virtual const char* title() const { return nullptr; }
  virtual View* view() = 0;
  virtual void initView() {}
  virtual void viewWillAppear() {}
  virtual void viewDidDisappear() {}
  virtual TitlesDisplay titlesDisplay() const {
    return TitlesDisplay::DisplayAllTitles;
  }
  /* Use these two functions only if the controller is in a stack hierarchy */
  // Pushes the given controller onto the parent StackViewController responder
  virtual void stackOpenPage(ViewController* nextPage);
  // Pop the parent StackViewController responder on a Left event
  bool popFromStackViewControllerOnLeftEvent(Ion::Events::Event event);
};

}  // namespace Escher
#endif
