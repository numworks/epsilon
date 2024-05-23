#ifndef POINCARE_LAYOUT_LATEX_PARSER_LATEX_TO_LAYOUT_H
#define POINCARE_LAYOUT_LATEX_PARSER_LATEX_TO_LAYOUT_H

#include <poincare/src/layout/rack.h>
#include <poincare/src/memory/tree_ref.h>
#include <poincare/src/memory/type_block.h>

#include <array>

namespace Poincare::Internal {

namespace LatexParser {

class LatexToLayout {
 public:
  static Tree* Parse(const char* latexString);

 private:
  struct MatchingIdentifierPairs {
    const char* left;
    const char* right;
    Type type;
  };

  constexpr static MatchingIdentifierPairs k_identifiersPairs[] = {
      {"\\left(", "\\right)", Type::ParenthesisLayout},
      {"\\left|", "\\right|", Type::AbsLayout},
  };

  constexpr static size_t k_nPairs = std::size(k_identifiersPairs);

  static void ParseOnRackUntil(Rack* parent, const char* latexString,
                               const char* stringEnd);
  static void ParseSpecialIdentifier(Rack* parent, const char** latexString,
                                     const char* stringEnd);
  static const char* FindIdentifier(const char* identifier, const char* string,
                                    const char* stringEnd);
};

}  // namespace LatexParser

}  // namespace Poincare::Internal

#endif
