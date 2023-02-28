#include <poincare_junior/include/layout.h>
#include <poincare_junior/include/expression.h>
#include <poincare_junior/src/layout/render.h>
#include <ion/unicode/code_point.h>
#include <string.h>

namespace PoincareJ {

EditionReference Layout::EditionPoolTextToLayout(const char * text) {
  int n = strlen(text);
  EditionReference ref = EditionReference::Push<BlockType::RackLayout>(n);
  for (int i = 0; i < n; i++) {
    EditionReference::Push<BlockType::CodePointLayout, CodePoint>(text[i]);
  }
  return ref;
}

Layout Layout::Parse(const char * textInput) {
  return Layout([](const char * text) {
      EditionPoolTextToLayout(text);
    }, textInput);
}

void Layout::toText(char * buffer, size_t bufferSize) const {
  memcpy(buffer, "-1+2*3", bufferSize);
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

}
