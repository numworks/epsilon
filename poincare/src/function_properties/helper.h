#pragma once

namespace Poincare::Internal {

class Tree;

double PositiveModulo(double i, double n);

void RemoveConstantTermsInAddition(Tree* e, const char* symbol);

bool DetectLinearPatternOfTrig(const Tree* e, const char* symbol, double* a,
                               double* b, double* c, bool acceptConstantTerm);

}  // namespace Poincare::Internal
