#ifndef ESCHER_EDITABLE_FIELD_H
#define ESCHER_EDITABLE_FIELD_H

#include <ion.h>
#include <escher/input_event_handler.h>
#include <ion/unicode/code_point.h>

class EditableField : public InputEventHandler {
public:
  using InputEventHandler::InputEventHandler;
  virtual bool isEditing() const = 0;
  virtual void setEditing(bool isEditing) = 0;
  virtual CodePoint XNTCodePoint(CodePoint defaultXNTCodePoint) = 0;
  virtual bool shouldFinishEditing(Ion::Events::Event event) = 0;
};

#endif
