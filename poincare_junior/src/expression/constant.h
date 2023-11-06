#ifndef POINCARE_EXPRESSION_CONSTANT_H
#define POINCARE_EXPRESSION_CONSTANT_H

#include <assert.h>
#include <poincare_junior/src/memory/tree.h>
#include <stdint.h>

namespace PoincareJ {

class Constant final {
 public:
  enum class Type : uint8_t { Pi = 0, E = 1, I = 2, Undefined = 3 };
  static enum Type Type(const Tree* node) {
    assert(node->isConstant());
    return static_cast<enum Type>(node->nodeValue(0));
  }
  constexpr static enum Type Type(char16_t name) {
    switch (name) {
      case 'e':
        return Type::E;
      case 'i':
        return Type::I;
      case u'π':
        return Type::Pi;
      default:
        return Type::Undefined;
    }
  }
  template <typename T>
  static T To(enum Type type) {
    switch (type) {
      case Type::Pi:
        return M_PI;
      case Type::I:
        return NAN;
      case Type::E:
        return M_E;
      default:
        assert(false);
    }
  }
  constexpr static CodePoint ToCodePoint(enum Type type) {
    switch (type) {
      case Type::Pi:
        return CodePoint(u'π');
      case Type::I:
        return CodePoint('i');
      case Type::E:
        return CodePoint('e');
      default:
        assert(false);
    }
  }
};

}  // namespace PoincareJ

#endif
