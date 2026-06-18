#include "dimension_checks.h"

#include "dimension.h"

namespace Poincare::Internal::Approximation {

template <typename ReturnType>
bool DimensionChecks(const Tree* e, const SymbolContext& symbolContext,
                     bool isListAllowed, bool optimize) {
  if (!optimize) {
    if (!Dimension::DeepCheck(e, symbolContext)) {
      return false;
    }
    if (!isListAllowed && Dimension::IsList(e, symbolContext)) {
      return false;
    }
    if (!DimensionMatchesReturnType<ReturnType>(
            Dimension::Get(e, symbolContext))) {
      return false;
    }
    return true;
  }
  COSTLY_ASSERT(Dimension::DeepCheck(e, symbolContext));
  if (!isListAllowed) {
    COSTLY_ASSERT(!Dimension::IsList(e, symbolContext));
  }
  COSTLY_ASSERT(
      DimensionMatchesReturnType<ReturnType>(Dimension::Get(e, symbolContext)));
  return true;
}

template <>
bool DimensionMatchesReturnType<Tree*>(Dimension dimension) {
  return !dimension.isNone();
}

template <>
bool DimensionMatchesReturnType<float>(Dimension dimension) {
  return dimension.isScalarOrUnit();
}
template <>
bool DimensionMatchesReturnType<double>(Dimension dimension) {
  return dimension.isScalarOrUnit();
}
template <>
bool DimensionMatchesReturnType<std::complex<float>>(Dimension dimension) {
  return dimension.isScalarOrUnit();
}
template <>
bool DimensionMatchesReturnType<std::complex<double>>(Dimension dimension) {
  return dimension.isScalarOrUnit();
}

template <>
bool DimensionMatchesReturnType<Coordinate2D<float>>(Dimension dimension) {
  return dimension.isPoint();
}
template <>
bool DimensionMatchesReturnType<Coordinate2D<double>>(Dimension dimension) {
  return dimension.isPoint();
}

template <>
bool DimensionMatchesReturnType<PointOrRealScalar<float>>(Dimension dimension) {
  return dimension.isPoint() || dimension.isScalarOrUnit();
}
template <>
bool DimensionMatchesReturnType<PointOrRealScalar<double>>(
    Dimension dimension) {
  return dimension.isPoint() || dimension.isScalarOrUnit();
}

template <>
bool DimensionMatchesReturnType<BooleanOrUndefined>(Dimension dimension) {
  return dimension.isBoolean();
}

template bool DimensionChecks<Tree*>(const Tree*, const SymbolContext&, bool,
                                     bool);
template bool DimensionChecks<float>(const Tree*, const SymbolContext&, bool,
                                     bool);
template bool DimensionChecks<double>(const Tree*, const SymbolContext&, bool,
                                      bool);
template bool DimensionChecks<std::complex<float>>(const Tree*,
                                                   const SymbolContext&, bool,
                                                   bool);
template bool DimensionChecks<std::complex<double>>(const Tree*,
                                                    const SymbolContext&, bool,
                                                    bool);
template bool DimensionChecks<Coordinate2D<float>>(const Tree*,
                                                   const SymbolContext&, bool,
                                                   bool);
template bool DimensionChecks<Coordinate2D<double>>(const Tree*,
                                                    const SymbolContext&, bool,
                                                    bool);
template bool DimensionChecks<PointOrRealScalar<float>>(const Tree*,
                                                        const SymbolContext&,
                                                        bool, bool);
template bool DimensionChecks<PointOrRealScalar<double>>(const Tree*,
                                                         const SymbolContext&,
                                                         bool, bool);
template bool DimensionChecks<BooleanOrUndefined>(const Tree*,
                                                  const SymbolContext&, bool,
                                                  bool);

}  // namespace Poincare::Internal::Approximation
