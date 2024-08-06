#ifndef ESCHER_EDITABLE_FIELD_H
#define ESCHER_EDITABLE_FIELD_H

#include <escher/scrollable_view.h>
#include <escher/text_cursor_view.h>
#include <ion.h>
#include <ion/events.h>
#include <omg/code_point.h>
#include <poincare/layout.h>

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
  virtual bool handleEventWithLayout(Poincare::Layout layout,
                                     bool forceCursorRightOfText = false) {
    return false;
  }
  virtual bool handleStoreEvent() { return false; }
  bool handleXNT(int currentIndex, CodePoint startingXNT);

 protected:
  bool privateHandleBoxEvent(Ion::Events::Event event);
  virtual bool prepareToEdit() {
    assert(false);
    return false;
  }

 private:
  virtual bool findXNT(char* buffer, size_t bufferSize, int xntIndex,
                       size_t* cycleSize) {
    assert(false);
    return false;
  }
  virtual void removePreviousXNT() { assert(false); }
};

}  // namespace Escher

#endif
