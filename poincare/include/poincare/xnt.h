#ifndef POINCARE_XNT_H
#define POINCARE_XNT_H

#include <omg/utf8_decoder.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::XNT {

bool FindXNTSymbol1D(UnicodeDecoder& decoder, char* buffer, size_t bufferSize,
                     int xntIndex, size_t* cycleSize);

bool FindXNTSymbol2D(const Internal::Tree* layout, const Internal::Tree* root,
                     char* buffer, size_t bufferSize, int xntIndex,
                     size_t* cycleSize);

CodePoint CodePointAtIndexInDefaultCycle(int index, CodePoint startingCodePoint,
                                         size_t* cycleSize);

}  // namespace Poincare::XNT

#endif
