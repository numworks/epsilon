#include "aliases.h"

#include <omg/unicode_helper.h>

namespace Poincare::Internal {

int Aliases::maxDifferenceWith(UnicodeDecoder* decoder) const {
  if (!hasMultipleAliases()) {
    return OMG::CompareDecoderWithNullTerminatedString(decoder,
                                                       m_formattedAliases);
  }
  int maxValueOfComparison = 0;
  for (const char* aliasInList : *this) {
    int tempValueOfComparison =
        OMG::CompareDecoderWithNullTerminatedString(decoder, aliasInList);
    if (tempValueOfComparison == 0) {
      return 0;
    }
    if (maxValueOfComparison < tempValueOfComparison ||
        maxValueOfComparison == 0) {
      maxValueOfComparison = tempValueOfComparison;
    }
  }
  return maxValueOfComparison;
}

int Aliases::maxDifferenceWith(LayoutSpan span) const {
  // TODO factorize with function above
  if (!hasMultipleAliases()) {
    return CompareLayoutSpanWithNullTerminatedString(span, m_formattedAliases);
  }
  int maxValueOfComparison = 0;
  for (const char* aliasInList : *this) {
    int tempValueOfComparison =
        CompareLayoutSpanWithNullTerminatedString(span, aliasInList);
    if (tempValueOfComparison == 0) {
      return 0;
    }
    if (maxValueOfComparison < tempValueOfComparison ||
        maxValueOfComparison == 0) {
      maxValueOfComparison = tempValueOfComparison;
    }
  }
  return maxValueOfComparison;
}

const char* Aliases::nextAlias(const char* currentPositionInAliases) const {
  if (!hasMultipleAliases()) {
    return nullptr;
  }
  assert(strlen(currentPositionInAliases) != 0);
  const char* beginningOfNextAlias =
      currentPositionInAliases + strlen(currentPositionInAliases) + 1;
  if (beginningOfNextAlias[0] == 0) {
    return nullptr;  // End of list
  }
  return beginningOfNextAlias;
}

}  // namespace Poincare::Internal
