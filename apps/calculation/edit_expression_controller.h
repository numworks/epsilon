#ifndef CALCULATION_EDIT_EXPRESSION_CONTROLLER_H
#define CALCULATION_EDIT_EXPRESSION_CONTROLLER_H

#include <escher.h>
#include "editable_expression_view.h"
#include "../shared/text_field_delegate.h"
#include "../shared/editable_expression_view_delegate.h"
#include "history_controller.h"
#include "calculation_store.h"
#include "text_field.h"

namespace Calculation {
class HistoryController;

/* TODO: implement a split view */
class EditExpressionController : public DynamicViewController, public Shared::TextFieldDelegate, public Shared::EditableExpressionViewDelegate {
public:
  EditExpressionController(Responder * parentResponder, HistoryController * historyController, CalculationStore * calculationStore);
  void didBecomeFirstResponder() override;
  void viewDidDisappear() override;
  bool handleEvent(Ion::Events::Event event) override;
  const char * textBody();
  void insertTextBody(const char * text);

  /* TextFieldDelegate */
  bool textFieldDidReceiveEvent(::TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(::TextField * textField, const char * text, Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(::TextField * textField, const char * text) override;

  /* EditableExpressionViewDelegate */
  bool editableExpressionViewDidReceiveEvent(::EditableExpressionView * editableExpressionView, Ion::Events::Event event) override;
  bool editableExpressionViewDidFinishEditing(::EditableExpressionView * editableExpressionView, const char * text, Ion::Events::Event event) override;
  bool editableExpressionViewDidAbortEditing(::EditableExpressionView * editableExpressionView, const char * text) override;
  void editableExpressionViewDidChangeSize(::EditableExpressionView * editableExpressionView) override;

private:
  class ContentView : public View {
  public:
    ContentView(Responder * parentResponder, TableView * subview, TextFieldDelegate * textFieldDelegate, EditableExpressionViewDelegate * editableExpressionViewDelegate);
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
    void reload();
    TextField * textField() { return &m_textField; }
    EditableExpressionView * editableExpressionView() { return &m_editableExpressionView; }
    TableView * mainView()  { return m_mainView; }
    void drawRect(KDContext * ctx, KDRect rect) const override;
    bool editionIsInTextField() const;
    static constexpr int k_bufferLength = TextField::maxBufferSize();
  private:
    static constexpr KDCoordinate k_textFieldHeight = 32;
    static constexpr KDCoordinate k_leftMargin = 5;
    static constexpr KDCoordinate k_verticalMargin = 5;
    constexpr static int k_separatorThickness = 1;
    KDCoordinate inputViewHeight() const;
    KDCoordinate editableExpressionViewHeight() const;
    TableView * m_mainView;
    TextField m_textField;
    EditableExpressionView m_editableExpressionView;
    char m_textBody[k_bufferLength];
  };
  View * loadView() override;
  void unloadView(View * view) override;
  void reloadView();
  Shared::TextFieldDelegateApp * textFieldDelegateApp() override;
  Shared::TextFieldAndEditableExpressionViewDelegateApp * textFieldAndEditableExpressionViewDelegateApp() override;
  Poincare::ExpressionLayout * expressionLayout();
  HistoryController * m_historyController;
  CalculationStore * m_calculationStore;
};

}

#endif
