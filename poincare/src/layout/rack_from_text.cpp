#include "rack_from_text.h"

#include <omg/utf8_decoder.h>
#include <poincare/src/memory/n_ary.h>

#include "code_point_layout.h"
#include "k_tree.h"

namespace Poincare::Internal {

void RackFromTextRec(UTF8Decoder* decoder, Tree* parent,
                     const Tree* parentheses) {
  assert(parent && parent->isNAry());
  assert(!parentheses || parentheses->layoutType() == LayoutType::Parentheses);
  CodePoint codePoint = decoder->nextCodePoint();
  while (codePoint != UCodePointNull) {
    Tree* child;
    switch (codePoint) {
      case UCodePointEmpty:
        child = KRackL()->cloneTree();
        break;
#if 0
      // TODO_PCJ: renable and treat braces the same way
      case '(': {
        /* Insert a ParenthesisLayout even if there are no matching right
         * parenthesis */
        child =
            SharedTreeStack->pushParenthesisLayout(false, false);
        RackFromTextRec(decoder, KRackL()->cloneTree(), child);
        break;
      }
      case ')':
        if (parentheses) {
          return;
        }
        // Insert ')' codepoint if it has no matching left parenthesis
#endif
      default:
        if (codePoint.isCombining()) {
          // This codepoint should be combined with the previous one
          assert(parent->numberOfChildren() > 0);
          Tree* lastChild = parent->lastChild();
          assert(lastChild->isCodePointLayout());
          CodePoint previousCodePoint =
              CodePointLayout::GetCodePoint(lastChild);
          lastChild->moveTreeOverTree(
              CodePointLayout::PushCombined(previousCodePoint, codePoint));
          codePoint = decoder->nextCodePoint();
          continue;
        }
        child = CodePointLayout::Push(codePoint);
    }
    NAry::AddOrMergeChildAtIndex(parent, child, parent->numberOfChildren());
    codePoint = decoder->nextCodePoint();
  }
}

Rack* RackFromText(const char* start, const char* end) {
  Rack* root = Rack::From(KRackL()->cloneTree());
  UTF8Decoder decoder(start, start, end);
  RackFromTextRec(&decoder, root, nullptr);
  return root;
}

}  // namespace Poincare::Internal
