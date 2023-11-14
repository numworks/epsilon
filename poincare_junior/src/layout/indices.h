#ifndef POINCARE_JUNIOR_LAYOUT_INDICES_H
#define POINCARE_JUNIOR_LAYOUT_INDICES_H

namespace PoincareJ {

namespace Fraction {
constexpr static int NumeratorIndex = 0;
constexpr static int DenominatorIndex = 1;
}  // namespace Fraction

namespace NthRoot {
constexpr static int RadicandIndex = 0;
constexpr static int IndexIndex = 1;
}  // namespace NthRoot

namespace Parametric {
constexpr static int VariableIndex = 0;
constexpr static int LowerBoundIndex = 1;
constexpr static int UpperBoundIndex = 2;
constexpr static int ArgumentIndex = 3;
}  // namespace Parametric

namespace Derivative {
constexpr static int VariableIndex = 0;
constexpr static int AbscissaIndex = 1;
constexpr static int DerivandIndex = 2;
constexpr static int OrderIndex = 3;

enum class VariableSlot : bool { Fraction, Assignment };
}  // namespace Derivative

namespace Integral {
constexpr static int DifferentialIndex = 0;
constexpr static int LowerBoundIndex = 1;
constexpr static int UpperBoundIndex = 2;
constexpr static int IntegrandIndex = 3;
}  // namespace Integral

namespace PtCombinatorics {
constexpr static int nIndex = 0;
constexpr static int kIndex = 1;
}  // namespace PtCombinatorics

namespace Binomial {
constexpr static int nIndex = 0;
constexpr static int kIndex = 1;
}  // namespace Binomial

namespace ListSequence {
constexpr static int FunctionIndex = 0;
constexpr static int VariableIndex = 1;
constexpr static int UpperBoundIndex = 2;
}  // namespace ListSequence

}  // namespace PoincareJ

#endif
