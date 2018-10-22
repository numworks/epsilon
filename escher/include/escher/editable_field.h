#ifndef ESCHER_EDITABLE_FIELD_H
#define ESCHER_EDITABLE_FIELD_H

#include <ion.h>
#include <escher/input_event_handler.h>

class EditableField : public InputEventHandler {
public:
  using InputEventHandler::InputEventHandler;
  virtual bool isEditing() const = 0;
  virtual void setEditing(bool isEditing, bool reinitDraftBuffer = true) = 0;
  virtual char XNTChar(char defaultXNTChar) = 0;
  virtual bool shouldFinishEditing(Ion::Events::Event event) = 0;
};

#endif
