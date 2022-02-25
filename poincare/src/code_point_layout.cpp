#include <poincare/code_point_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <escher/metric.h>

namespace Poincare {

 // Draw the pixels of the middle dot symbols to make it thinner than font
 // Should be modified if font is modified.
void CodePointLayoutNode::FillDotBuffer(KDColor * buffer, int length, KDColor expressionColor, KDColor backgroundColor) {
  assert(length >= k_dotWidth * k_defaultFont->glyphSize().height());
  for (int i = 0 ; i < k_dotWidth ; i ++) {
    for (int j = 0 ; j < k_defaultFont->glyphSize().height() ; j++) {
      buffer[i+k_dotWidth*j] = backgroundColor;
    }
  }
  buffer[k_dotWidth / 2 + k_dotWidth * (k_defaultFont->glyphSize().height() / 2 - 1)] = expressionColor;
}

// LayoutNode
void CodePointLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->position() == LayoutCursor::Position::Right) {
    cursor->setPosition(LayoutCursor::Position::Left);
    return;
  }
  LayoutNode * parentNode = parent();
  if (parentNode != nullptr) {
    parentNode->moveCursorLeft(cursor, shouldRecomputeLayout);
  }
}

void CodePointLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->position() == LayoutCursor::Position::Left) {
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  LayoutNode * parentNode = parent();
  if (parentNode != nullptr) {
    parentNode->moveCursorRight(cursor, shouldRecomputeLayout);
  }
}

int CodePointLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::CodePoint(buffer, bufferSize, m_codePoint);
}

bool CodePointLayoutNode::isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const {
  if (*numberOfOpenParenthesis <= 0) {
    if (m_codePoint == '+'
        || m_codePoint == UCodePointRightwardsArrow
        || m_codePoint == '='
        || m_codePoint == '<'
        || m_codePoint == '>'
        || m_codePoint == UCodePointInferiorEqual
        || m_codePoint == UCodePointSuperiorEqual
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
        if (indexOfThis > 0 && goingLeft) {
          brother = parent.childAtIndex(indexOfThis-1);
        } else if (indexOfThis < parent.numberOfChildren() - 1 && !goingLeft) {
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

bool CodePointLayoutNode::canBeOmittedMultiplicationLeftFactor() const {
  if (isMultiplicationCodePoint()) {
    return false;
  }
  return LayoutNode::canBeOmittedMultiplicationLeftFactor();
}

bool CodePointLayoutNode::canBeOmittedMultiplicationRightFactor() const {
  if (m_codePoint == '!' || isMultiplicationCodePoint()) {
    return false;
  }
  return LayoutNode::canBeOmittedMultiplicationRightFactor();
}

// Sizing and positioning
KDSize CodePointLayoutNode::computeSize() {
  KDCoordinate totalHorizontalMargin = 0;
  if (m_displayType == DisplayType::Implicit) {
    totalHorizontalMargin += Escher::Metric::OperatorHorizontalMargin;
  } else if (m_displayType == DisplayType::Operator) {
    totalHorizontalMargin += 2 * Escher::Metric::OperatorHorizontalMargin;
  } else if (m_displayType == DisplayType::Thousand) {
    totalHorizontalMargin += Escher::Metric::ThousandsSeparatorWidth;
  }
  KDSize glyph = m_font->glyphSize();
  KDCoordinate width = glyph.width();
  if (m_codePoint == UCodePointMiddleDot && m_font == k_defaultFont) {
    width = k_dotWidth;
  }
  return KDSize(width + totalHorizontalMargin, glyph.height());
}

KDCoordinate CodePointLayoutNode::computeBaseline() {
  return m_font->glyphSize().height()/2;
}

void CodePointLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  if (m_displayType == DisplayType::Operator || m_displayType == DisplayType::Implicit) {
    p = p.translatedBy(KDPoint(Escher::Metric::OperatorHorizontalMargin, 0));
  }
  if (m_codePoint == UCodePointMiddleDot && m_font == k_defaultFont) {
    int width = k_dotWidth;
    int height = m_font->glyphSize().height();
    KDColor dotPixels[width * height];
    FillDotBuffer(dotPixels, width * height, expressionColor, backgroundColor);
    ctx->fillRectWithPixels(
        KDRect(p, KDSize(width, height)),
        dotPixels, dotPixels);
    return;
  }
  constexpr int bufferSize = sizeof(CodePoint)/sizeof(char) + 1; // Null-terminating char
  char buffer[bufferSize];
  SerializationHelper::CodePoint(buffer, bufferSize, m_codePoint);
  ctx->drawString(buffer, p, m_font, expressionColor, backgroundColor);
}

bool CodePointLayoutNode::isMultiplicationCodePoint() const {
  return m_codePoint == '*'
    || m_codePoint == UCodePointMultiplicationSign
    || m_codePoint == UCodePointMiddleDot;
}

bool CodePointLayoutNode::protectedIsIdenticalTo(Layout l) {
  assert(l.type() == Type::CodePointLayout || l.type() == Type::CombinedCodePointsLayout);
  CodePointLayout & cpl = static_cast<CodePointLayout &>(l);
  return codePoint() == cpl.codePoint() && font() == cpl.font();
}

void CodePointLayout::DistributeThousandDisplayType(Layout l, int start, int stop) {
  if (l.type() != LayoutNode::Type::HorizontalLayout
   || stop - start < 5) {
    /* Do not add a separator to a number with less than five digits.
     * i.e. : 12 345 but 1234 */
    return;
  }
  for (int i = stop - 4; i >= start; i -= 3) {
    assert(l.childAtIndex(i).type() == LayoutNode::Type::CodePointLayout);
    static_cast<CodePointLayoutNode *>(l.childAtIndex(i).node())->setDisplayType(CodePointLayoutNode::DisplayType::Thousand);
  }
}

CodePointLayout CodePointLayout::Builder(CodePoint c, const KDFont * font) {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(CodePointLayoutNode));
  CodePointLayoutNode * node = new (bufferNode) CodePointLayoutNode(c, font);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<CodePointLayout &>(h);
}

}
