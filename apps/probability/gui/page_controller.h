#ifndef PAGE_CONTROLLER_H
#define PAGE_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>

using namespace Escher;

namespace Probability {

/*
 * This templatized class adds some utils function to handle the parentResponder as a StackViewController.
 * To enforce correct typing, the specialization class must require a StackViewController * be passed
 * as argument to the constructor.
 */
template <typename T>
class PageController : public T {
  using T::T;

 public:
  StackViewController * stackViewController() { return static_cast<StackViewController *>(T::parentResponder()); }
  /* Pushes the given controller onto the StackViewController */
  void openPage(ViewController * nextPage, bool subPage, KDColor textColor = Palette::SubTab,
                KDColor backgroundColor = KDColorWhite, KDColor separatorColor = Palette::GrayBright) {
    StackViewController * stack =
        stackViewController();  // We need to keep a ref, otherwise parentResponder might become nullptr
    if (!subPage) {
      assert(stack->topViewController() == this);
      stack->pop();
    }
    stack->push(nextPage, textColor, backgroundColor, separatorColor);
  }
};

class Page : public PageController<ViewController> {
 public:
  Page(StackViewController * stackViewController) : PageController(stackViewController){};
};

class SelectableListViewPage : public PageController<SelectableListViewController> {
 public:
  SelectableListViewPage(StackViewController * stackViewController,
                         SelectableTableViewDelegate * tableDelegate = nullptr)
      : PageController(stackViewController, tableDelegate){};
};

}

#endif /* PAGE_CONTROLLER_H */
