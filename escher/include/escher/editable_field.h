#ifndef ESCHER_EDITABLE_FIELD_H
#define ESCHER_EDITABLE_FIELD_H

#include <ion.h>
#include <escher/input_event_handler.h>
#include <ion/unicode/code_point.h>

namespace Escher {

/* TODO: improve classes hierarchy to share selection handling (and some other
 * features) between EditableField and TextInput. Refactor the following classes:
 * InputEventHandler, TextInput, TextArea, EditableField, LayoutField,
 * TextField, and their delegates! */

class EditableField : public InputEventHandler {
public:
  EditableField(InputEventHandlerDelegate * inputEventHandlerdelegate) :
    InputEventHandler(inputEventHandlerdelegate),
    m_selectionIsXNT(false) {}
  virtual bool isEditing() const = 0;
  virtual void setEditing(bool isEditing) = 0;
  virtual bool addXNTCodePoint(CodePoint defaultXNTCodePoint) = 0;
  virtual bool shouldFinishEditing(Ion::Events::Event event) = 0;
protected:
  bool m_selectionIsXNT;
};

}

#endif
