#ifndef SHARED_SINGLE_RANGE_CONTROLLER_H
#define SHARED_SINGLE_RANGE_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/menu_cell_with_editable_text.h>
#include <escher/switch_view.h>
#include <poincare/range.h>

#include "float_parameter_controller.h"
#include "pop_up_controller.h"

namespace Shared {

class SingleRangeController : public FloatParameterController<float> {
 public:
  SingleRangeController(Escher::Responder* parentResponder,
                        MessagePopUpController* confirmPopUpController);
  void viewWillAppear() override;

  int numberOfRows() const override { return k_numberOfTextCells + 2; }
  int typeAtRow(int row) const override {
    return row == 0 ? k_autoCellType
                    : FloatParameterController<float>::typeAtRow(row);
  }
  int reusableCellCount(int type) override {
    return type == k_autoCellType
               ? 1
               : FloatParameterController<float>::reusableCellCount(type);
  }
  Escher::HighlightCell* reusableCell(int index, int type) override;
  KDCoordinate nonMemoizedRowHeight(int row) override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;

  bool handleEvent(Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 Ion::Events::Event event) override;

 protected:
  constexpr static int k_numberOfTextCells = 2;
  constexpr static int k_autoCellType = 2;
  static_assert(k_autoCellType !=
                        FloatParameterController<float>::k_parameterCellType &&
                    k_autoCellType !=
                        FloatParameterController<float>::k_buttonCellType,
                "k_autoCellType value already taken.");

  virtual I18n::Message parameterMessage(int index) const = 0;
  virtual bool parametersAreDifferent() = 0;
  virtual void extractParameters() = 0;
  float parameterAtIndex(int index) override;
  void setAutoStatus(bool autoParam);
  virtual void setAutoRange() = 0;
  bool setParameterAtIndex(int parameterIndex, float f) override;
  void setRange(float min, float max);
  virtual float limit() const = 0;
  virtual void confirmParameters() = 0;
  virtual void pop(bool onConfirmation) = 0;

  Escher::MenuCellWithEditableText<Escher::MessageTextView>
      m_boundsCells[k_numberOfTextCells];
  Poincare::Range1D m_rangeParam;
  bool m_autoParam;

 private:
  int reusableParameterCellCount(int type) override {
    return k_numberOfTextCells;
  }
  Escher::HighlightCell* reusableParameterCell(int index, int type) override;
  Escher::TextField* textFieldOfCellAtIndex(Escher::HighlightCell* cell,
                                            int index) override;
  void buttonAction() override;

  Escher::MenuCell<Escher::MessageTextView, Escher::EmptyCellWidget,
                   Escher::SwitchView>
      m_autoCell;
  Shared::MessagePopUpController* m_confirmPopUpController;
};

}  // namespace Shared

#endif
