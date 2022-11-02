#ifndef SEQUENCE_CURVE_PARAMETER_CONTROLLER_H
#define SEQUENCE_CURVE_PARAMETER_CONTROLLER_H

#include <escher/explicit_selectable_list_view_controller.h>
#include <apps/shared/with_record.h>
#include "go_to_parameter_controller.h"
#include "cobweb_controller.h"

namespace Sequence {

class GraphController;

class CurveParameterController : public Escher::ExplicitSelectableListViewController, public Shared::WithRecord {
public:
  CurveParameterController(Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, GraphController * graphController, CobwebController * cobwebController, Shared::InteractiveCurveViewRange * graphRange, Shared::CurveViewCursor * cursor);
  const char * title() override;
  TELEMETRY_ID("CurveParameter");
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  void setRecord(Ion::Storage::Record record) override;
  int numberOfRows() const override { return k_numberOfRows; };
  Escher::HighlightCell * cell(int index) override;
private:
  constexpr static int k_numberOfRows = 3;
  GoToParameterController m_goToParameterController;
  Escher::MessageTableCellWithChevron m_goToCell;
  Escher::MessageTableCell m_sumCell;
  Escher::MessageTableCell m_cobwebCell;
  CobwebController * m_cobwebController;
  GraphController * m_graphController;
};

}

#endif
