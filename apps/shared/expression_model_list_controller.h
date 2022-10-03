#ifndef SHARED_EXPRESSION_MODEL_LIST_CONTROLLER_H
#define SHARED_EXPRESSION_MODEL_LIST_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/even_odd_message_text_cell.h>
#include <escher/input_view_controller.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/selectable_table_view_delegate.h>
#include "expression_model_store.h"

namespace Shared {

class ExpressionModelListController : public Escher::ViewController, public Escher::SelectableTableViewDataSource, public Escher::SelectableTableViewDelegate {
public:
  ExpressionModelListController(Escher::Responder * parentResponder, I18n::Message text);
  virtual void editExpression(Ion::Events::Event event);
  virtual bool editSelectedRecordWithText(const char * text);
protected:
  constexpr static KDFont::Size k_font = KDFont::Size::Large;
  constexpr static KDCoordinate k_expressionMargin = 5;
  // TableViewDataSource
  virtual int numberOfExpressionRows() const;
  bool isAddEmptyRow(int j);
  virtual KDCoordinate expressionRowHeight(int j);
  virtual void willDisplayExpressionCellAtIndex(Escher::HighlightCell * cell, int j);
  // Responder
  bool handleEventOnExpression(Ion::Events::Event event);
  virtual void addModel();
  virtual void didChangeModelsList() { resetSizesMemoization(); }
  virtual bool removeModelRow(Ion::Storage::Record record);
  virtual int modelIndexForRow(int j) { return j; }
  // ViewController
  virtual Escher::SelectableTableView * selectableTableView() = 0;
  virtual ExpressionModelStore * modelStore() = 0;
  virtual Escher::InputViewController * inputController() = 0;
  Escher::EvenOddMessageTextCell m_addNewModel;
  // Memoization
  virtual void resetSizesMemoization() {}
private:
  bool addEmptyModel();

};

}

#endif
