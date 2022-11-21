#ifndef POINCARE_EXPRESSION_CONSTANT_H
#define POINCARE_EXPRESSION_CONSTANT_H

#include <stdint.h>

namespace Poincare {

class Constant final {
public:
  enum class Type : uint8_t {
    Pi,
    E,
    Undefined
  };
  template<typename T>
  static T To(Type type) {
    switch (type) {
      case Constant::Type::Pi:
        return 3.14;
      case Constant::Type::E:
        return 2.72;
      default:
        assert(false);
    }
  }
};

}

#endif
