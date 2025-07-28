#include <assert.h>
#include <escher/container.h>
#include <escher/even_odd_editable_text_cell.h>

namespace Escher {

void AbstractEvenOddEditableTextCell::
    updateSubviewsBackgroundAfterChangingState() {
  editableTextCell()->textField()->setBackgroundColor(backgroundColor());
}

int AbstractEvenOddEditableTextCell::numberOfSubviews() const { return 1; }

View* AbstractEvenOddEditableTextCell::subviewAtIndex(int index) {
  assert(index == 0);
  return editableTextCell();
}

void AbstractEvenOddEditableTextCell::layoutSubviews(bool force) {
  setChildFrame(editableTextCell(),
                KDRect(bounds().left(), bounds().top(),
                       bounds().width() - k_rightMargin, bounds().height()),
                force);
}

void AbstractEvenOddEditableTextCell::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    App::app()->setFirstResponder(editableTextCell());
  } else {
    Responder::handleResponderChainEvent(event);
  }
}

}  // namespace Escher
