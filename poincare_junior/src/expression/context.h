#ifndef POINCARE_EXPRESSION_CONTEXT_H
#define POINCARE_EXPRESSION_CONTEXT_H

namespace PoincareJ {

enum class AngleUnit : uint8_t {
  Radian = 0,
  Degree,
  Gradian,
  LastAngleUnit = Gradian,
};
enum class ComplexFormat : uint8_t {
  Real = 0,
  Cartesian,
  Polar,
  LastComplexFormat = Polar,
};
enum class Strategy { Default, NumbersToFloat, ApproximateToFloat };
enum class UnitFormat : bool { Metric = 0, Imperial = 1 };

}  // namespace PoincareJ

#endif
