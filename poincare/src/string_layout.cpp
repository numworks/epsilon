#include <poincare/string_layout.h>
#include <algorithm>
#include <ion/unicode/utf8_helper.h>
#include <poincare/layout_helper.h>

namespace Poincare {

StringLayoutNode::StringLayoutNode(const char * string, int stringSize, const KDFont * font) :
  LayoutNode(),
  m_font(font),
  m_displayType(CodePointLayoutNode::DisplayType::None)
  {
    m_stringLength = strlcpy(m_string, string, stringSize);
  }

Layout StringLayoutNode::makeEditable() {
  return StringLayout(this).makeEditable();
}

int StringLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return strlcpy(buffer, m_string, bufferSize);
}

size_t StringLayoutNode::size() const {
  return sizeof(StringLayoutNode) + sizeof(char) * (m_stringLength + 1);
}

bool StringLayoutNode::protectedIsIdenticalTo(Layout l) {
  assert(l.type() == Type::StringLayout);
  StringLayout & sl = static_cast<StringLayout &>(l);
  return strncmp(m_string, sl.string(), std::max(m_stringLength + 1, sl.stringLength() + 1)) == 0;
}

// Sizing and positioning
KDSize StringLayoutNode::computeSize() {
  KDCoordinate totalHorizontalMargin = 0;
  KDSize glyph = m_font->glyphSize();

  return KDSize(UTF8Helper::StringGlyphLength(m_string) * glyph.width() + totalHorizontalMargin, glyph.height());
}

KDCoordinate StringLayoutNode::computeBaseline() {
  return m_font->glyphSize().height() / 2;
}

void StringLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  ctx->drawString(m_string, p, m_font, expressionColor, backgroundColor);
}

StringLayout StringLayout::Builder(const char * string, int stringSize, const KDFont * font) {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(StringLayoutNode) + sizeof(char) * stringSize);
  StringLayoutNode * node = new (bufferNode) StringLayoutNode(string, stringSize, font);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<StringLayout &>(h);
}

Layout StringLayout::makeEditable() {
  Layout editableLayout = LayoutHelper::StringToCodePointsLayout(string(), stringLength(), font());
  replaceWithInPlace(editableLayout);
  return editableLayout;
}

}
