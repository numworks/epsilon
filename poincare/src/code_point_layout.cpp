#include <poincare/code_point_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

int CodePointLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::CodePoint(buffer, bufferSize, m_codePoint);
}

bool CodePointLayoutNode::isCollapsable(int * numberOfOpenParenthesis, OMG::HorizontalDirection direction) const {
  if (*numberOfOpenParenthesis <= 0) {
    if (m_codePoint == '+'
        || m_codePoint == UCodePointRightwardsArrow
        || m_codePoint.isEquationOperator()
        || m_codePoint == ',')
    {
      return false;
    }
    if (m_codePoint == '-') {
      /* If the expression is like 3ᴇ-200, we want '-' to be collapsable.
       * Otherwise, '-' is not collapsable. */
      Layout thisRef = CodePointLayout(this);
      Layout parent = thisRef.parent();
      if (!parent.isUninitialized()) {
        int indexOfThis = parent.indexOfChild(thisRef);
        if (indexOfThis > 0) {
          Layout leftBrother = parent.childAtIndex(indexOfThis-1);
          if (leftBrother.type() == Type::CodePointLayout
              && static_cast<CodePointLayout&>(leftBrother).codePoint() == UCodePointLatinLetterSmallCapitalE)
          {
            return true;
          }
        }
      }
      return false;
    }
    if (isMultiplicationCodePoint()) {
      /* We want '*' to be collapsable only if the following brother is not a
       * fraction, so that the user can write intuitively "1/2 * 3/4". */
      Layout thisRef = CodePointLayout(this);
      Layout parent = thisRef.parent();
      if (!parent.isUninitialized()) {
        int indexOfThis = parent.indexOfChild(thisRef);
        Layout brother;
        if (indexOfThis > 0 && direction.isLeft()) {
          brother = parent.childAtIndex(indexOfThis-1);
        } else if (indexOfThis < parent.numberOfChildren() - 1 && direction.isRight()) {
          brother = parent.childAtIndex(indexOfThis+1);
        }
        if (!brother.isUninitialized() && brother.type() == LayoutNode::Type::FractionLayout) {
          return false;
        }
      }
    }
  }
  return true;
}

// Sizing and positioning
KDSize CodePointLayoutNode::computeSize(KDFont::Size font) {
  KDSize glyph = KDFont::GlyphSize(font);
  KDCoordinate width = glyph.width();

  // Handle the case of the middle dot which is thinner than the other glyphs
  if (m_codePoint == UCodePointMiddleDot) {
    width = k_middleDotWidth;
  }
  return KDSize(width, glyph.height());
}

KDCoordinate CodePointLayoutNode::computeBaseline(KDFont::Size font) {
  return KDFont::GlyphHeight(font)/2;
}

void CodePointLayoutNode::render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor) {
  // Handle the case of the middle dot which has to be drawn by hand since it is thinner than the other glyphs.
  if (m_codePoint == UCodePointMiddleDot) {
    int width = k_middleDotWidth;
    int height = KDFont::GlyphHeight(font);
    ctx->fillRect(KDRect(p, width, height), backgroundColor);
    ctx->fillRect(KDRect(p.translatedBy(KDPoint(width / 2, height / 2 - 1)), 1, 1), expressionColor);
    return;
  }
  // General case
  // Null-terminating char
  constexpr int bufferSize = sizeof(CodePoint)/sizeof(char) + 1;
  char buffer[bufferSize];
  SerializationHelper::CodePoint(buffer, bufferSize, m_codePoint);
  ctx->drawString(buffer, p, font, expressionColor, backgroundColor);
}

bool CodePointLayoutNode::isMultiplicationCodePoint() const {
  return m_codePoint == '*'
    || m_codePoint == UCodePointMultiplicationSign
    || m_codePoint == UCodePointMiddleDot;
}

bool CodePointLayoutNode::protectedIsIdenticalTo(Layout l) {
  assert(l.type() == Type::CodePointLayout || l.type() == Type::CombinedCodePointsLayout);
  CodePointLayout & cpl = static_cast<CodePointLayout &>(l);
  return codePoint() == cpl.codePoint();
}

CodePointLayout CodePointLayout::Builder(CodePoint c) {
  void * bufferNode = TreePool::sharedPool->alloc(sizeof(CodePointLayoutNode));
  CodePointLayoutNode * node = new (bufferNode) CodePointLayoutNode(c);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<CodePointLayout &>(h);
}

}
