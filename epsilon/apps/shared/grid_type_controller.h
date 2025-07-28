#ifndef SHARED_GRID_TYPE_CONTROLLER_H
#define SHARED_GRID_TYPE_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>
#include <escher/selectable_list_view_delegate.h>
#include <escher/stack_view_controller.h>
#include <escher/transparent_image_view.h>
namespace Shared {

class InteractiveCurveViewRange;

class GridTypeController
    : public Escher::UniformSelectableListController<
          Escher::MenuCell<Escher::MessageTextView, Escher::EmptyCellWidget,
                           Escher::TransparentImageView>,
          2> {
 public:
  enum class GridType { Cartesian, Polar };
  GridTypeController(Escher::Responder* parentResponder,
                     InteractiveCurveViewRange* interactiveCurveViewRange);

  void viewWillAppear() override;

  bool handleEvent(Ion::Events::Event event) override;
  Escher::StackViewController* stackController() {
    return static_cast<Escher::StackViewController*>(parentResponder());
  }

  const char* title() const override;

 private:
  InteractiveCurveViewRange* m_interactiveCurveViewRange;
};

}  // namespace Shared

#endif
