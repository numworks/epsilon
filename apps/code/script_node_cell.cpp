#include "script_node_cell.h"
#include <kandinsky/point.h>
#include <escher.h>

namespace Code {

constexpr char ScriptNodeCell::k_parentheses[];
constexpr char ScriptNodeCell::k_parenthesesWithEmpty[];

void ScriptNodeCell::ScriptNodeView::drawRect(KDContext * ctx, KDRect rect) const {
  const KDColor backgroundColor = isHighlighted()? Palette::Select : KDColorWhite;

  // If it exists, draw the description name.
  const char * descriptionName = m_scriptNode->description();
  if (descriptionName != nullptr) {
    ctx->drawString(descriptionName, KDPoint(0, m_frame.height() - k_bottomMargin - k_font->glyphSize().height()), k_font, Palette::GrayDark, backgroundColor);
  }

  // Draw the node name
  const char * nodeName = m_scriptNode->name();
  const int nodeNameLength = m_scriptNode->nameLength();
  KDSize nameSize = k_font->stringSize(nodeName, nodeNameLength);
  const KDCoordinate nodeNameY = k_topMargin;
  ctx->drawString(nodeName, KDPoint(0, nodeNameY), k_font, KDColorBlack, backgroundColor, nodeNameLength);
  // If it is needed, draw the parentheses
  if (m_scriptNode->type() == ScriptNode::Type::WithParentheses) {
    ctx->drawString(ScriptNodeCell::k_parentheses, KDPoint(nameSize.width(), nodeNameY), k_font, KDColorBlack, backgroundColor);
  }

  /* If it exists, draw the source name. If it did not fit, we would have put
   * nullptr at the node creation. */
  const char * sourceName = m_scriptNode->nodeSourceName();
  if (sourceName != nullptr) {
    KDSize sourceNameSize = k_font->stringSize(sourceName);
    ctx->drawString(sourceName, KDPoint(m_frame.width() - sourceNameSize.width(), nodeNameY), k_font, Palette::GrayDark, backgroundColor);
  }
}

KDSize ScriptNodeCell::ScriptNodeView::minimalSizeForOptimalDisplay() const {
  if (m_scriptNode->name() == nullptr) {
    return KDSizeZero;
  }
  return KDSize(
      k_optimalWidth,
      m_scriptNode->description() == nullptr ? k_simpleItemHeight : k_complexItemHeight);
}

bool ScriptNodeCell::CanDisplayNameAndSource(int nameLength, const char * source) {
  if (source == nullptr) {
    return true;
  }
  assert(nameLength > 0);
  const KDFont * font = ScriptNodeView::k_font;
  return font->glyphSize().width()*(nameLength + 1) + font->stringSize(source).width() <= ScriptNodeView::k_optimalWidth; // + 1 for the separating space
}

void ScriptNodeCell::setScriptNode(ScriptNode * scriptNode) {
  m_scriptNodeView.setScriptNode(scriptNode);
  reloadCell();
}

void ScriptNodeCell::setHighlighted(bool highlight) {
  TableCell::setHighlighted(highlight);
  m_scriptNodeView.setHighlighted(highlight);
}

void ScriptNodeCell::reloadCell() {
  layoutSubviews();
  HighlightCell::reloadCell();
}

}
