#ifndef SOLVER_GUI_PAGE_CONTROLLER_H
#define SOLVER_GUI_PAGE_CONTROLLER_H

// TODO Hugo : Factorize with probability

#include <apps/shared/float_parameter_controller.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>

namespace Solver {

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
  void openPage(Escher::ViewController * nextPage) {
    stackViewController()->push(nextPage, Escher::StackViewController::Style::GrayGradation, stackTitleStyleStep());
  }
private:
  virtual int stackTitleStyleStep() const { return -1; }
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

class DoubleParameterPage : public PageController<Shared::FloatParameterController<double>> {
public:
  DoubleParameterPage(Escher::StackViewController * stackViewController) :
        PageController(stackViewController) {}
private:
  int stackTitleStyleStep() const override { return 1; }
};

}  // namespace Solver

#endif /* SOLVER_GUI_PAGE_CONTROLLER_H */
