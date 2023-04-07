#include <poincare_junior/include/layout.h>
#include <poincare_junior/include/expression.h>
#include <poincare_junior/src/layout/p_pusher.h>
#include <poincare_junior/src/layout/render.h>
#include <poincare_junior/src/n_ary.h>
#include <ion/unicode/code_point.h>
#include <string.h>

namespace PoincareJ {

EditionReference Layout::EditionPoolTextToLayout(const char * text) {
  int n = strlen(text);
  const EditionReference ref = P_RACKL();
  EditionReference currentLayout = ref;
  for (int i = 0; i < n; i++) {
    EditionReference child;
    switch (text[i]) {
    case '(':
      child = EditionReference::Push<BlockType::ParenthesisLayout>();
      P_RACKL();
      NAry::AddOrMergeChildAtIndex(currentLayout, child, currentLayout.numberOfChildren());
      currentLayout = child.childAtIndex(0);
      continue;
    case UCodePointEmpty:
      child = P_RACKL();
      break;
    case ')':
      if (!currentLayout.parent().isUninitialized() &&
          !currentLayout.parent().parent().isUninitialized() &&
          currentLayout.parent().parent().type() == BlockType::RackLayout) {
        currentLayout = currentLayout.parent().parent();
        continue;
      }
      // Jump to default case
    default:
      child = EditionReference::Push<BlockType::CodePointLayout, CodePoint>(text[i]);
      break;
    }
    NAry::AddOrMergeChildAtIndex(currentLayout, child, currentLayout.numberOfChildren());
  }
  return ref;
}

Layout Layout::Parse(const char * textInput) {
  return Layout([](const char * text) {
      EditionPoolTextToLayout(text);
    }, textInput);
}

size_t Layout::toText(char * buffer, size_t bufferSize) const {
  size_t layoutTextLength = strlen("-1+2*3");
  assert(bufferSize > layoutTextLength);
  memcpy(buffer, "-1+2*3", bufferSize);
  return layoutTextLength;
}

void Layout::draw(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor) const {
  void * context[5] = {ctx, &p, &font, &expressionColor, &backgroundColor};
  send(
    [](const Node tree, void * context) {
      void ** contextArray = static_cast<void **>(context);
      KDContext * ctx = static_cast<KDContext *>(contextArray[0]);
      KDPoint p = *static_cast<KDPoint *>(contextArray[1]);
      KDFont::Size font = *static_cast<KDFont::Size *>(contextArray[2]);
      KDColor expressionColor = *static_cast<KDColor *>(contextArray[3]);
      KDColor backgroundColor = *static_cast<KDColor *>(contextArray[4]);
      Render::Draw(tree, ctx, p, font, expressionColor, backgroundColor);
    },
    &context
  );
}

KDSize Layout::size(KDFont::Size font) const {
  KDSize result = KDSizeZero;
  void * context[2] = {&font, &result};
  send(
    [](const Node tree, void * context) {
      void ** contextArray = static_cast<void **>(context);
      KDFont::Size font = *static_cast<KDFont::Size *>(contextArray[0]);
      KDSize * result = static_cast<KDSize *>(contextArray[1]);
      *result = Render::Size(tree, font);
    },
    &context
  );
  return result;
}

bool Layout::isEmpty() const {
  bool result = false;
  send(
    [](const Node tree, void * context) {
      bool  * result = static_cast<bool *>(context);
      *result = IsEmpty(tree);
    },
    &result
  );
  return result;
}

}
