#ifndef POINCARE_EXPRESSION_CONSTANT_H
#define POINCARE_EXPRESSION_CONSTANT_H

#include <assert.h>
#include <poincare_junior/src/memory/node.h>
#include <stdint.h>

namespace PoincareJ {

class Constant final {
 public:
  enum class Type : uint8_t { Pi, E, Undefined };
  static enum Type Type(const Node node) {
    assert(node.type() == BlockType::Constant);
    return static_cast<enum Type>(
        static_cast<uint8_t>(*(node.block()->next())));
  }
  constexpr static enum Type Type(char16_t name) {
    switch (name) {
      case 'e':
        return Constant::Type::E;
      case u'π':
        return Constant::Type::Pi;
      default:
        return Constant::Type::Undefined;
    }
  }
  template <typename T>
  static T To(enum Type type) {
    switch (type) {
      case Constant::Type::Pi:
        return M_PI;
      case Constant::Type::E:
        return M_E;
      default:
        assert(false);
    }
  }
};

}  // namespace PoincareJ

#endif
