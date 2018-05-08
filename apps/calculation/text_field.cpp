#include "text_field.h"

namespace Calculation {

TextField::TextField(Responder * parentResponder, char * textBuffer, size_t textBufferSize, TextFieldDelegate * delegate) :
  ::TextField(parentResponder, textBuffer, textBuffer, textBufferSize, delegate, false)
{
  setEditing(true);
}

bool TextField::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back) {
    return false;
  }
  if (event == Ion::Events::Ans) {
    return handleEventWithText("ans");
  }
  if (isEditing() && draftTextLength() == 0 &&
      (event == Ion::Events::Multiplication ||
       event == Ion::Events::Plus ||
       event == Ion::Events::Power ||
       event == Ion::Events::Square ||
       event == Ion::Events::Division ||
       event == Ion::Events::Sto)) {
    handleEventWithText("ans");
  }
  return(::TextField::handleEvent(event));
}

}
