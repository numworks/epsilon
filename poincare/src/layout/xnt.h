#ifndef POINCARE_LAYOUT_XNT_H
#define POINCARE_LAYOUT_XNT_H

#include <omg/utf8_decoder.h>
#include <poincare/src/memory/tree.h>

#include "layout_span_decoder.h"

namespace Poincare::Internal {

// TODO: move in helpers

bool ParameterText(LayoutSpanDecoder* varDecoder, const Layout** parameterStart,
                   size_t* parameterLength);

bool FindXNTSymbol1D(UnicodeDecoder& decoder, char* buffer, size_t bufferSize,
                     int xntIndex, size_t* cycleSize);

bool FindXNTSymbol2D(const Tree* layout, const Tree* root, char* buffer,
                     size_t bufferSize, int xntIndex, size_t* cycleSize);

CodePoint CodePointAtIndexInDefaultCycle(int index, CodePoint startingCodePoint,
                                         size_t* cycleSize);

}  // namespace Poincare::Internal

#endif
