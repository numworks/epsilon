#include "script_node_cell.h"
#include <algorithm>

using namespace Escher;

namespace Code {

ScriptNodeCell::ScriptNodeCell() :
  TableCell(),
  m_labelView(KDFont::Size::Large, KDContext::k_alignLeft, KDContext::k_alignCenter, KDColorBlack, KDColorWhite, k_maxNumberOfCharsInLabel),
  m_subLabelView(KDFont::Size::Small, KDContext::k_alignLeft, KDContext::k_alignCenter, Escher::Palette::GrayDark)
{}

void ScriptNodeCell::setScriptNode(ScriptNode * node) {
  /* Use a temporary buffer to crop label name, as strlen(node->name()) may be
   * greater than node->nameLength() */
  const size_t labelLength = std::min(node->nameLength(), k_maxNumberOfCharsInLabel);
  char temp_buffer[k_maxNumberOfCharsInLabel + 1];
  assert(strlen(node->name()) >= labelLength);
  memcpy(temp_buffer, node->name(), labelLength);
  temp_buffer[labelLength] = 0;

  m_labelView.setText(temp_buffer);

  if (node->type() == ScriptNode::Type::WithParentheses) {
    m_labelView.appendText(k_parentheses);
  }

  m_subLabelView.setText(node->description() != nullptr ? node->description() : "");
  reloadCell();
}

void ScriptNodeCell::setHighlighted(bool highlight) {
  TableCell::setHighlighted(highlight);
  KDColor backgroundColor = defaultBackgroundColor();
  m_labelView.setBackgroundColor(backgroundColor);
  m_subLabelView.setBackgroundColor(backgroundColor);
}

void ScriptNodeCell::reloadCell() {
  layoutSubviews();
  HighlightCell::reloadCell();
}

}