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
      default:
        child = SharedEditionPool->push<BlockType::CodePointLayout, CodePoint>(
            codePoint);
    }
    NAry::AddOrMergeChildAtIndex(parent, child, parent->numberOfChildren());
    codePoint = decoder->nextCodePoint();
  }
}

char *append(const char *text, char *buffer, char *end) {
  size_t len = std::min<size_t>(strlen(text), end - buffer);
  memcpy(buffer, text, len);
  return buffer + len;
}

char *Layout::Serialize(const Tree *layout, char *buffer, char *end) {
  switch (layout->layoutType()) {
    case LayoutType::CodePoint: {
      constexpr int bufferSize = sizeof(CodePoint) / sizeof(char) + 1;
      char codepointBuffer[bufferSize];
      CodePointLayout::GetName(layout, codepointBuffer, bufferSize);
      buffer = append(codepointBuffer, buffer, end);
      break;
    }
    case LayoutType::Parenthesis: {
      buffer = append("(", buffer, end);
      buffer = Serialize(layout->child(0), buffer, end);
      buffer = append(")", buffer, end);
      break;
    }
    case LayoutType::Rack: {
      for (const Tree *child : layout->children()) {
        buffer = Serialize(child, buffer, end);
        if (buffer == end) {
          return end;
        }
      }
      break;
    }
    case LayoutType::Fraction: {
      buffer = Serialize(layout->child(0), buffer, end);
      buffer = append("/", buffer, end);
      buffer = Serialize(layout->child(1), buffer, end);
      break;
    }
    case LayoutType::VerticalOffset: {
      buffer = append("^(", buffer, end);
      buffer = Serialize(layout->child(0), buffer, end);
      buffer = append(")", buffer, end);
      break;
    }
    default:
      buffer = append("?", buffer, end);
  }
  return buffer;
}

Layout Layout::Parse(const char *textInput) {
  return Layout([](const char *text) { EditionPoolTextToLayout(text); },
                textInput);
}

Layout Layout::FromExpression(const Expression *expr) {
  return Layout([](Tree *node) { Layoutter::LayoutExpression(node); }, expr);
}

void Layout::draw(KDContext *ctx, KDPoint p, KDFont::Size font,
                  KDColor expressionColor, KDColor backgroundColor,
                  LayoutSelection selection) const {
  void *context[] = {ctx,       &p, &font, &expressionColor, &backgroundColor,
                     &selection};
  send(
      [](const Tree *tree, void *context) {
        void **contextArray = static_cast<void **>(context);
        KDContext *ctx = static_cast<KDContext *>(contextArray[0]);
        KDPoint p = *static_cast<KDPoint *>(contextArray[1]);
        KDFont::Size font = *static_cast<KDFont::Size *>(contextArray[2]);
        KDColor expressionColor = *static_cast<KDColor *>(contextArray[3]);
        KDColor backgroundColor = *static_cast<KDColor *>(contextArray[4]);
        LayoutSelection selection =
            *static_cast<LayoutSelection *>(contextArray[5]);
        Render::Draw(tree, ctx, p, font, expressionColor, backgroundColor,
                     selection);
      },
      &context);
}

KDSize Layout::size(KDFont::Size font) const {
  KDSize result = KDSizeZero;
  void *context[2] = {&font, &result};
  send(
      [](const Tree *tree, void *context) {
        void **contextArray = static_cast<void **>(context);
        KDFont::Size font = *static_cast<KDFont::Size *>(contextArray[0]);
        KDSize *result = static_cast<KDSize *>(contextArray[1]);
        *result = Render::Size(tree);
      },
      &context);
  return result;
}

bool Layout::isEmpty() const {
  bool result = false;
  send(
      [](const Tree *tree, void *context) {
        bool *result = static_cast<bool *>(context);
        *result = IsEmpty(tree);
      },
      &result);
  return result;
}

}  // namespace PoincareJ
