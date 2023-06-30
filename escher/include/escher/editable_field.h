#ifndef ESCHER_EDITABLE_FIELD_H
#define ESCHER_EDITABLE_FIELD_H

#include <ion.h>
#include <ion/events.h>
#include <ion/unicode/code_point.h>

namespace Escher {

/* TODO: improve classes hierarchy to share selection handling (and some other
 * features) between EditableField and TextInput. Refactor the following
 * classes: TextInput, TextArea, EditableField, LayoutField,
 * TextField, and their delegates! */

class EditableField {
 public:
  EditableField() {}
  virtual bool handleEventWithText(const char* text, bool indentation = false,
                                   bool forceCursorRightOfText = false) {
    return false;
  }
  virtual bool handleStoreEvent() { return false; }
  virtual bool handleBoxEvent(Ion::Events::Event event);
  virtual bool addXNTCodePoint(CodePoint defaultXNTCodePoint) = 0;
};

}  // namespace Escher

#endif
