#ifndef POINCARE_LAYOUT_XNT_H
#define POINCARE_LAYOUT_XNT_H

#include <ion/unicode/utf8_decoder.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

bool FindXNTSymbol1D(UnicodeDecoder& decoder, char* buffer, size_t bufferSize,
                     int xntIndex, size_t* cycleSize);

bool FindXNTSymbol2D(const Tree* layout, const Tree* root, char* buffer,
                     size_t bufferSize, int xntIndex, size_t* cycleSize);

}  // namespace Poincare::Internal

#endif
