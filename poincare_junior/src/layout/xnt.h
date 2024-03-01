#ifndef POINCARE_JUNIOR_LAYOUT_XNT_H
#define POINCARE_JUNIOR_LAYOUT_XNT_H

#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

bool FindXNTSymbol2D(const Tree* layout, const Tree* root, char* buffer,
                     size_t bufferSize, int xntIndex, size_t* cycleSize);

}  // namespace PoincareJ

#endif
