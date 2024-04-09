#ifndef POINCARE_XNT_HELPERS_H
#define POINCARE_XNT_HELPERS_H

#include <ion/unicode/utf8_decoder.h>
#include <poincare/old/poincare_expressions.h>

#include "symbol.h"

namespace Poincare {

namespace XNTHelpers {

// Cycles
constexpr int k_maxCycleSize = 5;
constexpr CodePoint k_defaultXNTCycle[] = {
    Symbol::k_cartesianSymbol,
    Symbol::k_sequenceSymbol,
    Symbol::k_parametricSymbol,
    Symbol::k_polarSymbol,
    UCodePointNull,
};
constexpr CodePoint k_defaultContinuousXNTCycle[] = {
    Symbol::k_cartesianSymbol,
    Symbol::k_parametricSymbol,
    Symbol::k_polarSymbol,
    UCodePointNull,
};
constexpr CodePoint k_defaultDiscreteXNTCycle[] = {
    'k',
    'n',
    UCodePointNull,
};

CodePoint CodePointAtIndexInDefaultCycle(int index, CodePoint startingCodePoint,
                                         size_t* cycleSize);
CodePoint CodePointAtIndexInCycle(int index, const CodePoint* cycle,
                                  size_t* cycleSize);

}  // namespace XNTHelpers

}  // namespace Poincare

#endif
