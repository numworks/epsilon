#ifndef POINCARE_EXPRESSION_POINT_OF_INTEREST_H
#define POINCARE_EXPRESSION_POINT_OF_INTEREST_H

#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

/* TODO_PCJ: This tree is a bit odd. It would be better if it was handled in the
 *           apps. Maybe we need a generic, fixed-sized tree that can be
 *           customized out of PCJ, but used within lists and storage. */

// | PI TAG | ABSCISSA | ORDINATE | DATA | INTEREST | INVERTED | SUBCURVEINDEX |
class PointOfInterest final {
 public:
  /* Abscissa/ordinate are from the function perspective, while x/y are related
   * to the drawings. They differ only with functions along y. */
  static double GetAbscissa(const Tree* tree) {
    return tree->nodeValueBlock(0)->get<double>();
  }
  static double GetOrdinate(const Tree* tree) {
    return tree->nodeValueBlock(k_ordinateValueIndex)->get<double>();
  }
  static uint32_t GetData(const Tree* tree) {
    return tree->nodeValueBlock(k_dataValueIndex)->get<uint32_t>();
  }
  static uint8_t GetInterest(const Tree* tree) {
    return tree->nodeValueBlock(k_interestValueIndex)->get<uint8_t>();
  }
  static bool IsInverted(const Tree* tree) {
    return tree->nodeValueBlock(k_invertedValueIndex)->get<bool>();
  }
  static uint8_t GetSubCurveIndex(const Tree* tree) {
    return tree->nodeValueBlock(k_subCurveIndexValueIndex)->get<uint8_t>();
  }

 private:
  constexpr static uint8_t k_abscissaValueIndex = 0;
  constexpr static uint8_t k_ordinateValueIndex =
      k_abscissaValueIndex + sizeof(double);
  constexpr static uint8_t k_dataValueIndex =
      k_ordinateValueIndex + sizeof(double);
  constexpr static uint8_t k_interestValueIndex =
      k_dataValueIndex + sizeof(uint32_t);
  constexpr static uint8_t k_invertedValueIndex =
      k_interestValueIndex + sizeof(uint8_t);
  constexpr static uint8_t k_subCurveIndexValueIndex =
      k_invertedValueIndex + sizeof(bool);
};

}  // namespace PoincareJ

#endif
