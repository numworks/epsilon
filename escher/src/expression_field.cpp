#include <escher/expression_field.h>
#include <poincare/preferences.h>
#include <assert.h>
#include <algorithm>

using namespace Poincare;

namespace Escher {

ExpressionField::ExpressionField(Responder * parentResponder,
                                 InputEventHandlerDelegate * inputEventHandlerDelegate,
                                 LayoutFieldDelegate * layoutFieldDelegate,
                                 float horizontalAlignment,
                                 float verticalAlignment) :
    LayoutField(parentResponder, inputEventHandlerDelegate, layoutFieldDelegate, KDFont::Size::Large, horizontalAlignment, verticalAlignment),
    m_textField(parentResponder,
                nullptr,
                k_textFieldBufferSize,
                k_textFieldBufferSize,
                inputEventHandlerDelegate,
                nullptr,
                KDFont::Size::Large,
                horizontalAlignment,
                verticalAlignment,
                KDColorBlack,
                KDColorWhite),
    m_inputViewMemoizedHeight(0) {
  setBackgroundColor(KDColorWhite);
}

void ExpressionField::setEditing(bool isEditing, bool reinitDraftBuffer) {
  if (reinitDraftBuffer) {
    clearLayout();
  }
  LayoutField::setEditing(isEditing);
}

const char * ExpressionField::text() {
  layout().serializeForParsing(m_textField.draftTextBuffer(), k_textFieldBufferSize);
  return m_textField.draftTextBuffer();
}

void ExpressionField::setText(const char * text) {
  clearLayout();
  handleEventWithText(text, false, true);
}

void ExpressionField::didBecomeFirstResponder() {
  m_inputViewMemoizedHeight = inputViewHeight();
  LayoutField::didBecomeFirstResponder();
}

KDSize ExpressionField::minimalSizeForOptimalDisplay() const {
  return KDSize(LayoutField::minimalSizeForOptimalDisplay().width(), inputViewHeight());
}

bool ExpressionField::inputViewHeightDidChange() {
  KDCoordinate newHeight = inputViewHeight();
  bool didChange = m_inputViewMemoizedHeight != newHeight;
  m_inputViewMemoizedHeight = newHeight;
  return didChange;
}

void ExpressionField::reload() {
  // TODO reload only in non linearMode
  LayoutField::reload(KDSizeZero);
  // Currently used only for its baseline effect, useless on textField
}

KDCoordinate ExpressionField::inputViewHeight() const {
  return LayoutField::minimalSizeForOptimalDisplay().height();
}

void ExpressionField::restoreContent(const char * buffer, size_t size, int * cursorOffset, int * position) {
  if (size == 0) {
    return;
  }
  setLayout(Layout::LayoutFromAddress(buffer, size));
  if (*cursorOffset != -1) {
    const LayoutNode * cursorNode = reinterpret_cast<const LayoutNode *>(reinterpret_cast<char *>(layout().node()) + *cursorOffset);
    LayoutCursor restoredCursor = LayoutCursor(Layout(cursorNode));
    restoredCursor.setPosition(*position);
    *cursor() = restoredCursor;
  }
}

}
