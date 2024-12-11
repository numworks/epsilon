#ifndef SEQUENCE_CURVE_PARAMETER_CONTROLLER_H
#define SEQUENCE_CURVE_PARAMETER_CONTROLLER_H

#include <escher/chevron_view.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>

#include "cobweb_controller.h"
#include "go_to_parameter_controller.h"

namespace Sequence {

class GraphController;

class CurveParameterController
    : public Escher::ExplicitSelectableListViewController {
 public:
  CurveParameterController(GraphController* graphController,
                           CobwebController* cobwebController,
                           Shared::InteractiveCurveViewRange* graphRange,
                           Shared::CurveViewCursor* cursor);
  const char* title() const override;
  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event event) override;
  void setRecord(Ion::Storage::Record record);
  int numberOfRows() const override { return k_numberOfRows; };
  Escher::HighlightCell* cell(int row) override;

 private:
  constexpr static int k_numberOfRows = 3;
  GoToParameterController m_goToParameterController;
  Escher::MenuCell<Escher::MessageTextView, Escher::EmptyCellWidget,
                   Escher::ChevronView>
      m_goToCell;
  Escher::MenuCell<Escher::MessageTextView> m_sumCell;
  Escher::MenuCell<Escher::MessageTextView> m_cobwebCell;
  CobwebController* m_cobwebController;
  GraphController* m_graphController;
  Ion::Storage::Record m_record;
};

}  // namespace Sequence

#endif
