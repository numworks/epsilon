#include <assert.h>
#include <escher/container.h>
#include <escher/editable_text_cell.h>
#include <escher/palette.h>
#include <poincare/print_float.h>

namespace Escher {

void AbstractEditableTextCell::setHighlighted(bool highlight) {
  HighlightCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight ? Palette::Select : KDColorWhite;
  textField()->setBackgroundColor(backgroundColor);
}

const char* AbstractEditableTextCell::text() const {
  if (!const_cast<AbstractEditableTextCell*>(this)->textField()->isEditing()) {
    return const_cast<AbstractEditableTextCell*>(this)->textField()->text();
  }
  return nullptr;
}

int AbstractEditableTextCell::numberOfSubviews() const { return 1; }

View* AbstractEditableTextCell::subviewAtIndex(int index) {
  assert(index == 0);
  return textField();
}

void AbstractEditableTextCell::layoutSubviews(bool force) {
  KDRect cellBounds = bounds();
  setChildFrame(textField(), cellBounds, force);
}

void AbstractEditableTextCell::didBecomeFirstResponder() {
  App::app()->setFirstResponder(textField());
}

KDSize AbstractEditableTextCell::minimalSizeForOptimalDisplay() const {
  return const_cast<AbstractEditableTextCell*>(this)
      ->textField()
      ->minimalSizeForOptimalDisplay();
}

}  // namespace Escher
