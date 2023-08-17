#ifndef ESCHER_EDITABLE_FIELD_H
#define ESCHER_EDITABLE_FIELD_H

#include <escher/scrollable_view.h>
#include <escher/text_cursor_view.h>
#include <ion.h>
#include <ion/events.h>
#include <ion/unicode/code_point.h>

namespace Escher {

class EditableField : public TextCursorView::WithBlinkingCursor<
                          ScrollableView<ScrollView::NoDecorator>>,
                      public ScrollViewDataSource {
 public:
  EditableField(Responder* parentResponder, View* view)
      : TextCursorView::WithBlinkingCursor<ScrollableView>(parentResponder,
                                                           view, this) {}
  virtual bool handleEventWithText(const char* text, bool indentation = false,
                                   bool forceCursorRightOfText = false) {
    return false;
  }
  virtual bool handleStoreEvent() { return false; }
  virtual bool addXNTCodePoint(CodePoint defaultXNTCodePoint) = 0;

 protected:
  bool privateHandleBoxEvent(Ion::Events::Event event);
};

}  // namespace Escher

#endif
