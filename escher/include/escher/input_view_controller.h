#ifndef ESCHER_INPUT_VIEW_CONTROLLER_H
#define ESCHER_INPUT_VIEW_CONTROLLER_H

#include <escher/editable_expression_view.h>
#include <escher/scrollable_expression_view_with_cursor_delegate.h>
#include <escher/modal_view_controller.h>
#include <escher/invocation.h>
#include <escher/text_field.h>
#include <escher/text_field_delegate.h>

/* TODO Implement a split view. Because we use a modal view, the main view is
 * redrawn underneath the modal view, which is visible and ugly. */

class InputViewController : public ModalViewController, TextFieldDelegate, ScrollableExpressionViewWithCursorDelegate {
public:
  InputViewController(Responder * parentResponder, ViewController * child, TextFieldDelegate * textFieldDelegate, ScrollableExpressionViewWithCursorDelegate * scrollableExpressionViewWithCursorDelegate);
  void edit(Responder * caller, Ion::Events::Event event, void * context, const char * initialText, Invocation::Action successAction, Invocation::Action failureAction);
  const char * textBody();
  void abortEditionAndDismiss();

  /* TextFieldDelegate */
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(TextField * textField) override;
  Toolbox * toolboxForTextInput(TextInput * textInput) override;

  /* ScrollableExpressionViewWithCursorDelegate */
  bool scrollableExpressionViewWithCursorShouldFinishEditing(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor, Ion::Events::Event event) override;
  bool scrollableExpressionViewWithCursorDidReceiveEvent(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor, Ion::Events::Event event) override;
  bool scrollableExpressionViewWithCursorDidFinishEditing(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor, const char * text, Ion::Events::Event event) override;
  bool scrollableExpressionViewWithCursorDidAbortEditing(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor) override;
  void scrollableExpressionViewWithCursorDidChangeSize(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor) override;
  Toolbox * toolboxForScrollableExpressionViewWithCursor(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor) override;

private:
  class EditableExpressionViewController : public ViewController {
  public:
    EditableExpressionViewController(Responder * parentResponder, TextFieldDelegate * textFieldDelegate, ScrollableExpressionViewWithCursorDelegate * scrollableExpressionViewWithCursorDelegate);
    void didBecomeFirstResponder() override;
    View * view() override { return &m_editableExpressionView; }
    EditableExpressionView * editableExpressionView() { return &m_editableExpressionView; }
  private:
    EditableExpressionView m_editableExpressionView;
  };
  bool inputViewDidFinishEditing();
  bool inputViewDidAbortEditing();
  EditableExpressionViewController m_editableExpressionViewController;
  Invocation m_successAction;
  Invocation m_failureAction;
  TextFieldDelegate * m_textFieldDelegate;
  ScrollableExpressionViewWithCursorDelegate * m_scrollableExpressionViewWithCursorDelegate;
  bool m_inputViewHeightIsMaximal;
};

#endif
