#include <escher/expression_field.h>
#include <poincare/preferences.h>
#include <assert.h>
#include <algorithm>

using namespace Poincare;

namespace Escher {

static char s_draftBuffer[AbstractTextField::MaxBufferSize()];

ExpressionField::ExpressionField(Responder * parentResponder,
                                 InputEventHandlerDelegate * inputEventHandlerDelegate,
                                 LayoutFieldDelegate * layoutFieldDelegate,
                                 float horizontalAlignment,
                                 float verticalAlignment) :
    LayoutField(parentResponder, inputEventHandlerDelegate, layoutFieldDelegate, KDFont::Size::Large, horizontalAlignment, verticalAlignment),
    m_inputViewMemoizedHeight(0),
    m_draftBuffer(s_draftBuffer),
    m_draftBufferSize(AbstractTextField::MaxBufferSize())
{
  setBackgroundColor(KDColorWhite);
}

void ExpressionField::clearAndSetEditing(bool isEditing) {
  clearLayout();
  LayoutField::setEditing(isEditing);
}

const char * ExpressionField::text() {
  layout().serializeForParsing(m_draftBuffer, m_draftBufferSize);
  return m_draftBuffer;
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
  if (!linearMode()) {
    // Currently used only for its baseline effect, useless in linearMode
    LayoutField::reload(KDSizeZero);
  }
}

KDCoordinate ExpressionField::inputViewHeight() const {
  return std::max(k_minimalHeight, LayoutField::minimalSizeForOptimalDisplay().height());
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
