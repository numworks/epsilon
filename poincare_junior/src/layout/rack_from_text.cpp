#include "rack_from_text.h"

#include <ion/unicode/utf8_decoder.h>

#include "../n_ary.h"
#include "code_point_layout.h"
#include "k_tree.h"

namespace PoincareJ {

void RackFromTextRec(UTF8Decoder *decoder, Tree *parent,
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
            SharedEditionPool->push<Type::ParenthesisLayout>(false, false);
        RackFromTextRec(decoder, KRackL()->clone(), child);
        break;
      }
      case ')':
        if (parentheses) {
          return;
        }
        // Insert ')' codepoint if it has no matching left parenthesis
#endif
      default:
        child = CodePointLayout::Push(codePoint);
    }
    NAry::AddOrMergeChildAtIndex(parent, child, parent->numberOfChildren());
    codePoint = decoder->nextCodePoint();
  }
}

Rack *RackFromText(const char *text) {
  Rack *root = Rack::From(KRackL()->clone());
  UTF8Decoder decoder(text);
  RackFromTextRec(&decoder, root, nullptr);
  return root;
}

}  // namespace PoincareJ
