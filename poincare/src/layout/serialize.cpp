#include "serialize.h"

#include <poincare/src/expression/builtin.h>
#include <poincare/src/memory/pattern_matching.h>

#include <algorithm>

#include "autocompleted_pair.h"
#include "code_point_layout.h"
#include "grid.h"
#include "indices.h"
#include "parsing/tokenizer.h"
#include "vertical_offset.h"

namespace Poincare::Internal {

/* TODO Serialization needs to go through expressions when possible to be able
 * to use operator priority to minimize parentheses and get the correct
 * multiplication symbol. */

char* append(const char* text, char* buffer, char* end) {
  size_t len = std::min<size_t>(strlen(text), end - 1 - buffer);
  memcpy(buffer, text, len);
  return buffer + len;
}

Rack* rackForSerialization(const Rack* rack) {
  // Transform log^a(b) and a^log(b) in log(b,a)
  Tree* newRack = rack->cloneTree();
  /* Replace temporary parenthesis into non temporary parenthesis to avoid
   * duplicating pattern matching (temporary parentheses are serialized the
   * same as non temporary parentheses). */
  for (Tree* child : newRack->children()) {
    if (child->isParenthesesLayout() &&
        (AutocompletedPair::IsTemporary(child, Side::Left) ||
         AutocompletedPair::IsTemporary(child, Side::Right))) {
      child->cloneNodeOverNode(KParenthesesL);
    }
  }
  // log^a(b) -> log(b,a)
  PatternMatching::MatchReplace(
      newRack, KA_s ^ "log"_l ^ KSubscriptL(KB) ^ KParenthesesL(KC) ^ KD_s,
      KA_s ^ "log"_l ^ KParenthesesL(KC ^ ","_cl ^ KB) ^ KD_s);
  // a^log(b) -> log(b,a)
  PatternMatching::MatchReplace(
      newRack,
      KA_s ^ KPrefixSuperscriptL(KB) ^ "log"_l ^ KParenthesesL(KC) ^ KD_s,
      KA_s ^ "log"_l ^ KParenthesesL(KC ^ ","_cl ^ KB) ^ KD_s);
  assert(newRack->isRackLayout());
  return static_cast<Rack*>(newRack);
}

char* SerializeRack(const Rack* rack, char* buffer, char* end) {
  if (rack->numberOfChildren() == 0) {
    /* Text fields serializes layouts to insert them and we need an empty
     * codepoint for the cursor to be placed correctly in the text field.
     * TODO: should this behavior be behind a flag ? */
    buffer = append("\x11", buffer, end);
  }
  Tree* newRack = rackForSerialization(rack);
  for (const Tree* child : newRack->children()) {
    buffer = SerializeLayout(Layout::From(child), buffer, end,
                             rack->numberOfChildren() == 1);
    if (buffer == end) {
      newRack->removeTree();
      return end;
    }
  }
  newRack->removeTree();
  return buffer;
}

bool mayNeedParentheses(const Rack* rack) {
  ParsingContext parsingContext(nullptr,
                                ParsingContext::ParsingMethod::Classic);
  Tokenizer tokenizer(rack, &parsingContext);
  Token t = tokenizer.popToken();
  // Racks containing a single token should not need parentheses
  if (t.is(Token::Type::EndOfStream) || t.is(Token::Type::Layout)) {
    return true;
  }
  return !tokenizer.popToken().isEndOfStream();
}

char* serializeWithParentheses(const Rack* rack, char* buffer, char* end,
                               RackSerializer serializer,
                               bool forceParentheses = false) {
  bool addParentheses = forceParentheses || mayNeedParentheses(rack);
  if (addParentheses) {
    buffer = append("(", buffer, end);
  }
  buffer = serializer(rack, buffer, end);
  if (addParentheses) {
    buffer = append(")", buffer, end);
  }
  return buffer;
}

char* SerializeLayout(const Layout* layout, char* buffer, char* end,
                      bool isSingleRackChild, RackSerializer serializer) {
  switch (layout->layoutType()) {
    case LayoutType::CombinedCodePoints:
    case LayoutType::AsciiCodePoint:
    case LayoutType::UnicodeCodePoint: {
      // Special case for ≠
      if (CodePointLayout::IsCombinedCodePoint(
              layout, CodePoint('='), UCodePointCombiningLongSolidusOverlay)) {
        buffer = append("≠", buffer, end);
        break;
      }
      constexpr int bufferSize = sizeof(CodePoint) / sizeof(char) + 1;
      char codepointBuffer[bufferSize];
      CodePointLayout::CopyName(layout, codepointBuffer, bufferSize);
      buffer = append(codepointBuffer, buffer, end);
      break;
    }
    case LayoutType::Parentheses: {
      buffer = append("(", buffer, end);
      buffer = serializer(layout->child(0), buffer, end);
      buffer = append(")", buffer, end);
      break;
    }
    case LayoutType::CurlyBraces: {
      buffer = append("{", buffer, end);
      buffer = serializer(layout->child(0), buffer, end);
      buffer = append("}", buffer, end);
      break;
    }
    case LayoutType::Fraction: {
      if (!isSingleRackChild) {
        buffer = append("(", buffer, end);
      }
      buffer =
          serializeWithParentheses(layout->child(0), buffer, end, serializer);
      buffer = append("/", buffer, end);
      buffer =
          serializeWithParentheses(layout->child(1), buffer, end, serializer);
      if (!isSingleRackChild) {
        buffer = append(")", buffer, end);
      }
      break;
    }
    case LayoutType::VerticalOffset: {
      if (VerticalOffset::IsSuffixSuperscript(layout)) {
        buffer = append("^", buffer, end);
        buffer =
            serializeWithParentheses(layout->child(0), buffer, end, serializer);
      } else {
        /* TODO_PCJ: something else is needed here to translate the subscript.
         * For now, force parentheses around subscript expression */
        buffer = serializeWithParentheses(layout->child(0), buffer, end,
                                          serializer, true);
      }
      break;
    }
    case LayoutType::UnitSeparator:
    case LayoutType::OperatorSeparator:
    case LayoutType::ThousandsSeparator:
      break;
    case LayoutType::Matrix: {
      const Grid* grid = Grid::From(layout);
      buffer = append("[", buffer, end);
      for (int j = 0; j < grid->numberOfRows() - 1; j++) {
        buffer = append("[", buffer, end);
        for (int i = 0; i < grid->numberOfColumns() - 1; i++) {
          if (i > 0) {
            buffer = append(",", buffer, end);
          }
          buffer = serializer(grid->childAt(i, j), buffer, end);
        }
        buffer = append("]", buffer, end);
      }
      buffer = append("]", buffer, end);
      break;
    }
    case LayoutType::Diff: {
      buffer = append("diff(", buffer, end);
      buffer =
          serializer(layout->child(Derivative::k_derivandIndex), buffer, end);
      buffer = append(",", buffer, end);
      buffer =
          serializer(layout->child(Derivative::k_variableIndex), buffer, end);
      buffer = append(",", buffer, end);
      buffer =
          serializer(layout->child(Derivative::k_abscissaIndex), buffer, end);
      if (layout->toDiffLayoutNode()->isNthDerivative) {
        buffer = append(",", buffer, end);
        buffer =
            serializer(layout->child(Derivative::k_orderIndex), buffer, end);
      }
      buffer = append(")", buffer, end);
      break;
    }
    case LayoutType::Point2D: {
      buffer = append("(", buffer, end);
      buffer = serializer(layout->child(0), buffer, end);
      buffer = append(",", buffer, end);
      buffer = serializer(layout->child(1), buffer, end);
      buffer = append(")", buffer, end);
      break;
    }
    default: {
      if (layout->isPiecewiseLayout()) {
        buffer = append("piecewise", buffer, end);
      } else {
        const BuiltinWithLayout* builtin =
            BuiltinWithLayout::GetReservedFunction(layout->layoutType());
        if (!builtin) {
          assert(false);
          buffer = append("?", buffer, end);
        }
        buffer = append(builtin->aliases()->mainAlias(), buffer, end);
      }
      buffer = append("(", buffer, end);
      bool firstChild = true;
      for (IndexedChild<const Tree*> child : layout->indexedChildren()) {
        if (layout->isParametricLayout() &&
            child.index == layout->numberOfChildren() - 1) {
          break;
        }
        if (layout->isPiecewiseLayout() &&
            child.index == layout->numberOfChildren() - 2) {
          // last row is input row (contains 2 empty racks)
          break;
        }
        if (layout->isPiecewiseLayout() &&
            child.index == layout->numberOfChildren() - 3 &&
            child->numberOfChildren() == 0) {
          // the condition of the "real" last row is empty
          break;
        }
        if (!firstChild) {
          buffer = append(",", buffer, end);
        }
        if (layout->isParametricLayout() && child.index == 0) {
          buffer = serializer(Rack::From(layout->lastChild()), buffer, end);
          buffer = append(",", buffer, end);
        }
        buffer = serializer(Rack::From(child), buffer, end);
        firstChild = false;
      }
      buffer = append(")", buffer, end);
    }
  }
  return buffer;
}

}  // namespace Poincare::Internal
