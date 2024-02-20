#include <ion/unicode/code_point.h>
#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/layout/code_point_layout.h>
#include <poincare_junior/src/layout/k_tree.h>
#include <poincare_junior/src/layout/layout_selection.h>
#include <poincare_junior/src/layout/layoutter.h>
#include <poincare_junior/src/layout/render.h>
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/n_ary.h>
#include <string.h>

namespace PoincareJ {

Tree *Layout::EditionPoolTextToLayout(const char *text) {
  Tree *root = KRackL()->clone();
  UTF8Decoder decoder(text);
  EditionPoolTextToLayoutRec(&decoder, root, nullptr);
  return root;
}

void Layout::EditionPoolTextToLayoutRec(UTF8Decoder *decoder, Tree *parent,
                                        const Tree *parentheses) {
  CodePoint codePoint = decoder->nextCodePoint();
  assert(parent && parent->isNAry());
  assert(!parentheses || parentheses->layoutType() == LayoutType::Parenthesis);
  while (codePoint != UCodePointNull) {
    Tree *child;
    switch (codePoint) {
      case UCodePointEmpty:
        child = KRackL()->clone();
        break;
#if 0
      // TODO PCJ: renable and treat braces the same way
      case '(': {
        /* Insert a ParenthesisLayout even if there are no matching right
         * parenthesis */
        child =
            SharedEditionPool->push<BlockType::ParenthesisLayout>(false, false);
        EditionPoolTextToLayoutRec(decoder, KRackL()->clone(), child);
        break;
      }
      case ')':
        if (parentheses) {
          return;
        }
        // Insert ')' codepoint if it has no matching left parenthesis
#endif
      default:
        child = SharedEditionPool->push<BlockType::CodePointLayout, CodePoint>(
            codePoint);
    }
    NAry::AddOrMergeChildAtIndex(parent, child, parent->numberOfChildren());
    codePoint = decoder->nextCodePoint();
  }
}

LayoutReference LayoutReference::Parse(const char *textInput) {
  return LayoutReference(
      [](const char *text) { EditionPoolTextToLayout(text); }, textInput);
}

LayoutReference LayoutReference::FromExpression(const Expression *expr) {
  return LayoutReference([](Tree *node) { Layoutter::LayoutExpression(node); },
                         expr);
}

void LayoutReference::draw(KDContext *ctx, KDPoint p, KDFont::Size font,
                           KDColor expressionColor, KDColor backgroundColor,
                           LayoutSelection selection) const {
  Render::Draw(getTree(), ctx, p, font, expressionColor, backgroundColor,
               nullptr);
}

KDSize LayoutReference::size(KDFont::Size font) const {
  return Render::Size(getTree(), font);
}

}  // namespace PoincareJ
