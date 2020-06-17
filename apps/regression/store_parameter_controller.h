#ifndef REGRESSION_STORE_PARAMETER_CONTROLLER_H
#define REGRESSION_STORE_PARAMETER_CONTROLLER_H

#include "regression_controller.h"
#include "store.h"
#include "../shared/store_parameter_controller.h"

namespace Regression {

class StoreController;

class StoreParameterController : public Shared::StoreParameterController {
public:
  StoreParameterController(Responder * parentResponder, Store * store, StoreController * storeController);
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  // ListViewDataSource
  int numberOfRows() const override { return Shared::StoreParameterController::numberOfRows() + 1; }
  KDCoordinate rowHeight(int j) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
private:
  I18n::Message sortMessage() override { return I18n::Message::SortValues; }
  static constexpr int k_regressionCellType = 1;
  MessageTableCellWithChevronAndExpression m_changeRegressionCell;
  bool m_lastSelectionIsRegression;
};

}

#endif
