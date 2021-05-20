#ifndef PAGE_CONTROLLER_H
#define PAGE_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>

using namespace Escher;

namespace Probability {

/*
 * This templatized class adds some utils function to handle the parentResponder as a
 * StackViewController. To enforce correct typing, the specialization class must require a
 * StackViewController * be passed as argument to the constructor.
 */
template <typename T>
class PageController : public T {
  using T::T;

public:
  StackViewController * stackViewController() {
    return static_cast<StackViewController *>(T::parentResponder());
  }
  /* Pushes the given controller onto the StackViewController */
  void openPage(ViewController * nextPage, bool subPage, KDColor textColor = KDColorWhite,
                KDColor backgroundColor = Palette::PurpleBright,
                KDColor separatorColor = Palette::PurpleBright) {
    StackViewController * stack = stackViewController();  // We need to keep a ref, otherwise
                                                          // parentResponder might become nullptr
    if (!subPage) {
      assert(stack->topViewController() == this);
      // TODO cleanup
      // stack->pop();
    }
    stack->push(nextPage, textColor, backgroundColor, separatorColor);
  }
};

/* A Page is a controller that requires a StackViewController as its parent
 * and can open subPages easily by pushing them to the StackViewController. */
class Page : public PageController<ViewController> {
public:
  Page(StackViewController * stackViewController) : PageController(stackViewController){};
};

/* Similar to a Page but for SelectableListViewController */
class SelectableListViewPage : public PageController<SelectableListViewController> {
public:
  SelectableListViewPage(StackViewController * stackViewController,
                         SelectableTableViewDelegate * tableDelegate = nullptr)
      : PageController(stackViewController, tableDelegate){};
};

}  // namespace Probability

#endif /* PAGE_CONTROLLER_H */
