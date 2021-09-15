#ifndef PROBABILITY_GUI_PAGE_CONTROLLER_H
#define PROBABILITY_GUI_PAGE_CONTROLLER_H

#include <apps/shared/float_parameter_controller.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>

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
  Escher::StackViewController * stackViewController() {
    return static_cast<Escher::StackViewController *>(T::parentResponder());
  }
  /* Pushes the given controller onto the StackViewController */
  void openPage(Escher::ViewController * nextPage,
                KDColor backgroundColor = Escher::Palette::PurpleBright,
                KDColor separatorColor = Escher::Palette::PurpleBright,
                KDColor textColor = KDColorWhite) {
    stackViewController()->push(nextPage, textColor, backgroundColor, separatorColor);
  }
};

/* A Page is a controller that requires a StackViewController as its parent
 * and can open subPages easily by pushing them to the StackViewController. */
class Page : public PageController<Escher::ViewController> {
public:
  Page(Escher::StackViewController * stackViewController) : PageController(stackViewController) {}
};

/* Similar to a Page but for SelectableListViewController */
class SelectableListViewPage : public PageController<Escher::SelectableListViewController> {
public:
  SelectableListViewPage(Escher::StackViewController * stackViewController,
                         Escher::SelectableTableViewDelegate * tableDelegate = nullptr) :
        PageController(stackViewController, tableDelegate){};
};

class FloatParameterPage : public PageController<Shared::FloatParameterController<float>> {
public:
  FloatParameterPage(Escher::StackViewController * stackViewController) :
        PageController(stackViewController) {}
};

class DoubleParameterPage : public PageController<Shared::FloatParameterController<double>> {
public:
  DoubleParameterPage(Escher::StackViewController * stackViewController) :
        PageController(stackViewController) {}
};

}  // namespace Probability

#endif /* PROBABILITY_GUI_PAGE_CONTROLLER_H */
