#ifndef CALCULATION_EDIT_EXPRESSION_CONTROLLER_H
#define CALCULATION_EDIT_EXPRESSION_CONTROLLER_H

#include <escher.h>
#include <poincare/layout.h>
#include "expression_field.h"
#include "../shared/text_field_delegate.h"
#include "../shared/layout_field_delegate.h"
#include "history_controller.h"
#include "selectable_table_view.h"

namespace Calculation {

/* TODO: implement a split view */
class EditExpressionController : public ViewController, public Shared::TextFieldDelegate, public Shared::LayoutFieldDelegate {
public:
  EditExpressionController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, char * cacheBuffer, size_t * cacheBufferInformation, HistoryController * historyController, CalculationStore * calculationStore);

  /* k_layoutBufferMaxSize dictates the size under which the expression being
   * edited can be remembered when the user leaves Calculation. */
  static constexpr int k_layoutBufferMaxSize = 1024;
  /* k_cacheBufferSize is the size of the array to which m_cacheBuffer points.
   * It is used both as a way to buffer expression when pushing them the
   * CalculationStore, and as a storage for the current input when leaving the
   * application. */
  static constexpr int k_cacheBufferSize = (k_layoutBufferMaxSize < Constant::MaxSerializedExpressionSize) ? Constant::MaxSerializedExpressionSize : k_layoutBufferMaxSize;

  View * view() override { return &m_contentView; }
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  void insertTextBody(const char * text);
  void restoreInput();
  void memoizeInput();
  bool handleEvent(Ion::Events::Event event) override;

  /* TextFieldDelegate */
  bool textFieldDidReceiveEvent(::TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(::TextField * textField, const char * text, Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(::TextField * textField) override;

  /* LayoutFieldDelegate */
  bool layoutFieldDidReceiveEvent(::LayoutField * layoutField, Ion::Events::Event event) override;
  bool layoutFieldDidFinishEditing(::LayoutField * layoutField, Poincare::Layout layoutR, Ion::Events::Event event) override;
  bool layoutFieldDidAbortEditing(::LayoutField * layoutField) override;
  void layoutFieldDidChangeSize(::LayoutField * layoutField) override;

private:
  class ContentView : public View {
  public:
    ContentView(Responder * parentResponder, CalculationSelectableTableView * subview, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * textFieldDelegate, LayoutFieldDelegate * layoutFieldDelegate);
    void reload();
    CalculationSelectableTableView * mainView() { return m_mainView; }
    ExpressionField * expressionField() { return &m_expressionField; }
  private:
    int numberOfSubviews() const override { return 2; }
    View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    CalculationSelectableTableView * m_mainView;
    ExpressionField m_expressionField;
  };
  void reloadView();
  void clearCacheBuffer() { m_cacheBuffer[0] = 0; *m_cacheBufferInformation = 0; }
  bool inputViewDidReceiveEvent(Ion::Events::Event event, bool shouldDuplicateLastCalculation);
  bool inputViewDidFinishEditing(const char * text, Poincare::Layout layoutR);
  bool inputViewDidAbortEditing(const char * text);
  char * m_cacheBuffer;
  size_t * m_cacheBufferInformation;
  HistoryController * m_historyController;
  CalculationStore * m_calculationStore;
  ContentView m_contentView;
};

}

#endif
