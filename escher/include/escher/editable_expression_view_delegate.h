#ifndef ESCHER_EDITABLE_EXPRESSION_VIEW_DELEGATE_H
#define ESCHER_EDITABLE_EXPRESSION_VIEW_DELEGATE_H

#include <escher/toolbox.h>
#include <ion/events.h>

class EditableExpressionView;

class EditableExpressionViewDelegate {
public:
  virtual bool editableExpressionViewShouldFinishEditing(EditableExpressionView * editableExpressionView, Ion::Events::Event event) = 0;
  virtual bool editableExpressionViewDidReceiveEvent(EditableExpressionView * editableExpressionView, Ion::Events::Event event) = 0;
  /*virtual bool editableExpressionViewDidFinishEditing(EditableExpressionView * editableExpressionView, const char * text, Ion::Events::Event event) { return false; };
  virtual bool editableExpressionViewDidAbortEditing(EditableExpressionView * editableExpressionView, const char * text) {return false;};
  virtual bool editableExpressionViewDidHandleEvent(EditableExpressionView * editableExpressionView, Ion::Events::Event event, bool returnValue, bool textHasChanged) { return returnValue; };*/ // TODO?
  virtual Toolbox * toolboxForEditableExpressionView(EditableExpressionView * editableExpressionView) = 0;
};

#endif
