#ifndef SHARED_SINGLE_RANGE_CONTROLLER_H
#define SHARED_SINGLE_RANGE_CONTROLLER_H

#include "float_parameter_controller.h"
#include "interactive_curve_view_range.h"
#include <escher/pop_up_controller.h>
#include <escher/message_table_cell_with_editable_text.h>
#include <escher/message_table_cell_with_switch.h>

namespace Shared {

class SingleRangeController : public FloatParameterController<float> {
public:
  SingleRangeController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewRange * interactiveCurveViewRange, Escher::MessagePopUpController * confirmPopUpController);

  const char * title() override { return I18n::translate(m_editXRange ? I18n::Message::ValuesOfX : I18n::Message::ValuesOfY); }
  void viewWillAppear() override;

  int numberOfRows() const override { return k_numberOfTextCells + 2; }
  int typeAtIndex(int index) override { return index == 0 ? k_autoCellType : FloatParameterController<float>::typeAtIndex(index); }
  int reusableCellCount(int type) override { return type == k_autoCellType ? 1 : FloatParameterController<float>::reusableCellCount(type); }
  Escher::HighlightCell * reusableCell(int index, int type) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;

  bool handleEvent(Ion::Events::Event event) override;

  bool editXRange() const { return m_editXRange; }
  void setEditXRange(bool editXRange);

private:
  constexpr static int k_numberOfTextCells = 2;
  constexpr static int k_autoCellType = 2;
  static_assert(k_autoCellType != SimpleFloatParameterController<float>::k_parameterCellType && k_autoCellType != FloatParameterController<float>::k_buttonCellType, "k_autoCellType value already taken.");

  class LockableEditableCell : public Escher::MessageTableCellWithEditableText {
  public:
    Escher::Responder * responder() override;
    void setController(SingleRangeController * controller) { m_controller = controller; }
  private:
    SingleRangeController * m_controller;
  };

  bool autoStatus() const { return m_autoParam; }
  float parameterAtIndex(int index) override;
  int reusableParameterCellCount(int type) override { return k_numberOfTextCells; }
  Escher::HighlightCell * reusableParameterCell(int index, int type) override;
  bool setParameterAtIndex(int parameterIndex, float f) override;
  void extractParameters();
  bool parametersAreDifferent();
  void confirmParameters();
  void buttonAction() override;

  Escher::MessageTableCellWithSwitch m_autoCell;
  LockableEditableCell m_boundsCells[k_numberOfTextCells];
  InteractiveCurveViewRange * m_range;
  Escher::MessagePopUpController * m_confirmPopUpController;
  Range1D m_rangeParam;
  // m_secondaryRangeParam is only used when activating xAuto while yAuto is on.
  Range1D m_secondaryRangeParam;
  bool m_autoParam;
  bool m_editXRange;
};

}

#endif
